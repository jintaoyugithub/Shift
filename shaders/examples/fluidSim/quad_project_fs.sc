$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

#define bufferSize 512

BUFFER_RO(_isFluid, float, 0);
BUFFER_RO(_curVelX, float, 1);
BUFFER_RO(_curVelY, float, 2);

vec3 HSVtoRGB(float H, float S, float V) {
    // Normalize the hue to [0, 360)
    if (H < 0.0) H += 360.0;
    if (H >= 360.0) H -= 360.0;

    float C = V * S;
    float X = C * (1.0 - abs(mod(H / 60.0, 2.0) - 1.0));
    float m = V - C;

    float r, g, b;

    if (H >= 0.0 && H < 60.0) {
        r = C; g = X; b = 0.0;
    } else if (H >= 60.0 && H < 120.0) {
        r = X; g = C; b = 0.0;
    } else if (H >= 120.0 && H < 180.0) {
        r = 0.0; g = C; b = X;
    } else if (H >= 180.0 && H < 240.0) {
        r = 0.0; g = X; b = C;
    } else if (H >= 240.0 && H < 300.0) {
        r = X; g = 0.0; b = C;
    } else {
        r = C; g = 0.0; b = X;
    }

    // Add m to the final result to adjust brightness
    r += m;
    g += m;
    b += m;

    // Return the final color in RGB
    return vec3(r, g, b);
}

float dir2Angle(float x, float y) {
  float angle = atan2(x, y) * 180.0f / 3.14159;
  if(angle < 0) angle += 360.0f;

  return angle;
}

void main() {
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);

    if(_isFluid[index] == 0) {
      gl_FragColor = vec4(0.2, 0.2, 0.2, 1.0);

    } else if(_isFluid[index] == 1) {
      if (abs(_curVelX[index]) < 1e-5 && abs(_curVelY[index]) < 1e-5) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); 
      } else {
        float maxVel = speedDeltaTime;

        vec2 velDir = vec2(_curVelX[index], _curVelY[index]);
        float angle = dir2Angle(velDir.y, velDir.x);
        float speed = length(velDir);

        float H = angle;
        float S = min(speed / maxVel, 1.0f);
        float V = min(speed / maxVel, 1.0f);

        float ratio = speed / 1.2f;

        vec3 color = HSVtoRGB(H, S, V);

        //gl_FragColor = vec4(color * ratio, 1.0);
        gl_FragColor = vec4(color, 1.0);
      }
    } else {
      gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
}
