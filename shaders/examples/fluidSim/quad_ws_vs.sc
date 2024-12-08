$input a_position, a_texcoord0
$output v2f_texCoord0, v2f_posWS

#include "../../utils/bgfx_shader.sh"

void main() {
	//gl_Position = vec4(a_position, 1.0);
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	v2f_texCoord0 = a_texcoord0;
    v2f_posWS = mul(u_model[0], vec4(a_position, 1.0)).xyz;
}
