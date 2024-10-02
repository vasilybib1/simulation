#version 330 core 
out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec3 u_background;
uniform int u_size;

uniform sampler1D u_colorTexture;
uniform sampler1D u_dataTexture;

void main(){
  vec4 color = vec4(0.0);

  vec2 st = gl_FragCoord.xy / u_resolution;
  float centerDist = distance(st, vec2(u_background.x, u_background.y));
  if(centerDist < u_background.z){
    color = vec4(0.1, 0.1, 0.1, 1.0);
  }

  for(int i = 0; i < u_size; ++i){
    vec2 pos = vec2(texelFetch(u_dataTexture, i, 0).r, texelFetch(u_dataTexture, i, 0).g);
    float radius = texelFetch(u_dataTexture, i, 0).b;
    
    float dist = distance(st, pos);
    
    if(dist < radius){
      float r = texelFetch(u_colorTexture, i ,0).r;
      float g = texelFetch(u_colorTexture, i ,0).g;
      float b = texelFetch(u_colorTexture, i ,0).b;
      color = vec4(r, g, b, 1.0);
    }
  }

  FragColor = color;
}
