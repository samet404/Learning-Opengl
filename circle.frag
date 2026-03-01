#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main() {
    // Normalize coordinates to [-1, 1] and aspect
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution) / min(u_resolution.x, u_resolution.y);

    // Circle radius
    float radius = 0.3;

    // Distance from center
    float dist = length(uv);

    // Draw circle using smoothstep for anti-aliasing
    float circle = smoothstep(radius, radius - 0.002, dist);

    float pulse = 0.5 + 0.2 * sin(u_time * 10.0);

    gl_FragColor = vec4(vec3(circle * pulse), 1.0);
}
