#version  330  core										
out vec4 FragColor;	

uniform bool directionalLightOn = true;							
void main()												
{		
	vec3 result = vec3(0.0);				
	if(directionalLightOn)
		result = vec3(1.0);

	FragColor = vec4(result, 1.0);				
}