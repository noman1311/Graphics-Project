#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

// A simple Phong material. Use Material::Make() helpers for common materials.
struct Material {
    glm::vec3 baseColor;     // RGB base color (multiplied with diffuse texture if present)
    float     shininess;     // specular exponent
    float     specularStrength; // 0..1
    glm::vec3 emission;      // emissive component (for bulb, sky exterior, etc.)
    int       diffuseTex;    // OpenGL texture id, or 0 for none
    float     texScale;      // multiplier on UVs (e.g., wood grain tiling)

    static Material Make(glm::vec3 base, float shin = 32.0f, float spec = 0.4f,
                         glm::vec3 emiss = glm::vec3(0.0f), int tex = 0, float scale = 1.0f)
    {
        Material m;
        m.baseColor = base;
        m.shininess = shin;
        m.specularStrength = spec;
        m.emission = emiss;
        m.diffuseTex = tex;
        m.texScale = scale;
        return m;
    }
};

#endif
