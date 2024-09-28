#version 330 core 
out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_center;
uniform float u_radius;

void main(){
  vec2 st = gl_FragCoord.xy / u_resolution;
  float dist = distance(st, u_center);
  
  if(dist < u_radius){
    FragColor = vec4(0.5, 0.5, 0.5, 1.0);
  }else{
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
