#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec3 fragPos;
out vec2 fragTexCoord;
out mat3 TBN;

void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
    fragPos = vec3(u_M * vec4(position, 1.0));
    fragTexCoord = texCoord;
    vec3 T = normalize( vec3( u_M * vec4( tangent, 0.0 ) ) );
    vec3 B = normalize( vec3( u_M * vec4( bitangent, 0.0 ) ) );
    vec3 N = normalize( vec3( u_M * vec4( normal, 0.0 ) ) );
    TBN = mat3( T, B, N );
}
