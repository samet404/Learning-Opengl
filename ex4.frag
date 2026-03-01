#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

mat2 rotate(float angle) {
  return mat2(
    cos(angle), -sin(angle),
    sin(angle), cos(angle)
  );
};

void main() {
  vec2 pixelCoord = gl_FragCoord.xy/u_resolution;


  pixelCoord = pixelCoord - vec2(0.5);
  pixelCoord = rotate(u_time * 3.14) * pixelCoord;
  pixelCoord = pixelCoord + vec2(0.5);

  float borderWidth = 0.3;

  vec2 bottomLeft = step(vec2(borderWidth), pixelCoord);
  vec2 topRight = step(vec2(borderWidth), 1.0 - pixelCoord);

  float vec2ToFloat = (bottomLeft.x * bottomLeft.y) * (topRight.x * topRight.y);

  gl_FragColor = vec4(vec3(vec2ToFloat), 1.0);
}

