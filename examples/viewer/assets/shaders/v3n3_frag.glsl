#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec3 viewPos;

uniform vec3 u_LightPos;
uniform vec3 u_CameraPos;
uniform vec3 u_Color;

void main()
{
    vec3 lightColor = vec3(1.0);

    // AMBIENT
    float ambientFactor = 0.02;
    vec3 ambient = ambientFactor * lightColor;

    // DIFFUSE
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(u_LightPos - fragPos);
    float diffuseFactor = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseFactor * lightColor;

    // SPECULAR
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_CameraPos - fragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;

    vec3 color = (ambient +  diffuse + specular) * u_Color;

    float fogFactor = clamp(-viewPos.z / 5.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.1, 0.1, 0.1);
    gl_FragColor = vec4(mix(color, fogColor, fogFactor), 1.0);
}
