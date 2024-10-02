#version 330 core 
out vec4 FragColor;

in vec2 TexCoords;

uniform vec2 u_resolution;
uniform vec2 u_position_array[100];
uniform float u_center_radius;
uniform vec2 u_center_center;
uniform vec3 u_color[100];

uniform sampler1D radiusTexture;

void main(){
  vec4 color = vec4(0.0);

  vec2 st = gl_FragCoord.xy / u_resolution;
  float centerDist = distance(st, u_center_center);
  if(centerDist < u_center_radius){
    color = vec4(0.1, 0.1, 0.1, 1.0);
  }

  for(int i = 0; i < 100; ++i){
    float dist = distance(st, u_position_array[i]);
    //float val = texture(radiusTexture, (100, 0)).r;
    float val = texelFetch(radiusTexture, i, 0).r;
    
    if(dist < val){
      color = vec4(u_color[i].x, u_color[i].y, u_color[i].z, 1.0);
    }
  }

  FragColor = color;
}
