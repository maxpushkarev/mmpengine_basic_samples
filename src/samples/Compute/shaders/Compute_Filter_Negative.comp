#version 450

layout(std430, set = 0, binding = 0) buffer InputBuffer {
    int inputData[];
};

layout(std430, set = 0, binding = 1) buffer OutputBuffer {
    int outputData[];
};

layout(std430, set = 0, binding = 2) buffer OutputBuffer_Counter {
   uint currentStateCounter;
};

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main() {
	const int inputValue = inputData[gl_GlobalInvocationID.x];
	if (inputValue < 0)
	{
		uint i = atomicAdd(currentStateCounter, 1);
		outputData[i] = inputValue;
	}
}