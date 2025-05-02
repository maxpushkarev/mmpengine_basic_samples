#version 450

layout(set = 0, binding = 0) uniform GlobalUniforms {
    uint numElements;
    float someGlobalValue;
} ubo;

layout(set = 1, binding = 0) buffer InputBuffer {
    float inputData[];
};

layout(set = 1, binding = 1) buffer OutputBuffer {
    float outputData[];
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= ubo.numElements) {
        return;
    }
    outputData[idx] = inputData[idx] * ubo.someGlobalValue;
}