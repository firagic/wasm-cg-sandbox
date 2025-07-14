#version 300 es
precision highp float;


uniform mat4 view;
uniform mat4 projection;

const int SECTORS = 24;
const int STACKS = 18;
const float PI = 3.1415926;
const float HALF_PI = 1.5707963;
out vec2 vUV;

void main() {
    int vertexId = gl_VertexID;
    int quadId = vertexId / 6;
    int triId = vertexId % 6;

    int stack = quadId / SECTORS;
    int sector = quadId % SECTORS;

    float u0 = float(sector) / float(SECTORS);
    float u1 = float(sector + 1) / float(SECTORS);
    float v0 = float(stack) / float(STACKS);
    float v1 = float(stack + 1) / float(STACKS);

    vec2 uv[6];
    uv[0] = vec2(u0, v0);
    uv[1] = vec2(u1, v0);
    uv[2] = vec2(u1, v1);
    uv[3] = vec2(u0, v0);
    uv[4] = vec2(u1, v1);
    uv[5] = vec2(u0, v1);

    vUV = uv[triId];

    float lat = mix(-HALF_PI, HALF_PI, vUV.y);
    float lon = mix(-PI, PI, vUV.x);

    vec3 pos = vec3(
        cos(lat) * cos(lon),
        sin(lat),
        cos(lat) * sin(lon)
    );

    gl_Position = projection * view * vec4(pos, 1.0);
}