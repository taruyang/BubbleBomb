#version 400

// input
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

// output
out vec4 color;

// viewer
uniform vec3 viewPos;

// light
uniform vec3 lightPos;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

// material
uniform float 	  textureShininess;
uniform sampler2D textureDiffuse1;
uniform sampler2D textureSpecular1;

// point light
uniform float 	pointLightConstance = 1.0f;
uniform float 	pointLightLinear = 0.09f;
uniform float 	pointLightQuadratic = 0.032f;

// multiple point lights
#define NUM_POINT_LIGHTS 6

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), textureShininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (pointLightConstance + pointLightLinear * distance + pointLightQuadratic * (distance * distance));    

    vec3 ambient = lightAmbient * vec3(texture(textureDiffuse1, TexCoords));
    vec3 diffuse = lightDiffuse * diff * vec3(texture(textureDiffuse1, TexCoords));
    vec3 specular = lightSpecular * spec * vec3(texture(textureSpecular1, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
    // Ambient
    vec3 ambient = lightAmbient * vec3(texture(textureDiffuse1, TexCoords));
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * vec3(texture(textureDiffuse1, TexCoords));  
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), textureShininess);
    vec3 specular = lightSpecular * spec * vec3(texture(textureSpecular1, TexCoords));

    vec3 lightColor = ambient + diffuse + specular;  

    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
        lightColor += calcPointLight(pointLights[i], norm, FragPos, viewDir);    
        
    color = vec4(lightColor, 1.0f); 
}
