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

const float kEpsilon = 0.001;
const float kFar = 100.0;

struct HitInfo {
    float t;
    vec3 normal;
    vec3 albedo;
};

float hash21(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 34.45);
    return fract(p.x * p.y);
}

vec3 sky_color(vec3 rd)
{
    float t = clamp(rd.y * 0.5 + 0.5, 0.0, 1.0);
    return mix(vec3(0.08, 0.10, 0.14), vec3(0.55, 0.70, 0.95), t);
}

float intersect_sphere(vec3 ro, vec3 rd, vec3 center, float radius)
{
    vec3 oc = ro - center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;
    float h = b * b - c;
    if (h < 0.0) {
        return -1.0;
    }
    h = sqrt(h);
    float t0 = -b - h;
    float t1 = -b + h;
    if (t0 > kEpsilon) {
        return t0;
    }
    if (t1 > kEpsilon) {
        return t1;
    }
    return -1.0;
}

float intersect_ground(vec3 ro, vec3 rd, float plane_y)
{
    if (abs(rd.y) < 1e-5) {
        return -1.0;
    }
    float t = (plane_y - ro.y) / rd.y;
    return (t > kEpsilon) ? t : -1.0;
}

bool scene_hit(vec3 ro, vec3 rd, out HitInfo hit)
{
    hit.t = kFar;
    bool found = false;

    vec3 c0 = vec3(-1.1, -0.1, -0.6);
    vec3 c1 = vec3( 0.2,  0.2,  0.2);
    vec3 c2 = vec3( 1.4, -0.2,  0.4);
    float t_anim = 0.5 + 0.5 * sin(uTime * 0.8);
    c1.y = mix(0.05, 0.65, t_anim);

    float t = intersect_sphere(ro, rd, c0, 0.9);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        vec3 p = ro + rd * t;
        hit.normal = normalize(p - c0);
        hit.albedo = vec3(0.95, 0.35, 0.28);
    }

    t = intersect_sphere(ro, rd, c1, 0.55);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        vec3 p = ro + rd * t;
        hit.normal = normalize(p - c1);
        hit.albedo = vec3(0.35, 0.92, 0.55);
    }

    t = intersect_sphere(ro, rd, c2, 0.8);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        vec3 p = ro + rd * t;
        hit.normal = normalize(p - c2);
        hit.albedo = vec3(0.35, 0.55, 0.98);
    }

    t = intersect_ground(ro, rd, -1.0);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        vec3 p = ro + rd * t;
        float checker = mod(floor(p.x * 0.7) + floor(p.z * 0.7), 2.0);
        vec3 cA = vec3(0.74, 0.70, 0.62);
        vec3 cB = vec3(0.52, 0.50, 0.44);
        hit.normal = vec3(0.0, 1.0, 0.0);
        hit.albedo = mix(cA, cB, checker);
    }

    return found;
}

float shadow_trace(vec3 ro, vec3 rd)
{
    HitInfo sHit;
    bool blocked = scene_hit(ro, rd, sHit);
    if (!blocked) {
        return 1.0;
    }
    return 0.0;
}

vec3 shade(vec3 ro, vec3 rd)
{
    HitInfo hit;
    if (!scene_hit(ro, rd, hit)) {
        return sky_color(rd);
    }

    vec3 p = ro + rd * hit.t;
    vec3 n = hit.normal;
    vec3 l = normalize(vec3(0.55, 1.0, 0.35));
    vec3 v = normalize(-rd);
    vec3 h = normalize(l + v);

    float ndotl = max(dot(n, l), 0.0);
    float shadow = shadow_trace(p + n * 0.01, l);
    float diffuse = ndotl * shadow;
    float spec = pow(max(dot(n, h), 0.0), 48.0) * shadow;

    vec3 ambient = hit.albedo * 0.16;

    vec3 bounce_dir = normalize(n + vec3(0.25, 0.85, 0.35));
    float bounce_mix = 0.35 + 0.25 * hash21(gl_FragCoord.xy + uTime);
    vec3 bounce = mix(vec3(0.11, 0.10, 0.09), sky_color(bounce_dir), bounce_mix);

    vec3 color = ambient;
    color += hit.albedo * diffuse;
    color += vec3(1.0) * spec * 0.35;
    color += hit.albedo * bounce * 0.35;

    return color;
}

void main()
{
    vec2 frag = gl_FragCoord.xy / max(uResolution, vec2(1.0));
    vec2 p = frag * 2.0 - 1.0;
    p.x *= uResolution.x / max(uResolution.y, 1.0);

    vec3 ro = uCamPos;
    vec3 forward = normalize(uCamFront);
    vec3 upRef = normalize(uCamUp);
    vec3 right = normalize(cross(forward, upRef));
    vec3 up = cross(right, forward);

    float fovScale = tan(radians(clamp(uFov, 1.0, 89.0)) * 0.5);
    vec3 rd = normalize(forward + p.x * right * fovScale + p.y * up * fovScale);

    vec3 color = shade(ro, rd);
    color = pow(color, vec3(0.4545));
    FragColor = vec4(color, 1.0);
}
