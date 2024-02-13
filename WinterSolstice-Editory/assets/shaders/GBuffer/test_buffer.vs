#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexcoord;

out vec2 TexCoords;

uniform mat4 uViewProjection;

void main()
{
    TexCoords = aTexcoord;
	gl_Position =   vec4(aPosition,1.0); 
}