$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"

// buffer is a keywork in opengl, can not use as some common variable name
//BUFFER_RO(buffer, vec4, 0);
BUFFER_RO(buffers, vec4, 0);

void main() {
	gl_FragColor = buffers[int(gl_FragCoord.x)];
}
