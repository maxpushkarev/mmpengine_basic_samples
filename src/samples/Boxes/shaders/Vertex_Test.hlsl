#include "../../../../libs/mmpengine/src/Backend/Dx12/Shader.h"
#include "../TestShader.h"

VertexOut EntryPoint(VertexIn vin)
{
	VertexOut vout;

	float4 worldPos = mul(meshRendererData.worldMat, float4(vin.pos, 1.0f));
	float4 viewPos = mul(cameraData.viewMat, worldPos);
	vout.pos = mul(cameraData.projMat, viewPos);

	return vout;
}