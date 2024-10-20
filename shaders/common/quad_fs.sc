$input v2f_texCoord0

#include "../utils/bgfx_shader.sh"

void main() {
	gl_FragColor = vec4(v2f_texCoord0, 0.0, 1.0);
}
