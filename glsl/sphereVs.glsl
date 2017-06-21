#version 400

layout (location = 0) in vec3 position;
out vec3 vPosition;

void main()
{
	vPosition = position;
}
