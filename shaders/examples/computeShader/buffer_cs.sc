#include "../../utils/bgfx_compute.sh"

BUFFER_WO(sss, vec4, 0);

NUM_THREADS(1, 1, 1)
void main() {
	sss[gl_GlobalInvocationID.x] = vec4(1.0, 0.0, 0.0, 1.0);
}