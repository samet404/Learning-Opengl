#version 120
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;


vec3 rotateY(vec3 v, float t){
    float cost = cos(t); float sint = sin(t);
    return vec3(v.x * cost + v.z * sint, v.y, -v.x * sint + v.z * cost);
}

float smin( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float map(vec3 p){
    float d = (-1.*length(p)+3.)+1.5;    
    
    float m = 1.5; float s = 0.002;    
    d = smin(d, max(abs(p.z)-s, abs(p.x+p.y/2.)-.07 ), m );    
    
    return d;
}

float color( vec3 p){
   return 0.; 
}

void main()
{    
	vec2 uv = gl_FragCoord.xy * 2.0 / u_resolution.xy - 1.0;
  uv.x *= u_resolution.x / u_resolution.y;
  vec3 ray = vec3(uv.x,uv.y,1.0);
  
  // Color    
  vec3 color = vec3(0.0);    
  const int rayCount = 400;
  
  // Raymarching
  float t = 0.;
  for (int r = 1; r <= rayCount; r++)
  {
      // Ray Position
      vec3 p = vec3(0,0,-3.) + ray * t;        
      
      // Rotation 
      p = rotateY(p, u_mouse.x/u_resolution.x * 2.* 3.14);  
      
      // Deformation 
      float mask = length(p);
      p = rotateY(p,mask*sin(u_time)*0.2);        
    
      // distance
      float d = map(p);   
      
      //color
      if(true)
      {                 
        float iter = float(r) / float(rayCount);
        float ao = (1.-iter);
        ao*=ao;
        ao = 1.-ao;
        
        color += vec3(.1,.5,.6) * ao * 6.;            
        color += vec3(.27,.2,.4)*(t/8.);
        color *= 0.4;
        color -= .15;
                    
        break;          
      }
      
      // march along ray
      t +=  d *.05;        
    }
    
    // vignetting effect by Ippokratis
    // https://www.shadertoy.com/view/lsKSWR
    uv = gl_FragCoord.xy / u_resolution.xy;
    uv *=  1.0 - uv.xy; 
    float vig = uv.x*uv.y * 10.0;    
    vig = pow(vig, 0.2);        
    color *= vig;
    
    //color adjustement
    color.y *= .8;
    color.x *= 1.0;
    
	gl_FragColor = vec4(color, 1);
}
