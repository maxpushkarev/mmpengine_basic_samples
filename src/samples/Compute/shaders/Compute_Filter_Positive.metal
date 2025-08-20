#include <metal_stdlib>
using namespace metal;

kernel void FilterPositive(
    device const int*     inputData  [[ buffer(0) ]],
    device int*    outputData        [[ buffer(1) ]],
    device atomic_uint& outputCounter        [[ buffer(2) ]],
    uint3  gid   [[ thread_position_in_grid ]]
)
{
    const int inputValue = inputData[gid.x];
    if (inputValue >= 0)
    {
        uint i = atomic_fetch_add_explicit(&outputCounter, 1, memory_order_relaxed);
        outputData[i] = inputValue;
    }
}
