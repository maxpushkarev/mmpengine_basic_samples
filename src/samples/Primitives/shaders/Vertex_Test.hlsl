#include "../../../../libs/mmpengine/src/Backend/Dx12/Shader.h"
#include "../TestShader.h"

VertexOut EntryPoint(VertexIn vin)
{
	VertexOut vout;

	float4 worldPos = mul(float4(vin.pos, 1.0f), meshRendererData.worldMat);
    float4 viewPos = mul(worldPos, cameraData.viewMat);
    vout.pos = mul(viewPos, cameraData.projMat);

	return vout;
}