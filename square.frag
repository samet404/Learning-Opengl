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
  vec2 pixelCoord = gl_FragCoord.xy / u_resolution;
  float aspect = u_resolution.x / u_resolution.y;

  float space = 0.2;

  float widthOfSquare = (1.0 - 2.0 * space) / aspect;
  float squareSpaceX = (1.0 - widthOfSquare) / 2.0;

  
  float bottomLeftX = step(squareSpaceX, pixelCoord.x);
  float bottomLeftY = step(space, pixelCoord.y);

  float topRightX = step(squareSpaceX, 1.0 - pixelCoord.x);
  float topRightY = step(space, 1.0 - pixelCoord.y);

  float inSquare = (bottomLeftX * bottomLeftY) * (topRightX * topRightY);

  gl_FragColor = vec4(vec3(inSquare), 1.0);
}
