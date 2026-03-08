#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

float distance_from_sphere(in vec2 p, in vec2 c, float r)
{
	return length(p - c) - r;
}

const float CIRCLE_STROKE = 0.004;

float drawCircle(vec2 uv, float x, float y, float radius, float stroke) {
  float status = 0.0;

  float dist = length(uv.xy - vec2(x, y));
  
  if (
    dist < radius + stroke &&
    dist > radius - stroke
  ) status = 1.0;

  return status;
}

void main() {
  vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution) / min(u_resolution.x, u_resolution.y);
  vec2 muv = (u_mouse * 2.0 - u_resolution) / min(u_resolution.x, u_resolution.y);
  muv.y = 0.0 - muv.y;


  float mCircRadius = abs(distance_from_sphere(muv, vec2(0.0, 0.0), 0.5));


  float mouseCircle = drawCircle(uv, muv.x,  muv.y, mCircRadius, CIRCLE_STROKE);
  float mainCircle = drawCircle(uv, 0.0, 0.0, 0.5, CIRCLE_STROKE);
  float pulse = 0.5 + 0.2 * sin(u_time * 10.0);

  gl_FragColor = vec4(vec3((mouseCircle + mainCircle) * pulse), 1.0);
}
