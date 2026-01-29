const c_lib = @cImport({
    @cInclude("/Users/bartsky/dev/c/ccompiler/src/tools/settools.h");
    @cInclude("/Users/bartsky/dev/c/ccompiler/src/tools/firstset.h");
    @cInclude("/Users/bartsky/dev/c/ccompiler/src/tools/followset.h");
});
const std = @import("std");
const expect = std.testing.expect;
const c_allocator = std.heap.c_allocator;

const FirstSet = c_lib.FirstSet;
const FollowSet = c_lib.FollowSet;
const Definition = c_lib.Definition;
const Production = c_lib.Production;
const Statement = c_lib.Statement;
const GeneratorState = c_lib.GeneratorState;
const FirstSetHistory = c_lib.FirstSetHistory;

// Tests the utility function that checks if a FirstSet contains the special "epsilon" symbol.
// This is critical for LL(1) nullability checks to determine if a non-terminal can derive an empty string.
test "settools - doesSetContainEpsilon" {
    const items = try c_allocator.alloc([*c]u8, 2);
    defer c_allocator.free(items);

    const set = FirstSet{
        .definition_name = @constCast("test"),
        .itemCount = 2,
        .set = items.ptr,
    };

    set.set[0] = @constCast("id");
    set.set[1] = @constCast("epsilon");

    try expect(c_lib.doesSetContainEpsilon(@constCast(&set)) == true);

    set.set[1] = @constCast("else");
    try expect(c_lib.doesSetContainEpsilon(@constCast(&set)) == false);
}

// Verifies the string representation of a FirstSet.
// This ensures the debug output and set files (firstset.txt) are formatted correctly
// for manual auditing and analysis tools.
test "settools - get_first_set_string" {
    const items = try c_allocator.alloc([*c]u8, 2);
    defer c_allocator.free(items);

    const set = FirstSet{
        .definition_name = @constCast("test"),
        .itemCount = 2,
        .set = items.ptr,
    };

    set.set[0] = @constCast("id");
    set.set[1] = @constCast("+");

    const res = c_lib.get_first_set_string(@constCast(&set));
    const actual = std.mem.sliceTo(res, 0);
    try expect(std.mem.containsAtLeast(u8, actual, 1, "'id'"));
    try expect(std.mem.containsAtLeast(u8, actual, 1, "'+'"));
}

// Tests the core LL(1) logic for non-terminal nullability propagation.
// Given a production A -> B C, where B is nullable (can derive epsilon),
// the First(A) set must correctly include symbols from First(B) AND First(C).
// This verifies the algorithm properly continues scanning the production.
test "firstset - epsilon propagation" {
    // Mocking: A -> B C, B -> epsilon | b, C -> c
    // FirstSet(A) = {b, c}

    // 1. Statements
    var stmt_b = Statement{ .type = c_lib.TERMINAL, .content = @constCast("b"), .stringLength = 2 };
    var stmt_eps = Statement{ .type = c_lib.TERMINAL, .content = @constCast("epsilon"), .stringLength = 8 };
    var stmt_c = Statement{ .type = c_lib.TERMINAL, .content = @constCast("c"), .stringLength = 2 };
    var stmt_B_nt = Statement{ .type = c_lib.NONTERMINAL, .content = @constCast("B"), .stringLength = 2 };
    var stmt_C_nt = Statement{ .type = c_lib.NONTERMINAL, .content = @constCast("C"), .stringLength = 2 };

    // 2. Productions
    // B -> b | epsilon
    const prodB1_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodB1_stmts[0] = &stmt_b;
    var prodB1 = Production{ .statementCount = 1, .statements = prodB1_stmts.ptr };
    const prodB2_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodB2_stmts[0] = &stmt_eps;
    var prodB2 = Production{ .statementCount = 1, .statements = prodB2_stmts.ptr };
    const defB_prods = try c_allocator.alloc([*c]Production, 2);
    defB_prods[0] = &prodB1;
    defB_prods[1] = &prodB2;
    var defB = Definition{ .name = @constCast("B"), .productionCount = 2, .productions = defB_prods.ptr };

    // C -> c
    const prodC_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodC_stmts[0] = &stmt_c;
    var prodC = Production{ .statementCount = 1, .statements = prodC_stmts.ptr };
    const defC_prods = try c_allocator.alloc([*c]Production, 1);
    defC_prods[0] = &prodC;
    var defC = Definition{ .name = @constCast("C"), .productionCount = 1, .productions = defC_prods.ptr };

    // A -> B C
    const prodA_stmts = try c_allocator.alloc([*c]Statement, 2);
    prodA_stmts[0] = &stmt_B_nt;
    prodA_stmts[1] = &stmt_C_nt;
    var prodA = Production{ .statementCount = 2, .statements = prodA_stmts.ptr };
    const defA_prods = try c_allocator.alloc([*c]Production, 1);
    defA_prods[0] = &prodA;
    var defA = Definition{ .name = @constCast("A"), .productionCount = 1, .productions = defA_prods.ptr };

    // 3. State
    const definitions = try c_allocator.alloc([*c]Definition, 3);
    definitions[0] = &defA;
    definitions[1] = &defB;
    definitions[2] = &defC;

    const firstSets = try c_allocator.alloc([*c]FirstSet, 3);
    for (firstSets) |*s| s.* = null;

    const visited_counts = try c_allocator.alloc([*c]c_int, 3);
    var count0: c_int = 0;
    var count1: c_int = 0;
    var count2: c_int = 0;
    visited_counts[0] = &count0;
    visited_counts[1] = &count1;
    visited_counts[2] = &count2;

    const history_sets = try c_allocator.alloc([*c]FirstSet, 3);
    for (history_sets) |*s| s.* = null;

    var history = FirstSetHistory{ .arr_sets = history_sets.ptr, .arr_visited_count = visited_counts.ptr };

    const defs_names = try c_allocator.alloc([*c]u8, 3);
    defs_names[0] = @constCast("A");
    defs_names[1] = @constCast("B");
    defs_names[2] = @constCast("C");

    var state = GeneratorState{
        .defs = defs_names.ptr,
        .definitions = definitions.ptr,
        .defCount = 3,
        .first_set_history = &history,
        .historyCounter = 0,
        .firstSets = firstSets.ptr,
        .firstSetCounter = 0,
        .followSets = null,
        .followSetCounter = 0,
        .terminalCount = 0,
        .terminals = null,
        .nonterminalCount = 3,
        .nonterminals = null,
    };

    // 4. Test
    _ = c_lib.getFirstSet(1, &state); // B
    _ = c_lib.getFirstSet(2, &state); // C
    const resA = c_lib.getFirstSet(0, &state); // A

    try expect(resA.*.itemCount == 2);
    try expect(std.mem.eql(u8, std.mem.sliceTo(resA.*.set[0], 0), "b"));
    try expect(std.mem.eql(u8, std.mem.sliceTo(resA.*.set[1], 0), "c"));
}

// Verifies that the Follow set correctly propagates across multiple non-terminals.
// In the grammar S -> A B, where B is nullable, Follow(A) must contain First(B) (excluding epsilon)
// and also inherit Follow(S). This ensures recursive set dependencies are handled.
test "followset - complex propagation" {
    // Mocking:
    // S -> A B
    // A -> a | epsilon
    // B -> b | epsilon
    // Follow(A) = First(B) \ {epsilon} U Follow(S) = {b, $}
    // Follow(B) = Follow(S) = {$}

    // Statements
    var stmt_a = Statement{ .type = c_lib.TERMINAL, .content = @constCast("a"), .stringLength = 2 };
    var stmt_b = Statement{ .type = c_lib.TERMINAL, .content = @constCast("b"), .stringLength = 2 };
    var stmt_eps = Statement{ .type = c_lib.TERMINAL, .content = @constCast("epsilon"), .stringLength = 8 };
    var stmt_A_nt = Statement{ .type = c_lib.NONTERMINAL, .content = @constCast("A"), .stringLength = 2 };
    var stmt_B_nt = Statement{ .type = c_lib.NONTERMINAL, .content = @constCast("B"), .stringLength = 2 };

    // Productions
    // A -> a | epsilon
    const prodA1_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodA1_stmts[0] = &stmt_a;
    var prodA1 = Production{ .statementCount = 1, .statements = prodA1_stmts.ptr };
    const prodA2_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodA2_stmts[0] = &stmt_eps;
    var prodA2 = Production{ .statementCount = 1, .statements = prodA2_stmts.ptr };
    const defA_prods = try c_allocator.alloc([*c]Production, 2);
    defA_prods[0] = &prodA1;
    defA_prods[1] = &prodA2;
    var defA = Definition{ .name = @constCast("A"), .productionCount = 2, .productions = defA_prods.ptr };

    // B -> b | epsilon
    const prodB1_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodB1_stmts[0] = &stmt_b;
    var prodB1 = Production{ .statementCount = 1, .statements = prodB1_stmts.ptr };
    const prodB2_stmts = try c_allocator.alloc([*c]Statement, 1);
    prodB2_stmts[0] = &stmt_eps;
    var prodB2 = Production{ .statementCount = 1, .statements = prodB2_stmts.ptr };
    const defB_prods = try c_allocator.alloc([*c]Production, 2);
    defB_prods[0] = &prodB1;
    defB_prods[1] = &prodB2;
    var defB = Definition{ .name = @constCast("B"), .productionCount = 2, .productions = defB_prods.ptr };

    // S -> A B
    const prodS_stmts = try c_allocator.alloc([*c]Statement, 2);
    prodS_stmts[0] = &stmt_A_nt;
    prodS_stmts[1] = &stmt_B_nt;
    var prodS = Production{ .statementCount = 2, .statements = prodS_stmts.ptr };
    const defS_prods = try c_allocator.alloc([*c]Production, 1);
    defS_prods[0] = &prodS;
    var defS = Definition{ .name = @constCast("S"), .productionCount = 1, .productions = defS_prods.ptr };

    // State setup
    const definitions = try c_allocator.alloc([*c]Definition, 3);
    definitions[0] = &defS;
    definitions[1] = &defA;
    definitions[2] = &defB;

    // First sets
    const itemsfsA = try c_allocator.alloc([*c]u8, 2);
    itemsfsA[0] = @constCast("a");
    itemsfsA[1] = @constCast("epsilon");
    var fsA = FirstSet{ .itemCount = 2, .set = itemsfsA.ptr };
    const itemsfsB = try c_allocator.alloc([*c]u8, 2);
    itemsfsB[0] = @constCast("b");
    itemsfsB[1] = @constCast("epsilon");
    var fsB = FirstSet{ .itemCount = 2, .set = itemsfsB.ptr };
    const itemsfsS = try c_allocator.alloc([*c]u8, 2);
    itemsfsS[0] = @constCast("a");
    itemsfsS[1] = @constCast("b"); // simplified
    var fsS = FirstSet{ .itemCount = 2, .set = itemsfsS.ptr };

    const firstSets = try c_allocator.alloc([*c]FirstSet, 3);
    firstSets[0] = &fsS;
    firstSets[1] = &fsA;
    firstSets[2] = &fsB;

    // Follow sets
    const followSets = try c_allocator.alloc([*c]FollowSet, 3);
    for (followSets) |*s| s.* = null;
    const itemsfollowS = try c_allocator.alloc([*c]u8, 1);
    itemsfollowS[0] = @constCast("$");
    var followS = FollowSet{ .itemCount = 1, .set = itemsfollowS.ptr };
    followSets[0] = &followS;

    const visited_counts = try c_allocator.alloc([*c]c_int, 3);
    var counts = [_]c_int{ 0, 0, 0 };
    for (visited_counts, 0..) |*v, i| v.* = &counts[i];

    const history_sets = try c_allocator.alloc([*c]FollowSet, 3);
    history_sets[0] = &followS;
    history_sets[1] = null;
    history_sets[2] = null;

    var history = c_lib.FollowSetHistory{ .arr_sets = history_sets.ptr, .arr_visited_count = visited_counts.ptr };

    const defs_names = try c_allocator.alloc([*c]u8, 3);
    defs_names[0] = @constCast("S");
    defs_names[1] = @constCast("A");
    defs_names[2] = @constCast("B");

    var state = GeneratorState{
        .defs = defs_names.ptr,
        .definitions = definitions.ptr,
        .defCount = 3,
        .firstSets = firstSets.ptr,
        .followSets = followSets.ptr,
        .follow_set_history = &history,
        .nonterminalCount = 3,
    };

    // Test Follow(A)
    const resA = c_lib.getFollowSet(1, @constCast(&state));
    try expect(resA.*.itemCount == 2);
    // Should contains 'b' from First(B) and '$' from Follow(S)
    var hasB = false;
    var hasDollar = false;
    for (resA.*.set[0..2]) |item| {
        if (std.mem.eql(u8, std.mem.sliceTo(item, 0), "b")) hasB = true;
        if (std.mem.eql(u8, std.mem.sliceTo(item, 0), "$")) hasDollar = true;
    }
    try expect(hasB);
    try expect(hasDollar);
}
