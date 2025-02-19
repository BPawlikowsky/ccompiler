const c_lib = @cImport({
    @cInclude("./src/tools/lexer.h");
    @cInclude("./src/types.h");
    @cInclude("string.h");
    @cInclude("regex.h");
});
const std = @import("std");
const expect = std.testing.expect;
const c_allocator = std.heap.c_allocator;

const Token = c_lib.Token;
const Regex = c_lib.Regex;
const TokenType = c_lib.TokenType;

fn wrapInConstPtr(value: anyopaque) anyopaque {
    return @ptrCast(@constCast(value));
}

// test setup
var def = Regex{ .expression = @ptrCast(@constCast("^[a-zA-Z]+[-_a-zA-Z]*:")) };
var prod = Regex{ .expression = @ptrCast(@constCast("^<p>[[:print:]]*<\\/p>")) };
var comment = Regex{ .expression = @ptrCast(@constCast("^\\/\\/[[:print:]]+")) };
var whitespace = Regex{ .expression = @ptrCast(@constCast("^[ \t]+")) };
var newline = Regex{ .expression = @ptrCast(@constCast("^[\n]")) };

const lexicon = [_]*Regex{ &def, &prod, &comment, &whitespace, &newline };
const tokenType = enum(c_uint) { DEF = 0, PROD, COMMENT, WHITESPACE, NEWLINE };
const stringTokenType = [_][]const u8{ "DEF", "PROD", "COMMENT", "WHITESPACE", "NEWLINE" };
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

test "lexer - given valid data, return valid tokens" {
    const tokens = try c_allocator.alloc(Token, 10);
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
        try expect(std.mem.eql(u8, @typeName(@TypeOf(token)), @typeName(Token)));
        try expect(expected_tokens[i].type == token.type);
        try expect(std.mem.eql(
            u8,
            expected_tokens[i].content,
            std.mem.sliceTo(token.content, 0),
        ));
    }
}

test "lexer - given empty input string, return valid tokens" {
    const tokens = try c_allocator.alloc(Token, 10);
    const buffer = "";

    const count: c_int = 5;
    const token_count = c_lib.lexer(@ptrCast(@constCast(buffer)), tokens.ptr, @ptrCast(@constCast(&lexicon)), @ptrCast(@constCast(&tokenTypes)), count);

    const usize_token_count: usize = @intCast(token_count);
    for (tokens[0..usize_token_count], 0..) |token, i| {
        try expect(std.mem.eql(u8, @typeName(@TypeOf(token)), @typeName(Token)));
        try expect(expected_tokens[i].type == token.type);
        try expect(std.mem.eql(
            u8,
            expected_tokens[i].content,
            std.mem.sliceTo(token.content, 0),
        ));
    }
}

test "compileRegex - given a lexicon with valid regex, return success(0)" {
    const result = c_lib.compileRegex(@ptrCast(@constCast(&lexicon)), 5);
    try expect(result == 0);
}

test "compileRegex - given a lexicon with invalid regex, return error with message" {
    const expression = @constCast("[a-z");
    var regex = Regex{ .r = c_lib.regex_t{}, .expression = @ptrCast(expression) };
    const bad_lexicon = [_]*Regex{&regex};
    const result = c_lib.compileRegex(@ptrCast(@constCast(&bad_lexicon)), 1);
    std.debug.print("result: {d}\n", .{result});
    try expect(result != 0);
}
