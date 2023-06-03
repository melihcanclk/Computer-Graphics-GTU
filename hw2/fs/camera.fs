#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// ambient
	vec3 ambient = 0.1 * texture(texture1, TexCoord).rgb;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * texture(texture1, TexCoord).rgb;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
	vec3 specular = spec * texture(texture1, TexCoord).rgb;    
	vec3 result = (ambient + diffuse + specular) * texture(texture2, TexCoord).rgb;
	FragColor = vec4(result, 1.0);
}