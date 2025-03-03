const std = @import("std");

// Get the current timestamp in the format HH:MM:SS
var b: [9]u8 = undefined;
export fn getTimeStamp() callconv(.C) [*c]const u8 {
    const timestamp = std.time.timestamp() + (-6 * 3600);

    const seconds: u32 = @intCast(@mod(timestamp, 60));
    const minutes: u32 = @intCast(@mod(@divFloor(timestamp, 60), 60));
    const hours: u32 = @intCast(@mod(@divFloor(timestamp, 3600), 24));

    _ = std.fmt.bufPrint(&b, "{d:0>2}:{d:0>2}:{d:0>2}", .{ hours, minutes, seconds }) catch unreachable;

    return &b;
}
