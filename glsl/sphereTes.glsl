#version 400

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
out vec3 tePosition;
out vec3 fragPos;	// for light calculatoin in the world space

uniform mat4 PV;
uniform mat4 M;

void main()
{
	vec3 p0 = gl_TessCoord.x * tcPosition[0];
	vec3 p1 = gl_TessCoord.y * tcPosition[1];
	vec3 p2 = gl_TessCoord.z * tcPosition[2];

	tePosition = normalize(p0 + p1 + p2);
	fragPos = vec3(M * vec4(tePosition, 1.0f));
	gl_Position = PV * M * vec4(tePosition, 1);
}
