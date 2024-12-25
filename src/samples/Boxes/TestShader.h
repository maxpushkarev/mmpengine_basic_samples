#ifndef BOXES_TEST_SHADER
#define BOXES_TEST_SHADER 1
#include "../../../libs/mmpengine/src/Frontend/Shader.h"

#if MMPENGINE_HLSL

cbuffer cbPerCamera : register(b0)
{
	CameraData cameraData;
};

cbuffer cbPerObject : register(b1)
{
	MeshRendererData meshRendererData;
};

struct VertexIn
{
	float3 pos : POSITION;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
};

#endif

#endif