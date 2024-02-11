
#version 330 core
layout(location = 0) in vec3 aPosition;
uniform mat4 uViewProjection;
void main (){
	gl_Position =  uViewProjection * vec4( aPosition + 0.5 ,1.0f); 
}