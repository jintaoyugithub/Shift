$input v2f_texCoord0

#include "../utils/bgfx_shader.sh"

void main() {
	gl_FragColor = vec4(0.6, v2f_texCoord0, 1.0);
}

// Some notes
// Vertex/FragMent Shader can not include "[]", otherwise it will cause debugbreak, but works fine in relase mode
