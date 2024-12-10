#include "../../../../libs/mmpengine/src/Backend/Dx12/Shader.h"
RWStructuredBuffer<uint> output : register(u0);

[numthreads(128, 1, 1)]
void EntryPoint(uint3 tId : SV_DispatchThreadId)
{
	output[tId.x] = tId.x;
}