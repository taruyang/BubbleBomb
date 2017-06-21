#version 400

in vec2 TexCoords;
out vec4 color;

uniform sampler2D texImg;
uniform vec3 fontColor;

void main()
{    
	color = vec4(fontColor, texture(texImg, TexCoords).r);
}  
