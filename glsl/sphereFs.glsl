#version 400

out vec4 outColor;

in vec3 gNormal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

// multiple point lights
#define NUM_POINT_LIGHTS 6

uniform float 	pointLightConstance = 1.0f;
uniform float 	pointLightLinear = 0.09f;
uniform float 	pointLightQuadratic = 0.032f;

struct PointLight {
    vec3 position;
};

uniform PointLight pointLights[NUM_POINT_LIGHTS];

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (pointLightConstance + pointLightLinear * distance + pointLightQuadratic * (distance * distance));    

    vec3 ambient = 0.05f * lightColor * objectColor;
    vec3 diffuse = 0.8f * lightColor * diff * objectColor;
    vec3 specular = 1.0f * lightColor * spec * objectColor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
	// Ambient
	float ambientStrength = 0.05f;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse
	vec3 norm = normalize(gNormal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = 0.4f * diff * lightColor;

	// Specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  

	vec3 result = (ambient + diffuse + specular) * objectColor;

	for(int i = 0; i < NUM_POINT_LIGHTS; i++)
		result += calcPointLight(pointLights[i], norm, FragPos, viewDir);    

	outColor = vec4(result, 1.0f);
}

