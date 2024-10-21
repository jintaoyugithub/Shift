#include "../../utils/bgfx_compute.sh"

/**
* @brief declare write only buffer
* @parameter
* @parameter
* @parameter
*/
BUFFER_RW(colorBuffer, vec4, 0);

float hash(int index) {
	return fract(sin(index) * 43758.5453123);
}

vec4 randomColor(int index) {
	float x = hash(index);
	float y = hash(index+1);
	float z = hash(index+2);
	return vec4(x,y,z,1.0);
}

// specify work group size/thread num is necessary for a compute shader
NUM_THREADS(64, 1, 1)
void main() {
	// write a random color to the buffer
	//colorBuffer[gl_GlobalInvocationID.x] = randomColor(gl_LocalInvocationID.x);
	colorBuffer[gl_GlobalInvocationID.x] = vec4(0.0, 1.0, 0.0, 1.0);
}