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
    gl_FragColor = vec4(1.0);
}
