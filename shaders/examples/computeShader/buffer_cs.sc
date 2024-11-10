#include "../../utils/bgfx_compute.sh"

uniform vec4 test;

BUFFER_WO(buffers, vec4, 0);

NUM_THREADS(512, 1, 1)
void main() {
	// Note that the globalInvocationID is defined as workGroupID * workGroupSize + localInvocationID
	// workGroupID is defined by numWorkGroup declared by glDispatchCompute()
	// workGtoupSize is defined by NUM_THREADS() here
	// make sure globalInvocationID is match the size of the buffer
	vec4 keep = test;
	buffers[gl_GlobalInvocationID.x] = test;
}