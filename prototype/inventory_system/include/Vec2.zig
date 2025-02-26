const std = @import("std");

pub export fn print_hello() void {
    std.debug.print("Hello from Vec2.zig\n", .{});
}
