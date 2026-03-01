#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

float smoothstep2(float e0,  float e1, float x) {
    float t;
    t = clamp((x - e0) / (e1 - e0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

// Plot a line on Y using a value between 0.0-1.0
float plot(vec2 st) {    
    return smoothstep(smoothstep2(0.1, 0.9, st.x) - 0.02 , smoothstep2(0.1, 0.9, st.x), st.y);
}

void main() {
	vec2 st = gl_FragCoord.xy/u_resolution;

  float y = st.x;

  vec3 color = vec3(y);

  // Plot a line
  float pct = plot(st);
  color = (1.0-pct)*color+pct*vec3(0.0,1.0,0.0);

	gl_FragColor = vec4(color,1.0);
}
