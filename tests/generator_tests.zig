const c_lib = @cImport({
    @cInclude("/Users/bartsky/dev/c/ccompiler/src/LL1/parsingTable.h");
    @cInclude("/Users/bartsky/dev/c/ccompiler/src/LL1/parserGeneratorUtils.h");
});
const std = @import("std");
const expect = std.testing.expect;
const c_allocator = std.heap.c_allocator;

const ParsingTable = c_lib.ParsingTable;
const TableEntry = c_lib.TableEntry;
const Production = c_lib.Production;
const Statement = c_lib.Statement;

// Tests the utility that converts a Production struct into a human-readable string.
// This ensures that the CSV export and conflict logs accurately represent the grammar rules.
test "parsingTable - productionAsString" {
    var stmt1 = Statement{ .type = c_lib.TERMINAL, .content = @constCast("id"), .stringLength = 3 };
    var stmt2 = Statement{ .type = c_lib.TERMINAL, .content = @constCast("+"), .stringLength = 2 };

    var stmts = try c_allocator.alloc([*c]Statement, 2);
    defer c_allocator.free(stmts);
    stmts[0] = &stmt1;
    stmts[1] = &stmt2;

    var prod = Production{ .statementCount = 2, .statements = stmts.ptr };

    const res = c_lib.productionAsString(&prod);
    const actual = std.mem.sliceTo(res, 0);
    try expect(std.mem.containsAtLeast(u8, actual, 1, "id"));
    try expect(std.mem.containsAtLeast(u8, actual, 1, "+"));
}

// Tests the logic for filling the LL(1) parsing table and detecting conflicts.
// It ensures that a production is correctly placed in a table cell (row/col)
// and that subsequent attempts to fill the same cell increment the overwrite counter,
// which is the primary indicator of a non-LL(1) grammar.
test "parsingTable - addItemToTable and overwrites" {
    // 1. Setup Table
    var table = ParsingTable{
        .terminalCount = 2,
        .nonterminalCount = 1,
        .tableSize = 2,
        .terminals = (try c_allocator.alloc([*c]u8, 2)).ptr,
        .nonterminals = (try c_allocator.alloc([*c]u8, 1)).ptr,
        .table = (try c_allocator.alloc([*c]TableEntry, 2)).ptr,
    };
    defer c_allocator.free(table.terminals[0..2]);
    defer c_allocator.free(table.nonterminals[0..1]);
    defer c_allocator.free(table.table[0..2]);

    table.terminals[0] = @constCast("id");
    table.terminals[1] = @constCast("+");
    table.nonterminals[0] = @constCast("E");

    var entry1 = TableEntry{ .row = -1, .col = -1, .production = null, .@"error" = true };
    var entry2 = TableEntry{ .row = -1, .col = -1, .production = null, .@"error" = true };
    table.table[0] = &entry1;
    table.table[1] = &entry2;

    // 2. Setup Productions
    var stmt_id = Statement{ .type = c_lib.TERMINAL, .content = @constCast("id"), .stringLength = 3 };
    var stmts1 = try c_allocator.alloc([*c]Statement, 1);
    stmts1[0] = &stmt_id;
    var prod1 = Production{ .statementCount = 1, .statements = stmts1.ptr };

    var stmt_plus = Statement{ .type = c_lib.TERMINAL, .content = @constCast("+"), .stringLength = 2 };
    var stmts2 = try c_allocator.alloc([*c]Statement, 1);
    stmts2[0] = &stmt_plus;
    var prod2 = Production{ .statementCount = 1, .statements = stmts2.ptr };

    // 3. Test addItemToTable
    const added = c_lib.addItemToTable(0, 0, &prod1, &table);
    try expect(added == true);
    try expect(table.table[0].*.@"error" == false);

    const added_conflict = c_lib.addItemToTable(0, 0, &prod2, &table);
    try expect(added_conflict == false);

    c_allocator.free(stmts1);
    c_allocator.free(stmts2);
}
