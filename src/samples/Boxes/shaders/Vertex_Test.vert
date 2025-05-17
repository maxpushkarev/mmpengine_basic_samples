#version 450
#include "../../../../libs/mmpengine/src/Backend/Vulkan/Shader.h"
#include "../TestShader.h"

layout(location = 0) in vec3 inPosition;

void main() {
	vec4 worldPos = objectData.meshRendererData.worldMat * vec4(inPosition, 1.0f);
	vec4 viewPos = sceneData.cameraData.viewMat * worldPos;
	gl_Position = sceneData.cameraData.projMat * viewPos;
}