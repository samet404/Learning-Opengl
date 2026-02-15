#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main() {
  vec2 st = gl_FragCoord.xy / u_resolution;
  if (gl_FragCoord.x > u_mouse.x - 10.0 && gl_FragCoord.x < u_mouse.x + 10.0 && (u_resolution[1] - gl_FragCoord.y) > u_mouse.y - 10.0 && (u_resolution[1] - gl_FragCoord.y) < u_mouse.y + 10.0 ) {
    st = vec2(0.0, 0.0);
  }
	gl_FragColor = vec4(st.x,st.y,0.0,1.0);
}

