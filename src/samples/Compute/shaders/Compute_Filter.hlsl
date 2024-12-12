#include "../../../../libs/mmpengine/src/Backend/Dx12/Shader.h"

StructuredBuffer<int> input : register(t0);
//AppendStructuredBuffer<int> output : register(u0);
RWStructuredBuffer<int> output: register(u0);

[numthreads(128, 1, 1)]
void EntryPoint(uint3 tId : SV_DispatchThreadId)
{
	/*const int inputValue = input[tId.x];
	if (inputValue > 0)
	{
		output.Append(inputValue);
	}*/

	output[tId.x] = input[tId.x];
}