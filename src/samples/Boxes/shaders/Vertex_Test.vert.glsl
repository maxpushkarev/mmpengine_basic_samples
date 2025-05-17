#version 450
#include "../../../../libs/mmpengine/src/Backend/Vulkan/Shader.h"
//#include "../TestShader.h"

/*layout(std430, set = 0, binding = 0) buffer Buffer {
    uint outputData[];
};

layout(std140, set = 1, binding = 0) uniform GlobalUniforms0 {
    uint multiplier;
} ubo0;

layout(std140, set = 1, binding = 1) uniform GlobalUniforms1 {
    uint addition;
} ubo1;*/

void main() {
	//outputData[gl_GlobalInvocationID.x] = gl_GlobalInvocationID.x * ubo0.multiplier + ubo1.addition;
}