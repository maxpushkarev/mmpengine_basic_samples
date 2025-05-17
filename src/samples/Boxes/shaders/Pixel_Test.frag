#version 450
#include "../../../../libs/mmpengine/src/Backend/Vulkan/Shader.h"
#include "../TestShader.h"

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
}