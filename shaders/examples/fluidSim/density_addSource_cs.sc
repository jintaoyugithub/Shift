#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_RW(curDensityField, float, 1);
BUFFER_RW(nextDensityField, float, 2);
BUFFER_WO(afterNextDensityField, float, 3);
BUFFER_RO(curVelocityField, vec2, 4);

float _radius = 10.0;

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void addSource(uint index, float dt);

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void diffuse(uint x, uint y, float diff, float dt);

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void advect(uint x, uint y, uint index, float dt);

uint calIndex(uint x, uint y, uint width);

NUM_THREADS(32, 32, 1)
void main() {
    uint index = calIndex(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uBufferWidth);
    // The origin of gl_FragCoord is bottom left, however the origin of glfwGetCursorPos() is top left
    float dis = distance(vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y)), vec2(uMousePosX, uBufferHeight - uMousePosY));

    if (dis < _radius) {
        // add density
        addSource(index, uDeltaTime);

        // diffuse the density
        diffuse(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uDiff, uDeltaTime);

        // advect the density
        advect(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, index, uDeltaTime);
    } else {
        curDensityField[index] = prevDensityField[index];
    }
}

void addSource(uint index, float dt) {
    curDensityField[index] += 0.002 + prevDensityField[index] * dt;
}

void diffuse(uint x, uint y, float diff, float dt) {
    float diffRate = diff * dt * uBufferWidth * uBufferHeight;

    uint curIndex = calIndex(x, y, uBufferWidth);
    uint prevXIndex = calIndex(x-1, y, uBufferWidth);
    uint prevYIndex = calIndex(x, y-1, uBufferWidth);
    uint nextXIndex = calIndex(x+1, y, uBufferWidth);
    uint nextYIndex = calIndex(x, y+1, uBufferWidth);

    int itr = 20;
    for(int i=0; i<itr; i++) {
        nextDensityField[curIndex] = 
            curDensityField[curIndex] +
            diffRate *
            (nextDensityField[prevXIndex] +
             nextDensityField[nextXIndex] +
             nextDensityField[prevYIndex] +
             nextDensityField[nextYIndex]) /
            (1 + 4 * diffRate);
    }
}

void advect(uint x, uint y, uint index, float dt) {
    // previous position when passed time dt
    float xElapsed = float(x) - dt * float(uBufferWidth) * curVelocityField[index].x;
    float yElapsed = float(y) - dt * float(uBufferHeight) * curVelocityField[index].y;

    int i0 = int(xElapsed);
    int j0 = int(yElapsed);
    int i1 = i0 + 1;
    int j1 = j0 + 1;
    
    i0 = clamp(i0, 0, int(uBufferWidth) - 1);
    j0 = clamp(j0, 0, int(uBufferHeight) - 1);
    i1 = clamp(i1, 0, int(uBufferWidth) - 1);
    j1 = clamp(j1, 0, int(uBufferHeight) - 1);

    float s1 = xElapsed - i0;
    float s0 = 1 - s1;
    float t1 = yElapsed - j0;
    float t0 = 1 - t1;

    afterNextDensityField[index] = 
                s0 * (t0 * nextDensityField[calIndex(uint(i0), uint(j0), uBufferWidth)] +
                      t1 * nextDensityField[calIndex(uint(i0), uint(j1), uBufferWidth)]) +
                s1 * (t0 * nextDensityField[calIndex(uint(i1), uint(j0), uBufferWidth)] +
                      t1 * nextDensityField[calIndex(uint(i1), uint(j1), uBufferWidth)]);
}

uint calIndex(uint x, uint y, uint width) {
    return x + width * y;
}
