#version 300 es
precision highp float;


uniform mat4 view;
uniform mat4 projection;

const int sectors = 24;  // 24 longitude divisions (time zones)
const int stacks = 18;   // latitude divisions
const float PI = 3.1415926;

out vec3 vNormal;

void main() {
    int vertId = gl_VertexID;

    int quadId = vertId / 6;      // which quad (stack, sector)
    int triId = vertId % 6;       // vertex within two triangles

    int stack = quadId / sectors;
    int sector = quadId % sectors;

    vec2 uv[6];

    float u0 = float(sector) / float(sectors);
    float u1 = float(sector + 1) / float(sectors);
    float v0 = float(stack) / float(stacks);
    float v1 = float(stack + 1) / float(stacks);

    uv[0] = vec2(u0, v0);
    uv[1] = vec2(u1, v0);
    uv[2] = vec2(u1, v1);

    uv[3] = vec2(u0, v0);
    uv[4] = vec2(u1, v1);
    uv[5] = vec2(u0, v1);

    vec2 uvCoord = uv[triId];

    float lat = mix(-0.5 * PI, 0.5 * PI, uvCoord.y); // -π/2 to π/2
    float lon = mix(-PI, PI, uvCoord.x);             // -π to π

    float x = cos(lat) * cos(lon);
    float y = sin(lat);
    float z = cos(lat) * sin(lon);
    vec3 pos = vec3(x, y, z);

    vNormal = normalize(pos);
    gl_Position = projection * view * vec4(pos, 1.0);
}