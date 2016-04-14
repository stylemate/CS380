#version 330 core

// Ouput color
out vec3 color;
in vec3 fragmentColor;

void main(){
	// Output color = color specified in the vertex shader
	color = fragmentColor;
}