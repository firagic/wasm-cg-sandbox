#version 300 es
precision highp float;

uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos;
out vec4 LightSpacePos;

uniform mat4 gVP;
uniform mat4 gLightVP;
float gGridSize = 100.0;
uniform vec3 camera_world_pos;

const vec3 Pos[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0),      // bottom left
    vec3( 1.0, 0.0, -1.0),      // bottom right
    vec3( 1.0, 0.0,  1.0),      // top right
    vec3(-1.0, 0.0,  1.0)       // top left
);

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);


void main()
{
    int Index = Indices[gl_VertexID];
    vec3 vPos3 = Pos[Index] * gGridSize;

    vPos3.x += camera_world_pos.x;
    vPos3.z += camera_world_pos.z;

    vec4 vPos4 = vec4(vPos3, 1.0);

    gl_Position = projection * view * vPos4;
    LightSpacePos = gLightVP * vPos4;

    WorldPos = vPos3;
}

