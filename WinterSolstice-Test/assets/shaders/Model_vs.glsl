
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 0) in vec3 aNormal;
layout(location = 0) in vec2 aTexcoord;
uniform mat4 uViewProjection;

uniform mat4 uModel;
out vec2 vTexcoord;
void main (){
	vTexcoord = aTexcoord;
	gl_Position = uModel * uViewProjection * vec4( aPosition ,1.0f); 
}