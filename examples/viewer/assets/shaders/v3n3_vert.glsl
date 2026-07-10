#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec3 fragNormal;
out vec3 fragPos;
out vec3 viewPos;

void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
    fragPos = vec3(u_M * vec4(position, 1.0));
    viewPos = vec3(u_V * u_M * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(u_M))) * normal;
}
