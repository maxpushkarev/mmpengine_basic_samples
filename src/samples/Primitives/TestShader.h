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


#if MMPENGINE_GLSL

layout(std140, set = 0, binding = 0, column_major) uniform SceneData {
	CameraData cameraData;
} sceneData;

layout(std140, set = 1, binding = 0, column_major) uniform ObjectData {
	MeshRendererData meshRendererData;
} objectData;

#endif

#if MMPENGINE_MSL

struct VertexIn {
    float3 position [[attribute(0)]];
};

struct VertexOut {
    float4 position [[position]];
};

#endif

#endif
