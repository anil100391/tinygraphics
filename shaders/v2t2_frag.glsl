#version 330 core

in vec2 fragTexCoord;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main()
{
    float alpha = texture(u_Texture, fragTexCoord).r;
    gl_FragColor = vec4(u_Color, alpha);
}
