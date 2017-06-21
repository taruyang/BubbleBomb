#version 400

in vec2 TexCoord;
out vec4 color;

uniform vec4 objColor;
uniform sampler2D TexImg;

void main()
{
	vec4 texColor = texture(TexImg, TexCoord);
	if(texColor.a == 0.f)
		color = objColor;
	else
		color = texColor;
}
