#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float radius;
layout(location = 3) in float screenSize;

uniform mat4 u_V;
uniform mat4 u_P;

uniform float u_viewPortHeight;
uniform float u_fovY;
uniform float u_pointDrawThresholdSize;
uniform float u_pointDrawMinSize;

out vec3 vColor;

void main()
{
    gl_Position = u_P * u_V * vec4(position, 1.0);

    if ( screenSize <= u_pointDrawThresholdSize )
    {
        float t = screenSize / u_pointDrawThresholdSize;
        gl_PointSize = u_pointDrawMinSize * (1.0 - t) + t * screenSize;
    }
    else
    {
        gl_PointSize = 0.0;
    }

    vColor = color;
}
