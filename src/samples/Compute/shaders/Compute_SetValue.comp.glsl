#version 450

layout(std430, set = 0, binding = 0) buffer Buffer {
    uint outputData[];
};

layout(std140, set = 1, binding = 0) uniform GlobalUniforms0 {
    uint multiplier;
} ubo0;

layout(std140, set = 1, binding = 1) uniform GlobalUniforms1 {
    uint addition;
} ubo1;

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main() {
	outputData[gl_GlobalInvocationID.x] = gl_GlobalInvocationID.x * ubo0.multiplier + ubo1.addition;
}