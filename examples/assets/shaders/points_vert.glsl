#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float radius;
layout(location = 2) in vec3 color;

uniform mat4 u_V;
uniform mat4 u_P;

uniform float u_viewPortHeight;
uniform float u_fovY;

out vec3 vColor;

void main()
{
    vec4 viewPos = u_V * vec4(position, 1.0);
    gl_Position = u_P * viewPos;

    float distance = -viewPos.z;

    float projectedSize = radius * u_viewPortHeight / (distance * tan(u_fovY * 0.5));

    // minimum size for observational visibility
    gl_PointSize = max(projectedSize, 2.0);

    vColor = color;
}
