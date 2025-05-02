#version 450

layout(set = 0, binding = 0) buffer Buffer {
    uint outputData[];
};

layout(set = 1, binding = 0) uniform GlobalUniforms0 {
    uint multiplier;
} ubo0;

layout(set = 1, binding = 1) uniform GlobalUniforms1 {
    uint addition;
} ubo1;

void main() {
	outputData[gl_GlobalInvocationID.x] = gl_GlobalInvocationID.x * ubo0.multiplier + ubo1.addition;
}