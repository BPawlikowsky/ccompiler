// To standardize development, maybe you should use `lazyDependency()` instead of `dependency()`
// more info to see: https://ziglang.org/download/0.12.0/release-notes.html#toc-Lazy-Dependencies
const std = @import("std");
const Target = std.Target;

// zig fmt: off
pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "analyze",
        .root_source_file = b.path("src/analyze.zig"),
        .target = b.standardTargetOptions(
            .{
                .default_target = Target.Query{
                    .cpu_model = Target.Query.CpuModel.native
                }
            }
        )
    });

    b.installArtifact(exe);
}
// add module
