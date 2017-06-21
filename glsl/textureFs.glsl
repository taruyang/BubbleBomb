#version 400

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D TexImg;

void main()
{
	outColor = texture(TexImg, TexCoord);
}
