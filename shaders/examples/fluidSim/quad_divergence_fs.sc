$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

#define bufferSize 512

BUFFER_RO(_divergence, float, 0);
BUFFER_RO(_isFluid, float, 1);

vec4 DebugDispActive(float isFluid) {
  vec4 color;

  if(isFluid == 0) {
    color = vec4(1.0, 1.0, 1.0, 1.0);
  } else if (isFluid == -1) {
    color = vec4(0.0, 1.0, 0.0, 1.0);
  } else {
    color = vec4(0.2, 0.2, 0.2, 1.0);
  }

  return color;
}

vec4 DebugDispDiv(float div) {
  vec4 color;

  if(div < 0) {
    color = vec4(-div, 0.0, 0.0, 1.0);
  } else if(div > 0) {
    color = vec4(0.0, div, 0.0, 1.0);
  } else {
    color = vec4(0.0, 0.0, 0.5, 1.0);
  }

  return color;
}

void main() {
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);

    vec4 divColor = DebugDispDiv(_divergence[index]);
    vec4 actColor = DebugDispActive(_isFluid[index]);

    gl_FragColor = divColor;
    //gl_FragColor = actColor;
}
