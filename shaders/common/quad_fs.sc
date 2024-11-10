$input v2f_texCoord0

#include "../utils/bgfx_compute.sh"

uniform vec4 test2;

BUFFER_RO(buffers, vec4, 0);

void main() {
	//gl_FragColor = vec4(0.6, v2f_texCoord0, 1.0);
	vec4 keep = test2;
	gl_FragColor = buffers[int(test2.x)];
}

//Some notes
// Vertex/FragMent Shader can not include "[]", otherwise it will cause debugbreak, but works fine in relase mode
