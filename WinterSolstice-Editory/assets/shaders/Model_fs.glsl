
#version 330 core
layout(location = 0) out vec4 color;
uniform sampler2D texture_diffuse;
in vec2 vTexcoord;
void main(){
color = vec4(0.0,0.0,1.0,1.0);
}