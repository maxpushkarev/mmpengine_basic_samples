#version 450
#include "../../../../libs/mmpengine/src/Backend/Vulkan/Shader.h"
#include "../TestShader.h"

layout(location = 0) in vec3 inPosition;

void main() {
	vec4 worldPos = vec4(inPosition, 1.0f)* objectData.meshRendererData.worldMat;
	vec4 viewPos = worldPos * sceneData.cameraData.viewMat;
	gl_Position = viewPos * sceneData.cameraData.projMat;
}