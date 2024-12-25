#include "../../../../libs/mmpengine/src/Backend/Dx12/Shader.h"
#include "../TestShader.h"

float4 EntryPoint(VertexOut pin) : SV_Target
{
	float4 res = float4(1.0f, 0.0f, 0.0f, 0.0f);
	return res;
}
