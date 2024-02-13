#version 450 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int v_Entity;

in vec2 TexCoords;

layout (binding = 0)uniform sampler2D gPosition;
layout (binding = 1)uniform sampler2D gNormal;
layout (binding = 2)uniform sampler2D gAlbedoSpec;
layout (binding = 3)uniform sampler2D gEntity;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform float viewWidth;
uniform float viewHeight;
uniform vec3 viewPos;

void main()
{         
    // retrieve data from gbuffer
    vec4 Diffusef = texture(gAlbedoSpec, TexCoords);
    if(Diffusef.a == 0) discard;
    vec3 Diffuse = Diffusef.rgb;

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec4 result = texture(gNormal, TexCoords);
    vec3 Normal = result.rgb;
    float Specular = 1.0;
    
    ivec2 texelCoord = ivec2(int(viewWidth * TexCoords.x), int(viewHeight * TexCoords.y));
    v_Entity = int(texelFetch(gEntity, texelCoord, 0).a);

   // v_Entity = texelCoord.y; 
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].Color * spec * Specular;
            // attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }    
    FragColor = vec4(lighting, Diffusef.a);
}