#version 120
#extension GL_EXT_gpu_shader4 : enable
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

#define PI 3.14159

// SDF function taken from https://www.shadertoy.com/view/4fs3zf
float star(vec2 p, float radius, float inset, float n){
    float teta = 2.0 * PI / n;
    mat2x2 rot1 = mat2x2(cos(teta), sin(teta), -sin(teta), cos(teta));
    vec2 p1 = vec2(0.0, radius);
    vec2 p2 = vec2(sin(teta*0.5), cos(teta*0.5))*radius*inset;
    
    float tetaP = PI + atan(-p.x, -p.y);
    tetaP = mod(tetaP + PI / n, 2.0 * PI);
    
    for(float i = teta; i < tetaP; i+= teta)
         p = rot1 *p;
    
    p.x = abs(p.x);
    
    // sdf segment
    vec2 ba = p2-p1;
    vec2 pa = p - p1;
    float h =clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    float d = length(pa-h*ba);
    d *= sign(dot(p - p1, -vec2(ba.y, -ba.x)));
    return d;

}

float starPattern(vec2 p, int starCount, float speed)
{
    float colSum = 0.;
    float outerShape = 1. - smoothstep(-.01, -.001, star(p, 1., .6, 5.));
    for (int i = 0; i < starCount * 2; i += 1)
    {
        float delta = float(i) - (2. * fract(u_time * speed) - 1.);
        float radius = 1. - delta / float(starCount * 2);
        float star = 1. - smoothstep(-.01, -.001, star(p, radius, .6, 5.));
        
        colSum = i % 2 == 0 ? colSum + star: colSum - star;
    }
    float innerStarDelta = 2. * fract(u_time * speed) - 1.;
    float innerStarRadius = innerStarDelta / float(starCount * 2);
    if (innerStarRadius > 0.)
    {
        float innerStar = 1. - smoothstep(-.01, -.001, star(p, innerStarRadius, .6, 5.));
        colSum += innerStar;
    }
    
    colSum *= outerShape;
    
    return colSum;
}


void main()
{
    // Normalized pixel coordinates (from -1 to -0.5)
    vec2 uv = (gl_FragCoord.xy/u_resolution*2.0) - 1.0;
    // Normalized pixel coordinates (from 0.0 to 1.0)
    uv.x *= u_resolution.x/u_resolution.y;
    uv.y += .1;
    
    float starCol = starPattern(uv, 4, 2f);
    gl_FragColor = vec4(vec3(starCol) ,1.);
}
