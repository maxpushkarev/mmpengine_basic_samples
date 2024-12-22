#include "../../../../libs/mmpengine/src/Backend/Dx12/Shader.h"

RWStructuredBuffer<uint> output : register(u0);
cbuffer cb1 : register(b0)
{
	uint multiplier;
};

cbuffer cb2 : register(b1)
{
	uint addition;
};

[numthreads(128, 1, 1)]
void EntryPoint(uint3 tId : SV_DispatchThreadId)
{
	output[tId.x] = tId.x * multiplier + addition;
}