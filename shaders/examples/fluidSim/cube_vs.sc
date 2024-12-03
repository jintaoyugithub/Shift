$input a_position, a_texcoord0
$output v2f_posMS, v2f_posWS, v2f_texCoord0

#include "../../utils/bgfx_shader.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	//v2f_posMS = mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	v2f_posMS = a_position.xyz;
    v2f_posWS = mul(u_model[0], vec4(a_position, 1.0)).xyz;
    v2f_texCoord0 = a_texcoord0;
}
