#version 300 es
precision highp float;

in vec2 vUv;
out vec4 FragColor;

uniform vec2 uResolution;
uniform float uTime;
uniform vec3 uCamPos;
uniform vec3 uCamFront;
uniform vec3 uCamUp;
uniform float uFov;

const float FAR_CLIP = 100.0;
const float HIT_EPS = 0.001;
const int MAX_STEPS = 176;
const float MIN_STEP = 0.002;

float hash21(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 34.45);
    return fract(p.x * p.y);
}

float sd_sphere(vec3 p, float r)
{
    return length(p) - r;
}

float sd_box(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sd_torus(vec3 p, vec2 radii)
{
    vec2 q = vec2(length(p.xz) - radii.x, p.y);
    return length(q) - radii.y;
}

float sd_capped_cylinder_y(vec3 p, float radius, float half_height)
{
    vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(radius, half_height);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

vec2 op_union(vec2 a, vec2 b)
{
    return (a.x < b.x) ? a : b;
}

vec2 map_objects(vec3 p)
{
    vec2 hit = vec2(1e9, -1.0);

    vec3 c0 = vec3(-1.1, -0.1, -0.6);
    vec3 c1 = vec3(0.2, 0.2, 0.2);
    vec3 c2 = vec3(1.4, -0.2, 0.4);
    float t_anim = 0.5 + 0.5 * sin(uTime * 0.8);
    c1.y = mix(0.05, 0.65, t_anim);

    vec3 box_center = vec3(-2.6, -0.2, 0.8 + 0.25 * sin(uTime * 0.6));
    vec3 box_half = vec3(0.55, 0.8, 0.55);

    vec3 cyl_center = vec3(2.35, -0.05, -1.15);
    float cyl_half_height = 0.85 + 0.12 * sin(uTime * 1.1);

    vec3 torus_center = vec3(-0.15, -0.02 + 0.12 * sin(uTime * 0.9), -2.2);
    vec2 torus_radii = vec2(0.82, 0.24);

    hit = op_union(hit, vec2(sd_sphere(p - c0, 0.9), 1.0));
    hit = op_union(hit, vec2(sd_sphere(p - c1, 0.55), 2.0));
    hit = op_union(hit, vec2(sd_sphere(p - c2, 0.8), 3.0));
    hit = op_union(hit, vec2(sd_box(p - box_center, box_half), 4.0));
    hit = op_union(hit, vec2(sd_capped_cylinder_y(p - cyl_center, 0.46, cyl_half_height), 5.0));
    hit = op_union(hit, vec2(sd_torus(p - torus_center, torus_radii), 6.0));

    return hit;
}

float hit_epsilon(float t)
{
    return clamp(HIT_EPS * (1.0 + 0.05 * t), HIT_EPS, 0.02);
}

bool raymarch_objects(vec3 ro, vec3 rd, out float t, out float mat)
{
    t = 0.0;
    mat = -1.0;

    for (int i = 0; i < MAX_STEPS; ++i) {
        vec2 h = map_objects(ro + rd * t);
        float eps = hit_epsilon(t);
        if (h.x < eps) {
            mat = h.y;
            return true;
        }

        t += max(h.x, MIN_STEP);
        if (t > FAR_CLIP) {
            break;
        }
    }

    return false;
}

vec3 calc_normal_objects(vec3 p)
{
    float e = 0.0015;
    vec3 ex = vec3(e, 0.0, 0.0);
    vec3 ey = vec3(0.0, e, 0.0);
    vec3 ez = vec3(0.0, 0.0, e);

    float dx = map_objects(p + ex).x - map_objects(p - ex).x;
    float dy = map_objects(p + ey).x - map_objects(p - ey).x;
    float dz = map_objects(p + ez).x - map_objects(p - ez).x;
    return normalize(vec3(dx, dy, dz));
}

float hard_shadow(vec3 ro, vec3 rd)
{
    float t = 0.03;

    for (int i = 0; i < 96; ++i) {
        float d = map_objects(ro + rd * t).x;
        if (d < 0.0009) {
            return 0.0;
        }
        t += max(d, 0.01);
        if (t > 24.0) {
            break;
        }
    }

    return 1.0;
}

vec3 sky_color(vec3 rd)
{
    float t = clamp(rd.y * 0.5 + 0.5, 0.0, 1.0);
    return mix(vec3(0.08, 0.10, 0.14), vec3(0.55, 0.70, 0.95), t);
}

vec3 material_color(float mat, vec3 p)
{
    if (mat < 0.5) {
        float checker = mod(floor(p.x * 0.7) + floor(p.z * 0.7), 2.0);
        vec3 cA = vec3(0.74, 0.70, 0.62);
        vec3 cB = vec3(0.52, 0.50, 0.44);
        return mix(cA, cB, checker);
    }
    if (mat < 1.5) {
        return vec3(0.95, 0.35, 0.28);
    }
    if (mat < 2.5) {
        return vec3(0.35, 0.92, 0.55);
    }
    if (mat < 3.5) {
        return vec3(0.35, 0.55, 0.98);
    }
    if (mat < 4.5) {
        return vec3(0.92, 0.76, 0.32);
    }
    if (mat < 5.5) {
        return vec3(0.38, 0.84, 0.93);
    }
    return vec3(0.96, 0.62, 0.30);
}

vec3 shade(vec3 ro, vec3 rd)
{
    float t_plane = 1e9;
    if (abs(rd.y) > 1e-5) {
        float tp = (-1.0 - ro.y) / rd.y;
        if (tp > HIT_EPS) {
            t_plane = tp;
        }
    }

    float t_obj;
    float mat_obj;
    bool hit_obj = raymarch_objects(ro, rd, t_obj, mat_obj);
    bool hit_plane = (t_plane < FAR_CLIP);

    if (!hit_obj && !hit_plane) {
        return sky_color(rd);
    }

    bool use_plane = hit_plane && (!hit_obj || t_plane < t_obj);

    float t = use_plane ? t_plane : t_obj;
    float mat = use_plane ? 0.0 : mat_obj;

    vec3 p = ro + rd * t;
    vec3 n = use_plane ? vec3(0.0, 1.0, 0.0) : calc_normal_objects(p);
    vec3 l = normalize(vec3(0.55, 1.0, 0.35));
    vec3 v = normalize(-rd);
    vec3 h = normalize(l + v);

    float ndotl = max(dot(n, l), 0.0);
    float shadow = hard_shadow(p + n * 0.01, l);
    float diffuse = ndotl * shadow;
    float spec = pow(max(dot(n, h), 0.0), 48.0) * shadow;

    vec3 albedo = material_color(mat, p);
    vec3 ambient = albedo * 0.16;

    vec3 bounce_dir = normalize(n + vec3(0.25, 0.85, 0.35));
    float bounce_mix = 0.35 + 0.25 * hash21(gl_FragCoord.xy + uTime);
    vec3 bounce = mix(vec3(0.11, 0.10, 0.09), sky_color(bounce_dir), bounce_mix);

    vec3 color = ambient;
    color += albedo * diffuse;
    color += vec3(1.0) * spec * 0.35;
    color += albedo * bounce * 0.35;

    return color;
}

void main()
{
    vec2 frag = gl_FragCoord.xy / max(uResolution, vec2(1.0));
    vec2 p = frag * 2.0 - 1.0;
    p.x *= uResolution.x / max(uResolution.y, 1.0);

    vec3 ro = uCamPos;
    vec3 forward = normalize(uCamFront);
    vec3 up_ref = normalize(uCamUp);
    vec3 right = normalize(cross(forward, up_ref));
    vec3 up = cross(right, forward);

    float fov_scale = tan(radians(clamp(uFov, 1.0, 89.0)) * 0.5);
    vec3 rd = normalize(forward + p.x * right * fov_scale + p.y * up * fov_scale);

    vec3 color = shade(ro, rd);
    color = pow(color, vec3(0.4545));
    FragColor = vec4(color, 1.0);
}
