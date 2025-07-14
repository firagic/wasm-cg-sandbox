#version 300 es
precision highp float;

in vec3 WorldPos;
in vec4 LightSpacePos;

layout(location = 0) out vec4 FragColor;

uniform vec3 camera_world_pos;
float gGridSize = 100.0;
float gGridMinPixelsBetweenCells = 2.0;
float gGridCellSize = 0.025;
vec4 gGridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
vec4 gGridColorThick = vec4(0.0, 0.0, 0.0, 1.0);
vec3 gLightDirection;
uniform sampler2D gShadowMap;
bool gShadowsEnabled = false;


float compute_lod_val(vec2 dudv, float cell_size_lod)
{
    vec2 repeated_pattern = mod(WorldPos.xz, cell_size_lod);
    vec2 normalized_repeated_pattern = repeated_pattern / dudv;
    vec2 clamped_normalized_repeated_pattern = clamp(normalized_repeated_pattern, vec2(0.0), vec2(1.0));
    vec2 mapped_tile_data_range = clamped_normalized_repeated_pattern * 2.0 - vec2(1.0);
    vec2 v_shaped_gradient = abs(mapped_tile_data_range);
    vec2 inverted_v_shaped_gradient = vec2(1.0) - v_shaped_gradient;
    return max(inverted_v_shaped_gradient.x, inverted_v_shaped_gradient.y);
    // vec2 v = vec2(1.0) - abs(clamp(mod(WorldPos.xz, cell_size_lod) / dudv, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    // return max(v.x, v.y);
}


float compute_shadow_factor(vec3 light_direction)
{
    if(!gShadowsEnabled)
        return 1.0;

    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;

    vec3 ShadowCoords = ProjCoords * 0.5 + vec3(0.5);

    vec3 Normal = vec3(0.0, 1.0, 0.0);

    float Depth = texture(gShadowMap, ShadowCoords.xy).x;

    float DiffuseFactor = dot(Normal, -light_direction);

    float bias = 0.0;//max(0.05 * (1.0 - DiffuseFactor), 0.005);  

    if (Depth + bias < ShadowCoords.z)
        return 0.15;
    else
        return 1.0;
}

void main()
{
    vec4 Color;
    
    vec2 dvx = vec2(dFdx(WorldPos.x), dFdy(WorldPos.x));
    vec2 dvy = vec2(dFdx(WorldPos.z), dFdy(WorldPos.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);

    float l = length(dudv);

    float LOD = max(0.0, log(l * gGridMinPixelsBetweenCells / gGridCellSize) / log(10.0) + 1.0);

    dudv *= 4.0;
    
    float GridCellSizeLod = gGridCellSize * pow(10.0, floor(LOD));
    float Lod2a = compute_lod_val(dudv, GridCellSizeLod * 100.0);
    if (Lod2a > 0.0) {
        Color = gGridColorThick;
        Color.a *= Lod2a;
    } else {
        float Lod1a = compute_lod_val(dudv, GridCellSizeLod * 10.0);
        float LOD_fade = fract(LOD);
        if (Lod1a > 0.0) {
            Color = mix(gGridColorThick, gGridColorThin, LOD_fade);
	        Color.a *= Lod1a;
        } else {
            float Lod0a = compute_lod_val(dudv, GridCellSizeLod);
            Color = gGridColorThin;
	        Color.a *= (Lod0a * (1.0 - LOD_fade));
        }
    }
    
    float OpacityFalloff = (1.0 - clamp((length(WorldPos.xz - camera_world_pos.xz) / gGridSize), 0.0, 1.0));

    Color.a *= OpacityFalloff;
    if (Color.a < 0.01) discard;

    float ShadowFactor = compute_shadow_factor(gLightDirection);

    if (ShadowFactor < 1.0) {
        FragColor = vec4(Color.xyz * ShadowFactor, 1.0 - ShadowFactor);
    } else {
        FragColor = Color;
    }

}