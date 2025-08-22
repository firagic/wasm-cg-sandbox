#version 300 es
precision highp float;

in vec2 vUV;

out vec4 FragColor;

uniform float sectors;
uniform float stacks;

float edgeFactor() {
    vec2 d = fwidth(vUV); // pixel-space derivative
    vec2 grid = fract(vUV * vec2(sectors, stacks));

    // Thicker line by using larger multiplier
    vec2 edgeWidth = d * 15.0;

    float edgeU = min(
        smoothstep(0.0, edgeWidth.x, grid.x),
        smoothstep(0.0, edgeWidth.x, 1.0 - grid.x)
    );
    float edgeV = min(
        smoothstep(0.0, edgeWidth.y, grid.y),
        smoothstep(0.0, edgeWidth.y, 1.0 - grid.y)
    );

    return min(edgeU, edgeV); // 0 near line, ~1 in middle of cell
}

void main() {
    float edge = edgeFactor();
    float alpha = smoothstep(0.0, 0.2, 1.0 - edge);  // sharper transition

    if (alpha < 0.05)
        discard;  // Completely discard non-line pixels

    FragColor = vec4(vec3(0.0), 1.0);  // Opaque black lines
}