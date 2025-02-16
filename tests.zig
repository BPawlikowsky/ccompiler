const c_lib = @cImport({
    @cInclude("./src/tools/lexer.h");
    @cInclude("./src/types.h");
    @cInclude("string.h");
});
const std = @import("std");
const expect = std.testing.expect;
const c_allocator = std.heap.c_allocator;

const Token = c_lib.Token;
const Regex = c_lib.Regex;
const TokenType = c_lib.TokenType;

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
const expected_tokens = [_]Token{
    Token{ .type = 0, .content = @ptrCast(@constCast("aaa:")) },
    Token{ .type = @as(u8,TokenType.NEWLINE), .content = "\n" },
    Token{ .type = .PROD, .content = "<p>yyyyyyy</p>" },
    Token{ .type = .NEWLINE, .content = "\n" },
    Token{ .type = .DEF, .content = "bbb:" },
    Token{ .type = .NEWLINE, .content = "\n" },
    Token{ .type = .PROD, .content = "<p>aaa aaa</p>" }
};
// zig fmt: on

test "aaa" {
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
        // const e: TokenType = token.type;
        // std.debug.print("token count: {d}\n", .{token_count});
        // std.debug.print("actual type: {s}\n", .{@typeName(@TypeOf(token))});
        // std.debug.print(".type = .{s}\n", .{stringTokenType[e]});
        // std.debug.print(".content = \"{s}\"\n", .{token.content});
        // std.debug.print("Token type: {s}\n", .{@typeName(Token)});
        try expect(std.mem.eql(u8, @typeName(@TypeOf(token)), @typeName(Token)));
        try expect(expected_tokens[i].type == token.type);
    }
}
