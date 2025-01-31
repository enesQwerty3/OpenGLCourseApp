#version 330 core

struct DirectionalLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shinines;
};

struct PointLight{
    vec3 position;
    float kLinear;
    float kConstant; // = 1
    float kQuadratic;
};

struct SoftSpotlight{
    vec3 position;
    vec3 direction;
    float cutOff;
    float cutOffOuter;
};

//in and outs
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

//textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

//light attributes
uniform vec3 viewPos;
uniform bool directionalLightOn = true;
uniform bool flashlightOn = false;

//lights
uniform DirectionalLight directionalLight;

uniform DirectionalLight flashDirectioanlLight;
uniform PointLight flashPointLight;
uniform SoftSpotlight flashSoftSpotlight;

//functions
vec3 CalculateDirectionalLight(DirectionalLight light);
vec3 CalculateFlashLight(DirectionalLight dLight, PointLight pLight, SoftSpotlight sLight);

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = mix(texture2D(texture_diffuse1, TexCoords), texture2D(texture_specular1, TexCoords), 0.2);

    vec3 result = vec3(0.0);
    if(directionalLightOn){
        result += CalculateDirectionalLight(directionalLight);
    }

    if(flashlightOn){
        result += CalculateFlashLight(flashDirectioanlLight, flashPointLight, flashSoftSpotlight);
    }

    FragColor = vec4(result, 1.0);
}

//phong lighting
vec3 CalculateDirectionalLight(DirectionalLight light){
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    float diff = 0.0;
    vec3 specular = vec3(0.0);
    float spec = 0.0;

    vec3 normal = normalize(Normal);
    //ambient light
    ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;

    //diffuse light
    vec3 LightDir = normalize(light.position - FragPos); //direction of directional light
    diff = max(dot(normal, LightDir), 0.0);
    diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    //specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-LightDir, normal);
    spec = pow(max(dot(reflectDir, viewDir), 0.0), light.shinines);
    specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;

    vec3 result = ambient + diffuse + specular;
    return result;
}

//flashlight
vec3 CalculateFlashLight(DirectionalLight dLight, PointLight pLight, SoftSpotlight sLight){
    vec3 light = CalculateDirectionalLight(dLight);
    //point light
    float distance = length(pLight.position - FragPos);
    float attenuation = (1/(pLight.kConstant + distance * pLight.kLinear + distance * distance * pLight.kQuadratic));
    light*=attenuation;

    //soft spotlight
    vec3 spotlightDir = normalize(sLight.position - FragPos);
    float theta = dot(spotlightDir, normalize(-sLight.direction)); //cos value of angle between spotlight and light direction
    float epsilon = sLight.cutOff - sLight.cutOffOuter;
    float intensity = clamp((theta - sLight.cutOffOuter) / epsilon, 0.0, 1.0);
    light *= intensity;

    vec3 result = vec3(light);
    return result;
}