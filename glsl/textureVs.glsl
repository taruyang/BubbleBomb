#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 PVM;

void main()
{
	gl_Position = PVM * vec4(position, 1.0f);
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
