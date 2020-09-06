#version 330 core


struct Material
{
    vec3 tint;

    sampler2D diffuse;
    sampler2D specular;

    float shininess;
    float specular_strength;
};


struct Attenuation
{
    float constant;
    float linear;
    float quadratic;
};


struct DirectionalLight
{
    vec3 normalized_direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct PointLight
{
    Attenuation attenuation;
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct SpotLight
{
    Attenuation attenuation;
    vec3 position;
    vec3 direction;
    float cos_cutoff;
    float cos_outer_cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in vec3 fNormal;
in vec3 fFragmentPosition;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform Material uMaterial;

#define NUMBER_OF_POINT_LIGHTS 4
uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLights[NUMBER_OF_POINT_LIGHTS];
uniform SpotLight uSpotLight;

uniform vec3 uViewPosition;


float
CalculateAttenuation(Attenuation att, vec3 light_position)
{
    float distance = length(light_position - fFragmentPosition);
    float distance2 = distance * distance;
    float attenuation = 1.0 / (att.constant + att.linear * distance + att.quadratic * distance2);
    return attenuation;
}


vec3
CalculateBaseLight
(
    vec3 light_ambient,
    vec3 light_diffuse,
    vec3 light_specular,
    vec3 object_color,
    vec3 specular_sample,
    vec3 normal,
    vec3 view_direction,
    vec3 light_direction
)
{
    vec3 reflection_direction = reflect(-light_direction, normal);
    float diffuse_component = max(0.0f, dot(normal, light_direction));
    float specular_component = pow(max(0.0f, dot(view_direction, reflection_direction)), uMaterial.shininess);

    vec3 ambient = object_color * light_ambient * uMaterial.tint;
    vec3 diffuse = object_color * light_diffuse * uMaterial.tint * diffuse_component;
    vec3 specular = light_specular * specular_sample * uMaterial.specular_strength * specular_component;

    return ambient + diffuse + specular;
}


vec3
CalculateDirectionalLight
(
    DirectionalLight light,
    vec3 object_color,
    vec3 specular_sample,
    vec3 normal,
    vec3 view_direction
)
{
    vec3 light_direction = -light.normalized_direction;
    return CalculateBaseLight
    (
        light.ambient, light.diffuse, light.specular,
        object_color, specular_sample, normal, view_direction, light_direction
    );
}


vec3
CalculatePointLight
(
    PointLight light,
    vec3 object_color,
    vec3 specular_sample,
    vec3 normal,
    vec3 view_direction
)
{
    vec3 light_direction = normalize(light.position - fFragmentPosition);
    vec3 color = CalculateBaseLight
    (
        light.ambient, light.diffuse, light.specular,
        object_color, specular_sample, normal, view_direction, light_direction
    );
    float attenuation = CalculateAttenuation(light.attenuation, light.position);
    return attenuation * color;
}


float
CalculateSpotLightIntensity(SpotLight light, vec3 light_direction)
{
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cos_cutoff - light.cos_outer_cutoff;
    float intensity = clamp((theta - light.cos_outer_cutoff) / epsilon, 0.0, 1.0);
    return intensity;
}


vec3
CalculateSpotLight
(
    SpotLight light,
    vec3 object_color,
    vec3 specular_sample,
    vec3 normal,
    vec3 view_direction
)
{
    vec3 light_direction = normalize(light.position - fFragmentPosition);

    float intensity = CalculateSpotLightIntensity(light, light_direction);

    vec3 color = CalculateBaseLight
    (
        light.ambient, intensity * light.diffuse, intensity * light.specular,
        object_color, specular_sample, normal, view_direction, light_direction
    );
    float attenuation = CalculateAttenuation(light.attenuation, light.position);
    return attenuation * color;
}


void
main()
{
    vec4 diffuse_sample = texture(uMaterial.diffuse, fTexCoord);
    vec3 specular_sample = texture(uMaterial.specular, fTexCoord).rgb;
    vec3 object_color = fColor.rgb * diffuse_sample.rgb;

    vec3 normal = normalize(fNormal);
    vec3 view_direction = normalize(uViewPosition - fFragmentPosition);
    
    vec3 color = CalculateDirectionalLight(uDirectionalLight, object_color, specular_sample, normal, view_direction);
    color += CalculateSpotLight(uSpotLight, object_color, specular_sample, normal, view_direction);
    for(int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
    {
        color += CalculatePointLight(uPointLights[i], object_color, specular_sample, normal, view_direction);
    }
    FragColor = vec4(color, 1.0f);
}
