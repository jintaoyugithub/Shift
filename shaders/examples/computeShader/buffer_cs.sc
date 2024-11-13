#include "../../utils/bgfx_compute.sh"

//BUFFER_WO(buffer, vec4, 0);
BUFFER_WO(buffers, vec4, 0);

NUM_THREADS(512, 1, 1)
void main() {
	buffers[gl_GlobalInvocationID.x] = vec4(0.2, 0.4, 0.7, 1.0);
}