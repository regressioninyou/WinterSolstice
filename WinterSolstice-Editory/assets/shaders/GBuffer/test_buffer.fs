#version 450 core
layout (location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D Texture;

void main()
{    
	color = vec4(1.0,texture(Texture,TexCoords).gb,1.0);
}