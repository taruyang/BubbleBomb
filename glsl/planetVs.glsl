#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
flat out int InstanceID;

uniform mat4 PV;
uniform mat4 M;
uniform mat3 normalMat;

void main()
{
	gl_Position = PV * M * instanceMatrix * vec4(position, 1.0f);
	FragPos = vec3(M * vec4(position, 1.0f));
	TexCoords = texCoords;
	Normal = normalMat * normal;
	InstanceID = gl_InstanceID;
}
