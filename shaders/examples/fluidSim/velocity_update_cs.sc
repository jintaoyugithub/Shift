#include "../../utils/bgfx_compute.sh"

BUFFER_RO(prevVelocityFieldX, vec4, 0);
BUFFER_RO(prevVelocityFieldY, vec4, 1);
BUFFER_WO(curVelocityFieldX, vec4, 2);
BUFFER_WO(curVelocityFieldY, vec4, 3);

NUM_THREADS(1, 1, 1)
void main() {
    
}