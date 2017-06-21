#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
out vec3 gNormal;
in vec3 fragPos[3];
out vec3 FragPos;

uniform mat3 normalMat;

void main()
{
//	vec3 A = tePosition[2] - tePosition[0];
//	vec3 B = tePosition[1] - tePosition[0];
//	gNormal = normalMat * normalize(cross(A, B));

	for(int i = 0; i < 3; i++)
	{
		gNormal = normalMat * normalize(tePosition[i]);
		gl_Position = gl_in[i].gl_Position; 
		FragPos = fragPos[i];
		EmitVertex();
	}

	EndPrimitive();
}

