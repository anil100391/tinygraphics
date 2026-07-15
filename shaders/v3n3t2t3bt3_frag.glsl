#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragTexCoord;
in mat3 TBN;

uniform vec3 u_LightPos;
uniform vec3 u_CameraPos;
uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;

void main()
{
    vec3 lightColor = vec3(1.0);

    // AMBIENT
    float ambientFactor = 0.02;
    vec3 ambient = ambientFactor * lightColor;

    // DIFFUSE
    // obtain normal from normal map in range [0,1]
    vec3 norm = texture(u_NormalMap, fragTexCoord).rgb;
    // transform normal vector to range [-1,1]
    norm = norm * 2.0 - 1.0;
    norm.xy *= 2.0;
    // recompute z (instead of distorting it)"
    norm.z = sqrt( 1.0 - clamp( dot( norm.xy, norm.xy ), 0.0, 1.0 ) );

    norm = normalize(TBN * norm);

    vec3 lightDir = normalize(u_LightPos - fragPos);
    float diffuseFactor = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseFactor * lightColor;

    // SPECULAR
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_CameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;

    vec3 color = (ambient +  diffuse + specular) * texture(u_Texture, fragTexCoord).rgb;
    // color = (ambient +  diffuse + specular) * diffuse;
    gl_FragColor = vec4(color, 1.0);
}
