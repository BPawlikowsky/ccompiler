const std = @import("std");
const os = std.os;
const fs = std.fs;

const Set = struct {
    name: string,
    elements: []string,
    fn init(self: std.Self, list: std.ArrayList(string)) std.Self {
        self.name = list.items[0];
        self.elements = list.items[1..];
        return self;
    }
};

const string = []const u8;

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    const allocator = arena.allocator();
    const argv = try std.process.argsAlloc(allocator);

    if (argv.len < 1) {
        std.debug.print("Too few arguments...", .{});
    }

    // Load args
    std.debug.print("len: {d}", .{os.argv.len});
    const firstset_filename: string = argv[1];
    const followset_filename: string = argv[2];
    const nonterminal_count: string = argv[3];

    // Load files
    const dir = fs.cwd();
    const firstset_file = try dir.openFile(firstset_filename, .{});
    const followset_file = try dir.openFile(followset_filename, .{});
    const firstset_file_stats = try firstset_file.stat();
    const followset_file_stats = try followset_file.stat();

    // Read files to buffer
    const firstset = try firstset_file.readToEndAlloc(allocator, @sizeOf(string) * firstset_file_stats.size);
    const followset = try followset_file.readToEndAlloc(allocator, @sizeOf(string) * followset_file_stats.size);
    std.debug.print("nonterminal count: {s}\n", .{nonterminal_count});

    var firstset_lines_split = std.mem.splitSequence(u8, firstset, "\n");
    var followset_lines_split = std.mem.splitSequence(u8, followset, "\n");

    var firstsets = try std.ArrayList(Set).initCapacity(allocator, firstset.len);
    var followsets = try std.ArrayList(Set).initCapacity(allocator, followset.len);

    // construct first sets
    while (firstset_lines_split.next()) |line| {
        var list = try std.ArrayList(string).initCapacity(allocator, 100);
        var split = std.mem.splitSequence(u8, line, "\'");
        while (split.next()) |terminal| {
            try list.append(allocator, terminal);
        }
        const set_struct = filter_list(list, allocator);
        try firstsets.append(allocator, try set_struct);
    }

    // construct follow sets
    while (followset_lines_split.next()) |line| {
        var list = try std.ArrayList(string).initCapacity(allocator, 100);
        var split = std.mem.splitSequence(u8, line, "\'");
        while (split.next()) |terminal| {
            try list.append(allocator, terminal);
        }
        const set_struct = filter_list(list, allocator);
        try followsets.append(allocator, try set_struct);
    }

    for (firstsets.items) |item| {
        std.debug.print("\n{s}\n", .{item.name});
        for (item.elements) |terminal| {
            std.debug.print("{s}, ", .{terminal});
        }
    }

    firstFollowCheck(firstsets, followsets);
}

fn filter_list(list: std.ArrayList(string), allocator: std.mem.Allocator) !Set {
    var filtered_list = try std.ArrayList([]const u8).initCapacity(allocator, list.capacity);

    for (list.items) |item| {
        const a = std.mem.eql(u8, item, " | ");
        const b = std.mem.containsAtLeast(u8, item, 1, "FirstSet");
        const c = std.mem.eql(u8, item, ":  ");
        const d = (item.len > 1);
        if (!a and !b and !c and d) {
            try filtered_list.append(allocator, item);
        }
    }
    std.log.debug("Items len: {d}", .{filtered_list.items.len});
    if (filtered_list.items.len > 0) {
        var set: Set = undefined;
        if (filtered_list.items.len > 1) {
            const elements = filtered_list.items[1 .. filtered_list.items.len - 1];
            set = Set{ .name = filtered_list.items[0], .elements = elements };
        } else {
            const elements = filtered_list.items[0..];
            set = Set{ .name = filtered_list.items[0], .elements = elements };
        }
        return set;
    }
    const dummy = try allocator.alloc(string, 1);
    dummy[0] = "";
    return Set{ .elements = dummy, .name = "" };
}

fn firstFollowCheck(firstsets: std.ArrayList(Set), followsets: std.ArrayList(Set)) void {
    for (firstsets.items) |firstset| {
        std.debug.print("{s}:\n", .{firstset.name});
        for (firstset.elements, 0..) |element, i| {
            const be = for (followsets.items[i].elements) |value| {
                if (std.mem.eql(u8, element, value)) {
                    break true;
                }
            } else false;
            std.debug.print("{s}({d}): {}\n", .{ element, element.len, be });
        }
    }
}
