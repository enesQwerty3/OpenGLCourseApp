#version 330 core
out vec4 FragColor;
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

//textures
uniform sampler2D diffuseT;
uniform sampler2D specularT;  

//player view position
uniform vec3 viewPos;

struct DirectionalLight {
    vec3 position;
	vec3 direction;
	float shininess;	//specular light shininess

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {  
	vec3 position;
	vec3 direction;
	//attenuation values
	float kConstant;
	float kLinear;
	float kQuadratic;
}; 

struct SoftSpotlight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float cutOffOuter;
};


uniform PointLight pointLight;
uniform DirectionalLight directionalLight;
uniform SoftSpotlight softSpotlight;


uniform int lightOn;
uniform int lightype;

vec3 CalculateLight(int lightOn, int lightType);

void main()
{
	vec3 result = CalculateLight(lightOn, lightype);
    FragColor = vec4(result, 1.0);
} 


vec3 CalculateLight(int lightOn, int lightype){
	if(lightOn == 1){
		
		vec3 diffuse = vec3(0.0);
		vec3 specular = vec3(0.0);	
		
		//fragment normal
		vec3 norm = normalize(Normal);
		//light direction from fragment towards light
		vec3 lightDir = normalize(directionalLight.position - FragPos);
		
		//Directional light
		// phong ambient	
		vec3 ambient = directionalLight.ambient * texture(diffuseT, TexCoords).rgb;

		// phong diffuse 
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = directionalLight.diffuse * diff * texture(diffuseT, TexCoords).rgb;  
		
		// phong specular	
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), directionalLight.shininess);
		specular = directionalLight.specular * spec * texture(specularT, TexCoords).rgb;  
		//Directional light

		if(lightype == 1){
			//Soft Spotlight
				//degree between spot light and lightDir
			float theta = dot(lightDir, normalize(-softSpotlight.direction));
			float epsilon =	 softSpotlight.cutOff - softSpotlight.cutOffOuter;
			float intensity = clamp((theta - softSpotlight.cutOffOuter) / epsilon, 0.0, 1.0);
			diffuse *= intensity;
			specular *= intensity;
			//
		}
		//if its a spotlight also enable point light
		if(lightype == 2  || lightype == 1){
			//Point light
			float distance = length(pointLight.position - FragPos);
			float attenuation = 1 / (pointLight.kConstant + pointLight.kLinear * distance + pointLight.kQuadratic * distance * distance);
			ambient*=attenuation;
			diffuse*=attenuation;
			specular*=attenuation;
			//
		}

		vec3 result = ambient + diffuse + specular;
		return result;
	}

	else{
		vec3 result = vec3(0.0);
		return result;
	}   
}