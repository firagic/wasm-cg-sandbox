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

float intersect_box(vec3 ro, vec3 rd, vec3 bmin, vec3 bmax, out vec3 normal)
{
    vec3 inv_rd = 1.0 / rd;
    vec3 t0 = (bmin - ro) * inv_rd;
    vec3 t1 = (bmax - ro) * inv_rd;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

    float t_near = max(max(tmin.x, tmin.y), tmin.z);
    float t_far = min(min(tmax.x, tmax.y), tmax.z);

    if (t_far < 0.0 || t_near > t_far) {
        normal = vec3(0.0);
        return -1.0;
    }

    float t_hit = (t_near > kEpsilon) ? t_near : t_far;
    if (t_hit <= kEpsilon) {
        normal = vec3(0.0);
        return -1.0;
    }

    vec3 p = ro + rd * t_hit;
    vec3 c = 0.5 * (bmin + bmax);
    vec3 h = 0.5 * (bmax - bmin);
    vec3 q = abs(p - c) / max(h, vec3(1e-4));

    if (q.x > q.y && q.x > q.z) {
        normal = vec3(sign(p.x - c.x), 0.0, 0.0);
    } else if (q.y > q.z) {
        normal = vec3(0.0, sign(p.y - c.y), 0.0);
    } else {
        normal = vec3(0.0, 0.0, sign(p.z - c.z));
    }

    return t_hit;
}

float intersect_cylinder_y(vec3 ro, vec3 rd, vec3 center, float radius, float half_height, out vec3 normal)
{
    vec3 p = ro - center;
    float best_t = kFar;
    normal = vec3(0.0);

    float a = rd.x * rd.x + rd.z * rd.z;
    float b = 2.0 * (p.x * rd.x + p.z * rd.z);
    float c = p.x * p.x + p.z * p.z - radius * radius;

    if (a > 1e-6) {
        float h = b * b - 4.0 * a * c;
        if (h >= 0.0) {
            float s = sqrt(h);
            float inv_2a = 0.5 / a;
            float t0 = (-b - s) * inv_2a;
            float t1 = (-b + s) * inv_2a;

            float y0 = p.y + rd.y * t0;
            if (t0 > kEpsilon && abs(y0) <= half_height) {
                best_t = t0;
                vec3 h0 = p + rd * t0;
                normal = normalize(vec3(h0.x, 0.0, h0.z));
            }

            float y1 = p.y + rd.y * t1;
            if (t1 > kEpsilon && t1 < best_t && abs(y1) <= half_height) {
                best_t = t1;
                vec3 h1 = p + rd * t1;
                normal = normalize(vec3(h1.x, 0.0, h1.z));
            }
        }
    }

    if (abs(rd.y) > 1e-6) {
        float t_top = (half_height - p.y) / rd.y;
        if (t_top > kEpsilon && t_top < best_t) {
            vec3 p_top = p + rd * t_top;
            if (dot(p_top.xz, p_top.xz) <= radius * radius) {
                best_t = t_top;
                normal = vec3(0.0, 1.0, 0.0);
            }
        }

        float t_bottom = (-half_height - p.y) / rd.y;
        if (t_bottom > kEpsilon && t_bottom < best_t) {
            vec3 p_bottom = p + rd * t_bottom;
            if (dot(p_bottom.xz, p_bottom.xz) <= radius * radius) {
                best_t = t_bottom;
                normal = vec3(0.0, -1.0, 0.0);
            }
        }
    }

    if (best_t >= kFar) {
        normal = vec3(0.0);
        return -1.0;
    }

    return best_t;
}

float sd_torus(vec3 p, vec2 radii)
{
    vec2 q = vec2(length(p.xz) - radii.x, p.y);
    return length(q) - radii.y;
}

vec3 torus_normal(vec3 p, vec2 radii)
{
    const float e = 0.001;
    float dx = sd_torus(p + vec3(e, 0.0, 0.0), radii) - sd_torus(p - vec3(e, 0.0, 0.0), radii);
    float dy = sd_torus(p + vec3(0.0, e, 0.0), radii) - sd_torus(p - vec3(0.0, e, 0.0), radii);
    float dz = sd_torus(p + vec3(0.0, 0.0, e), radii) - sd_torus(p - vec3(0.0, 0.0, e), radii);
    return normalize(vec3(dx, dy, dz));
}

float intersect_torus(vec3 ro, vec3 rd, vec3 center, vec2 radii, out vec3 normal)
{
    float bound_t = intersect_sphere(ro, rd, center, radii.x + radii.y + 0.1);
    if (bound_t < 0.0) {
        normal = vec3(0.0);
        return -1.0;
    }

    float t = max(bound_t - 2.0 * radii.y, 0.0);
    for (int i = 0; i < 72; ++i) {
        vec3 local_p = ro + rd * t - center;
        float d = sd_torus(local_p, radii);
        if (d < 0.0012) {
            normal = torus_normal(local_p, radii);
            return t;
        }
        t += max(d * 0.75, 0.002);
        if (t > kFar) {
            break;
        }
    }

    normal = vec3(0.0);
    return -1.0;
}

vec3 get_probe_center()
{
    vec3 c0 = vec3(-1.1, -0.1, -0.6);
    vec3 c1 = vec3(0.2, 0.2, 0.2);
    vec3 c2 = vec3(1.4, -0.2, 0.4);
    float t_anim = 0.5 + 0.5 * sin(uTime * 0.8);
    c1.y = mix(0.05, 0.65, t_anim);

    vec3 box_center = vec3(-2.6, -0.2, 0.8 + 0.25 * sin(uTime * 0.6));
    vec3 cyl_center = vec3(2.35, -0.05, -1.15);
    vec3 torus_center = vec3(-0.15, -0.02 + 0.12 * sin(uTime * 0.9), -2.2);

    vec3 path0 = c0;
    vec3 path1 = c1;
    vec3 path2 = c2;
    vec3 path3 = box_center + vec3(0.0, 0.20, 0.0);
    vec3 path4 = cyl_center + vec3(0.0, 0.10, 0.0);
    vec3 path5 = torus_center;

    float segment = fract(uTime * 0.08) * 6.0;
    float u = fract(segment);
    u = u * u * (3.0 - 2.0 * u);

    vec3 from = path0;
    vec3 to = path1;
    if (segment < 1.0) {
        from = path0; to = path1;
    } else if (segment < 2.0) {
        from = path1; to = path2;
    } else if (segment < 3.0) {
        from = path2; to = path3;
    } else if (segment < 4.0) {
        from = path3; to = path4;
    } else if (segment < 5.0) {
        from = path4; to = path5;
    } else {
        from = path5; to = path0;
    }

    return mix(from, to, u);
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
    vec3 box_center = vec3(-2.6, -0.2, 0.8 + 0.25 * sin(uTime * 0.6));
    vec3 box_half = vec3(0.55, 0.8, 0.55);
    vec3 cyl_center = vec3(2.35, -0.05, -1.15);
    float cyl_half_height = 0.85 + 0.12 * sin(uTime * 1.1);
    vec3 torus_center = vec3(-0.15, -0.02 + 0.12 * sin(uTime * 0.9), -2.2);
    vec2 torus_radii = vec2(0.82, 0.24);
    vec3 probe_center = get_probe_center();

    vec3 n_tmp;

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

    t = intersect_box(ro, rd, box_center - box_half, box_center + box_half, n_tmp);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        hit.normal = n_tmp;
        hit.albedo = vec3(0.92, 0.76, 0.32);
    }

    t = intersect_cylinder_y(ro, rd, cyl_center, 0.46, cyl_half_height, n_tmp);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        hit.normal = n_tmp;
        hit.albedo = vec3(0.38, 0.84, 0.93);
    }

    t = intersect_torus(ro, rd, torus_center, torus_radii, n_tmp);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        hit.normal = n_tmp;
        hit.albedo = vec3(0.96, 0.62, 0.30);
    }

    t = intersect_sphere(ro, rd, probe_center, 0.40);
    if (t > 0.0 && t < hit.t) {
        found = true;
        hit.t = t;
        vec3 p = ro + rd * t;
        hit.normal = normalize(p - probe_center);
        hit.albedo = vec3(0.95, 0.95, 0.62);
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
