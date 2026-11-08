#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragTexCoord;

uniform float u_Time;

uniform vec3 u_LightPos;
uniform vec3 u_CameraPos;

// Simple procedural noise
float hash(vec3 p)
{
    p = fract(p * 0.3183099 + vec3(0.1));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);

    f = f * f * (3.0 - 2.0 * f);

    return mix(
        mix(mix(hash(i), hash(i+vec3(1,0,0)), f.x),
            mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x), f.y),

        mix(mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
            mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x), f.y),
        f.z
    );
}

void main()
{
    vec3 p = normalize(fragNormal);

    // Animated solar turbulence
    float n1 = noise(p * 9.0 + vec3(u_Time * 0.15));
    float n2 = noise(p * 20.0 - vec3(u_Time * 0.25));

    float plasma = n1 * 0.65 + n2 * 0.35;

    // Solar color gradient
    vec3 darkOrange = vec3(1.0, 0.12, 0.01);
    vec3 orange = vec3(1.0, 0.38, 0.02);
    vec3 yellow = vec3(1.0, 0.85, 0.20);
    vec3 whiteHot = vec3(1.0, 0.98, 0.78);

    vec3 color = mix(darkOrange, orange, smoothstep(0.1, 0.4, plasma));
    color = mix(color, yellow, smoothstep(0.35, 0.7, plasma));
    color = mix(color, whiteHot, smoothstep(0.72, 0.95, plasma));

    // High emission for HDR bloom
    color *= 3.0;

    gl_FragColor = vec4(color, 1.0);
}
