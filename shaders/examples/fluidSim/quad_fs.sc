$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

#define bufferSize 512

BUFFER_RO(_isFluid, float, 0);


void main() {
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);

    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_FragColor = vec4(1-_isFluid[index].xxx, 1.0);
}
