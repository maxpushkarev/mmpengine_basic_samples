#include <metal_stdlib>
using namespace metal;

struct GlobalUniforms0
{
    uint multiplier;
};

struct GlobalUniforms1
{
    uint addition;
};

kernel void VertexTest(
    device uint*                 outputData  [[ buffer(0) ]],
    constant GlobalUniforms0&    ubo0        [[ buffer(1) ]],
    constant GlobalUniforms1&    ubo1        [[ buffer(2) ]],
    uint3                        gid         [[ thread_position_in_grid ]]
)
{
    outputData[gid.x] = gid.x * ubo0.multiplier + ubo1.addition;
}
