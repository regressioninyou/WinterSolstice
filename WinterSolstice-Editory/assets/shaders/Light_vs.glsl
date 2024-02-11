#version 450 core
layout(location = 0) in vec3 aVertexPosition;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uViewProjection;

void main(void) {
	gl_Position = uViewProjection * uModelMatrix * vec4(aVertexPosition, 1.0);
}