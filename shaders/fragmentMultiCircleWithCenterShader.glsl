#version 330 core 
out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_position_array[10];
uniform float u_radius_array[10];
uniform float u_center_radius;
uniform vec2 u_center_center;

void main(){
  vec4 color = vec4(0.0);

  vec2 st = gl_FragCoord.xy / u_resolution;
  float centerDist = distance(st, u_center_center);
  if(centerDist < u_center_radius){
    color = vec4(0.1, 0.1, 0.1, 1.0);
  }


  for(int i = 0; i < 10; ++i){
    float dist = distance(st, u_position_array[i]);
    
    if(dist < u_radius_array[i]){
      color = vec4(1.0, 1.0, 1.0, 1.0);
    }
  }

  FragColor = color;
}
