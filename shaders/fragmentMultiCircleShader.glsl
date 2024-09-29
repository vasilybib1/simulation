#version 330 core 
out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_position_array[100];
uniform float u_radius_array[100];

void main(){
  vec4 color = vec4(0.0);

  for(int i = 0; i < 100; ++i){
    vec2 st = gl_FragCoord.xy / u_resolution;
    float dist = distance(st, u_position_array[i]);
    
    if(dist < u_radius_array[i]){
      color = vec4(u_position_array[i].x/2, u_position_array[i].y/2, 0.5, 1.0);
    }
  }

  FragColor = color;
}
