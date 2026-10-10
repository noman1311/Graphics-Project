#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// Material
uniform vec3  materialBaseColor;
uniform float materialShininess;
uniform float materialSpecularStrength;
uniform vec3  materialEmission;
uniform int   useDiffuseTexture;
uniform float materialTexScale;
uniform sampler2D diffuseTexture;

// Light (single point light)
uniform vec3  lightPos;
uniform vec3  lightColor;
uniform vec3  viewPos;
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;

void main()
{
    // --- Base albedo (texture or solid color) ---
    vec4 baseSample = vec4(materialBaseColor, 1.0);
    if (useDiffuseTexture == 1)
    {
        baseSample = texture(diffuseTexture, TexCoords * materialTexScale);
        // Multiply by tint color so the texture and the color are blended.
        baseSample.rgb *= materialBaseColor;
    }
    vec3 albedo = baseSample.rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * distance * distance);

    // Ambient: dual-hemisphere ambient fill (soft diffuse room fill from above, warm floor bounce from below)
    // Carefully balanced to illuminate all corners and vertical surfaces without washing out textures or flattening 3D contrast
    float skyFactor = clamp(norm.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 skyAmbient = vec3(0.40, 0.42, 0.45);    // diffuse room skylight fill
    vec3 groundAmbient = vec3(0.35, 0.32, 0.28); // warm floor bounce
    vec3 ambientColor = mix(groundAmbient, skyAmbient, skyFactor) * lightColor;
    vec3 ambient = ambientColor * albedo;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo * attenuation;

    // Specular (Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = materialSpecularStrength * spec * lightColor * attenuation;

    // Emissive (bulbs, glowing markers, etc.)
    vec3 emission = materialEmission;

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(clamp(result, 0.0, 1.0), 1.0);
}
