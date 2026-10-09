#version 330 core
out vec4 FragColor;

// Light / bulb shader. The bulb is rendered with emission as its base color
// (passed via a uniform), so it looks luminous regardless of nearby lighting.
uniform vec3 lightColor;

void main()
{
    // Slight rim-style boost so the bulb always reads as "on".
    FragColor = vec4(lightColor * 1.3, 1.0);
}
