const c_lib = @cImport({
    @cInclude("/Users/bartsky/dev/c/ccompiler/src/tools/lexer.h");
});
const std = @import("std");
const expect = std.testing.expect;
const c_allocator = std.heap.c_allocator;

const Token = c_lib.Token;
const Regex = c_lib.Regex;
const TokenType = c_lib.TokenType;

// test setup for generator lexicon
var def = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^[a-zA-Z]+[-_a-zA-Z]*:")) };
var prod = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^<p>[[:print:]]*<\\/p>")) };
var comment = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^\\/\\/[[:print:]]+")) };
var whitespace = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^[ \t]+")) };
var newline = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^[\n]")) };

const lexicon = [_]*Regex{ &def, &prod, &comment, &whitespace, &newline };
const tokenType = enum(c_uint) { DEF = 0, PROD, COMMENT, WHITESPACE, NEWLINE };
const tokenTypes = [_]c_uint{ @intFromEnum(tokenType.DEF), @intFromEnum(tokenType.PROD), @intFromEnum(tokenType.COMMENT), @intFromEnum(tokenType.WHITESPACE), @intFromEnum(tokenType.NEWLINE) };

// zig fmt: off
const ZToken = struct{ type: c_uint, content: []const u8};
const expected_tokens = [_]ZToken{
    ZToken{ .type = 0, .content = "aaa:" },
    ZToken{ .type = 4, .content = "\n" },
    ZToken{ .type = 1, .content = "<p>yyyyyyy</p>" },
    ZToken{ .type = 4, .content = "\n" },
    ZToken{ .type = 0, .content = "bbb:" },
    ZToken{ .type = 4, .content = "\n" },
    ZToken{ .type = 1, .content = "<p>aaa aaa</p>" }
};
// zig fmt: on

// Tests basic tokenization of a grammar-like string.
// This ensures the lexer correctly identifies definitions (DEF),
// productions (PROD), and newlines in a standard input format.
test "lexer - given valid data, return valid tokens" {
    const tokens = try c_allocator.alloc(Token, 10);
    defer c_allocator.free(tokens);
    const buffer =
        \\aaa:
        \\<p>yyyyyyy</p>
        \\bbb:
        \\<p>aaa aaa</p>
    ;

    const count: c_int = 5;
    const token_count = c_lib.lexer(@ptrCast(@constCast(buffer)), tokens.ptr, @ptrCast(@constCast(&lexicon)), @ptrCast(@constCast(&tokenTypes)), count);

    const usize_token_count: usize = @intCast(token_count);
    for (tokens[0..usize_token_count], 0..) |token, i| {
        try expect(expected_tokens[i].type == token.type);
        try expect(std.mem.eql(
            u8,
            expected_tokens[i].content,
            std.mem.sliceTo(token.content, 0),
        ));
    }
}

// Verifies that an empty input results in zero tokens.
// This ensures the lexer loop handles termination and empty buffers correctly.
test "lexer - given empty input string, return valid tokens" {
    const tokens = try c_allocator.alloc(Token, 10);
    defer c_allocator.free(tokens);
    const buffer = "";

    const count: c_int = 5;
    const token_count = c_lib.lexer(@ptrCast(@constCast(buffer)), tokens.ptr, @ptrCast(@constCast(&lexicon)), @ptrCast(@constCast(&tokenTypes)), count);

    try expect(token_count == 0);
}

// Ensures that the regex compilation step correctly initializes the regex_t structures.
// This is critical for the runtime performance and correctness of regexec calls.
test "compileRegex - given a lexicon with valid regex, return success(0)" {
    const result = c_lib.compileRegex(@ptrCast(@constCast(&lexicon)), 5);
    try expect(result == 0);
}

// Verifies that the compiler correctly detects and reports regex syntax errors.
// This ensures the lexer doesn't crash on malformed patterns and provides feedback.
test "compileRegex - given a lexicon with invalid regex, return error with message" {
    const expression = @constCast("[a-z");
    var regex = Regex{ .r = std.mem.zeroInit(c_lib.regex_t, .{}), .expression = @ptrCast(expression) };
    const bad_lexicon = [_]*Regex{&regex};
    const result = c_lib.compileRegex(@ptrCast(@constCast(&bad_lexicon)), 1);
    try expect(result != 0);
}

// Tests the "first-match" priority system of the lexer.
// It ensures that longer/more specific operators like '++' or '<<=' are matched
// before shorter prefixes like '+' or '<', provided they appear first in the lexicon.
test "lexer - operators and priority" {
    const tokens = try c_allocator.alloc(Token, 20);
    defer c_allocator.free(tokens);
    const buffer = "++ + -- - += = <<= << <";

    // Test priority: longest/specific patterns should match first if they appear first in lexicon
    var op_plus_plus = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^\\+\\+")) };
    var op_plus = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^\\+")) };
    var op_minus_minus = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^--")) };
    var op_minus = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^-")) };
    var op_plus_assign = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^\\+=")) };
    var op_assign = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^=")) };
    var op_shl_assign = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^<<=")) };
    var op_shl = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^<<")) };
    var op_lt = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^<")) };
    var ws = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^[ ]+")) };

    const ops_lexicon = [_]*Regex{ &op_plus_plus, &op_minus_minus, &op_plus_assign, &op_shl_assign, &op_shl, &op_plus, &op_minus, &op_assign, &op_lt, &ws };
    const ops_types = [_]c_uint{ 10, 10, 10, 10, 10, 10, 10, 10, 10, 11 };

    const token_count = c_lib.lexer(@ptrCast(@constCast(buffer)), tokens.ptr, @ptrCast(@constCast(&ops_lexicon)), @ptrCast(@constCast(&ops_types)), 10);

    const expected = [_][]const u8{ "++", " ", "+", " ", "--", " ", "-", " ", "+=", " ", "=", " ", "<<=", " ", "<<", " ", "<" };
    const usize_token_count: usize = @intCast(token_count);

    for (tokens[0..usize_token_count], 0..) |token, i| {
        try expect(std.mem.eql(u8, expected[i], std.mem.sliceTo(token.content, 0)));
    }
}

// Verifies that the lexer correctly handles complex multiline comment patterns.
// It ensures that newlines and asterisks within the comment block are correctly consumed
// and that the lexer resumes accurate tokenization immediately following the comment.
test "lexer - multiline comments and whitespace" {
    const tokens = try c_allocator.alloc(Token, 10);
    defer c_allocator.free(tokens);
    const buffer = "/* line 1\n   line 2 */ int x;";

    var mlc = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^\\/\\*([^*]|\\*+[^*/])*\\*+\\/")) };
    var ident = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^[a-z]+")) };
    var ws_nl = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^[ \t\n]+")) };
    var sc = Regex{ .r = undefined, .expression = @ptrCast(@constCast("^;")) };

    const lexicon_mlc = [_]*Regex{ &mlc, &ws_nl, &ident, &sc };
    const types_mlc = [_]c_uint{ 1, 11, 8, 10 };

    const token_count = c_lib.lexer(@ptrCast(@constCast(buffer)), tokens.ptr, @ptrCast(@constCast(&lexicon_mlc)), @ptrCast(@constCast(&types_mlc)), 4);

    try expect(token_count == 6);
    try expect(std.mem.eql(u8, "/* line 1\n   line 2 */", std.mem.sliceTo(tokens[0].content, 0)));
    try expect(std.mem.eql(u8, " ", std.mem.sliceTo(tokens[1].content, 0)));
    try expect(std.mem.eql(u8, "int", std.mem.sliceTo(tokens[2].content, 0)));
}
