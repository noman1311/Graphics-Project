#include "Scene.h"
#include "ProceduralTextures.h"
// ProceduralTextures::Xxx generators are defined in src/ProceduralTextures.cpp.

Scene::Scene()
    : cube(PrimitiveFactory::CreateCube()),
      cylinder(PrimitiveFactory::CreateCylinder()),
      sphere(PrimitiveFactory::CreateSphere()),
      plane(PrimitiveFactory::CreatePlane())
{
    // Build procedural textures once at startup.
    texWoodPlanks  = ProceduralTextures::WoodPlanks(1024, 1024);
    texDarkWood    = ProceduralTextures::DarkWood(512, 512);
    texLightWood   = ProceduralTextures::LightWood(512, 512);
    texPlaster     = ProceduralTextures::Plaster(512, 512);
    texCeilingMatte= ProceduralTextures::CeilingMatte(512, 512);
    texTableTennis = ProceduralTextures::TableTennisSurface(512, 512);
    texSky         = ProceduralTextures::Sky(512, 512);
    texGrass       = ProceduralTextures::Grass(512, 512);
    texFabric      = ProceduralTextures::Fabric(256, 256);
    // Glass: an off-white "milky" texture we tint blue in the material.
    texGlass       = ProceduralTextures::CeilingMatte(64, 64);

    // Light setup: place it slightly offset from room center along -Z so it
    // hangs above the table tennis table, and tune attenuation for a 15m room.
    // lightPos matches the bulb's geometric center (yCeil - 0.45 = 14.55).
    lightPos        = glm::vec3(0.0f, 14.55f, -2.0f);
    lightColor      = glm::vec3(1.00f, 0.92f, 0.78f);  // warm white ~3200K
    lightConstant   = 1.0f;
    lightLinear     = 0.045f;
    lightQuadratic  = 0.0075f;
}

void Scene::setMaterial(Shader& shader, const Material& m) const
{
    shader.setVec3 ("materialBaseColor",         m.baseColor);
    shader.setFloat("materialShininess",         m.shininess);
    shader.setFloat("materialSpecularStrength",  m.specularStrength);
    shader.setVec3 ("materialEmission",          m.emission);
    shader.setInt  ("useDiffuseTexture",         m.diffuseTex != 0 ? 1 : 0);
    shader.setFloat("materialTexScale",          m.texScale);
    if (m.diffuseTex != 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m.diffuseTex);
        shader.setInt("diffuseTexture", 0);
    }
}

void Scene::drawMesh(Shader& shader, Mesh& mesh,
                     glm::vec3 position, glm::vec3 scale,
                     const Material& material, glm::vec3 rotation)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (rotation.x != 0.0f) model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (rotation.y != 0.0f) model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotation.z != 0.0f) model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    setMaterial(shader, material);
    mesh.Draw();
}

void Scene::DrawEnvironment(Shader& shader)
{
    // Floor
    drawMesh(shader, cube,
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(30.0f, 0.1f, 30.0f),
        Material::Make(glm::vec3(1.0f), 16.0f, 0.18f,
                       glm::vec3(0.0f), texWoodPlanks.id, 6.0f));

    // Ceiling
    drawMesh(shader, cube,
        glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(30.0f, 0.1f, 30.0f),
        Material::Make(glm::vec3(1.0f), 8.0f, 0.04f,
                       glm::vec3(0.0f), texCeilingMatte.id, 4.0f));

    // Left and right solid walls (no windows)
    drawMesh(shader, cube,
        glm::vec3(-15.0f, 7.5f, 0.0f), glm::vec3(0.1f, 15.0f, 30.0f),
        Material::Make(glm::vec3(1.0f), 8.0f, 0.03f,
                       glm::vec3(0.0f), texPlaster.id, 4.0f));
    drawMesh(shader, cube,
        glm::vec3(15.0f, 7.5f, 0.0f), glm::vec3(0.1f, 15.0f, 30.0f),
        Material::Make(glm::vec3(1.0f), 8.0f, 0.03f,
                       glm::vec3(0.0f), texPlaster.id, 4.0f));

    // Front wall (z = +15) - solid
    drawMesh(shader, cube,
        glm::vec3(0.0f, 7.5f, 15.0f), glm::vec3(30.0f, 15.0f, 0.1f),
        Material::Make(glm::vec3(1.0f), 8.0f, 0.03f,
                       glm::vec3(0.0f), texPlaster.id, 4.0f));

    // Back wall (z = -15) is split into 4 segments around the window opening.
    // Window opening is 6 wide, 4 tall, sill at y=4, top at y=8.
    // Wall extends 30 in x, 15 in y, centered at (0, 7.5, -15).
    //   Left segment:  x: -15..-3, y: 0..15
    //   Right segment: x:   3..15, y: 0..15
    //   Top segment:   x:  -3.. 3, y: 8..15
    //   Bottom segment:x:  -3.. 3, y: 0..4
    auto plasterMat = Material::Make(glm::vec3(1.0f), 8.0f, 0.03f,
                                    glm::vec3(0.0f), texPlaster.id, 4.0f);
    // Left (width 12, full height 15, x center = -9)
    drawMesh(shader, cube, glm::vec3(-9.0f, 7.5f, -15.0f),
        glm::vec3(12.0f, 15.0f, 0.1f), plasterMat);
    // Right (width 12, full height 15, x center = 9)
    drawMesh(shader, cube, glm::vec3(9.0f, 7.5f, -15.0f),
        glm::vec3(12.0f, 15.0f, 0.1f), plasterMat);
    // Top (width 6, height 7, y center = 11.5)
    drawMesh(shader, cube, glm::vec3(0.0f, 11.5f, -15.0f),
        glm::vec3(6.0f, 7.0f, 0.1f), plasterMat);
    // Bottom (width 6, height 4, y center = 2.0)
    drawMesh(shader, cube, glm::vec3(0.0f, 2.0f, -15.0f),
        glm::vec3(6.0f, 4.0f, 0.1f), plasterMat);

    // A thin floor baseboard strip around the room (runs along inside of walls).
    auto baseboard = Material::Make(glm::vec3(0.92f, 0.88f, 0.78f), 16.0f, 0.05f);
    float baseboardH = 0.18f;
    drawMesh(shader, cube, glm::vec3(0.0f, baseboardH * 0.5f, -14.95f),
        glm::vec3(30.0f, baseboardH, 0.05f), baseboard);
    drawMesh(shader, cube, glm::vec3(0.0f, baseboardH * 0.5f, 14.95f),
        glm::vec3(30.0f, baseboardH, 0.05f), baseboard);
    drawMesh(shader, cube, glm::vec3(-14.95f, baseboardH * 0.5f, 0.0f),
        glm::vec3(0.05f, baseboardH, 30.0f), baseboard);
    drawMesh(shader, cube, glm::vec3(14.95f, baseboardH * 0.5f, 0.0f),
        glm::vec3(0.05f, baseboardH, 30.0f), baseboard);
}

void Scene::DrawDoor(Shader& shader)
{
    // Door situated on the Left/West wall (X = -15.0f), centered at Z = -3.0f
    auto doorFrameMat = Material::Make(glm::vec3(0.50f, 0.30f, 0.16f), 24.0f, 0.25f,
                                      glm::vec3(0.0f), texDarkWood.id, 2.0f);
    auto doorLeafMat  = Material::Make(glm::vec3(0.72f, 0.50f, 0.30f), 24.0f, 0.25f,
                                      glm::vec3(0.0f), texLightWood.id, 2.0f);
    auto panelInsetMat= Material::Make(glm::vec3(0.46f, 0.28f, 0.14f), 16.0f, 0.20f,
                                      glm::vec3(0.0f), texDarkWood.id, 1.0f);
    auto brassMat     = Material::Make(glm::vec3(0.92f, 0.82f, 0.45f), 64.0f, 0.70f);

    float doorZ = -3.0f;
    float doorX = -14.94f; // aligned with inner surface of left wall

    // Architrave / Frame
    // Left jamb (north, Z = -4.35)
    drawMesh(shader, cube, glm::vec3(doorX + 0.01f, 3.25f, doorZ - 1.35f),
        glm::vec3(0.12f, 6.5f, 0.16f), doorFrameMat);
    // Right jamb (south, Z = -1.65)
    drawMesh(shader, cube, glm::vec3(doorX + 0.01f, 3.25f, doorZ + 1.35f),
        glm::vec3(0.12f, 6.5f, 0.16f), doorFrameMat);
    // Header (top lintel, Y = 6.56)
    drawMesh(shader, cube, glm::vec3(doorX + 0.01f, 6.56f, doorZ),
        glm::vec3(0.12f, 0.18f, 2.86f), doorFrameMat);
    // Threshold / saddle along floor
    drawMesh(shader, cube, glm::vec3(doorX + 0.02f, 0.03f, doorZ),
        glm::vec3(0.14f, 0.06f, 2.70f), doorFrameMat);

    // Door Leaf (main paneled slab)
    drawMesh(shader, cube, glm::vec3(doorX, 3.25f, doorZ),
        glm::vec3(0.06f, 6.4f, 2.54f), doorLeafMat);

    // 4 Classic Recessed Door Panels
    // Upper pair (Y = 4.75, height 2.2, width 0.9)
    drawMesh(shader, cube, glm::vec3(doorX + 0.022f, 4.75f, doorZ - 0.65f),
        glm::vec3(0.02f, 2.2f, 0.90f), panelInsetMat);
    drawMesh(shader, cube, glm::vec3(doorX + 0.022f, 4.75f, doorZ + 0.65f),
        glm::vec3(0.02f, 2.2f, 0.90f), panelInsetMat);
    // Lower pair (Y = 1.75, height 2.0, width 0.9)
    drawMesh(shader, cube, glm::vec3(doorX + 0.022f, 1.75f, doorZ - 0.65f),
        glm::vec3(0.02f, 2.0f, 0.90f), panelInsetMat);
    drawMesh(shader, cube, glm::vec3(doorX + 0.022f, 1.75f, doorZ + 0.65f),
        glm::vec3(0.02f, 2.0f, 0.90f), panelInsetMat);

    // Door Hardware: Brass Backplate & Lever Handle
    drawMesh(shader, cube, glm::vec3(doorX + 0.038f, 3.20f, doorZ + 0.95f),
        glm::vec3(0.02f, 0.55f, 0.12f), brassMat);
    // Stem protruding from door
    drawMesh(shader, cylinder, glm::vec3(doorX + 0.08f, 3.30f, doorZ + 0.95f),
        glm::vec3(0.04f, 0.09f, 0.04f), brassMat, glm::vec3(0.0f, 0.0f, 90.0f));
    // Horizontal lever handle
    drawMesh(shader, cylinder, glm::vec3(doorX + 0.13f, 3.30f, doorZ + 0.85f),
        glm::vec3(0.035f, 0.24f, 0.035f), brassMat, glm::vec3(90.0f, 0.0f, 0.0f));
    // Keyhole
    drawMesh(shader, cylinder, glm::vec3(doorX + 0.045f, 3.05f, doorZ + 0.95f),
        glm::vec3(0.03f, 0.02f, 0.03f), Material::Make(glm::vec3(0.12f), 16.0f, 0.1f), glm::vec3(0.0f, 0.0f, 90.0f));
}

void Scene::DrawCeilingLight(Shader& phongShader, Shader& lightShader)
{
    // ----- Ceiling fixture: mount plate, socket/housing, bulb -----
    // Place it directly under the ceiling at the same XZ as lightPos.
    float yCeil = 15.0f;

    // Mount plate: thin disc at the ceiling.
    drawMesh(phongShader, cylinder,
        glm::vec3(lightPos.x, yCeil - 0.05f, lightPos.z),
        glm::vec3(0.6f, 0.05f, 0.6f),
        Material::Make(glm::vec3(0.18f, 0.16f, 0.14f), 32.0f, 0.4f));

    // Socket / housing: short cylinder.
    drawMesh(phongShader, cylinder,
        glm::vec3(lightPos.x, yCeil - 0.20f, lightPos.z),
        glm::vec3(0.18f, 0.20f, 0.18f),
        Material::Make(glm::vec3(0.85f, 0.82f, 0.78f), 32.0f, 0.3f));

    // Bulb (frosted diffuser): a sphere slightly above the lightPos, so the
    // light source is roughly the bulb's geometric center.
    float bulbRadius = 0.32f;
    glm::vec3 bulbCenter = glm::vec3(lightPos.x, yCeil - 0.45f, lightPos.z);
    Material bulbMat = Material::Make(glm::vec3(1.0f, 0.95f, 0.85f),
                                      64.0f, 0.6f,
                                      lightColor * 1.4f);
    drawMesh(phongShader, sphere, bulbCenter,
        glm::vec3(bulbRadius * 2.0f), bulbMat);

    // The bright bulb halo using lightShader (unlit, fully bright).
    // The caller (Scene::Draw) already set view/projection on the lightShader.
    lightShader.use();
    drawMesh(lightShader, sphere, bulbCenter,
        glm::vec3(bulbRadius * 1.15f),  // slightly larger than the bulb body
        Material::Make(lightColor, 1.0f, 0.0f, lightColor));
    // Restore the Phong shader so subsequent draw calls behave correctly.
    phongShader.use();
}

void Scene::DrawWindow(Shader& shader)
{
    // Window opening: 6 wide, 4 tall, sill y=4, top y=8, in the back wall.
    // The frame is a light wood, surrounding the opening.
    float xL = -3.0f, xR = 3.0f;
    float yB = 4.0f,  yT = 8.0f;
    float zFrame = -14.92f;  // sits just inside the wall plane (-15)
    float frameT = 0.12f;    // frame thickness
    float frameD = 0.20f;    // how far the frame protrudes inward (+Z)

    auto frameMat = Material::Make(glm::vec3(0.86f, 0.78f, 0.62f),
                                   32.0f, 0.30f,
                                   glm::vec3(0.0f), texLightWood.id, 1.0f);

    // Outer frame (4 pieces): top, bottom, left, right
    // Top horizontal
    drawMesh(shader, cube,
        glm::vec3(0.0f, yT + frameT * 0.5f, zFrame + frameD * 0.5f),
        glm::vec3(xR - xL + frameT * 2.0f, frameT, frameD),
        frameMat);
    // Bottom horizontal (also acts as the sill - make it thicker/deeper)
    drawMesh(shader, cube,
        glm::vec3(0.0f, yB - 0.10f, zFrame + 0.10f),
        glm::vec3(xR - xL + 0.5f, 0.18f, 0.40f),
        frameMat);
    // Left vertical
    drawMesh(shader, cube,
        glm::vec3(xL - frameT * 0.5f, (yB + yT) * 0.5f, zFrame + frameD * 0.5f),
        glm::vec3(frameT, yT - yB, frameD),
        frameMat);
    // Right vertical
    drawMesh(shader, cube,
        glm::vec3(xR + frameT * 0.5f, (yB + yT) * 0.5f, zFrame + frameD * 0.5f),
        glm::vec3(frameT, yT - yB, frameD),
        frameMat);

    // Central horizontal mullion
    drawMesh(shader, cube,
        glm::vec3(0.0f, (yB + yT) * 0.5f, zFrame + frameD * 0.5f),
        glm::vec3(xR - xL, 0.06f, 0.05f),
        frameMat);

    // Sill (interior ledge protruding from the wall)
    drawMesh(shader, cube,
        glm::vec3(0.0f, yB - 0.05f, zFrame + 0.18f),
        glm::vec3(xR - xL + 0.5f, 0.08f, 0.30f),
        frameMat);

    // Glass pane: a thin slab just behind the frame, in two halves
    // (above and below the central mullion). Use a low-spec, slightly emissive
    // material so it never goes black.
    Material glassMat = Material::Make(
        glm::vec3(0.65f, 0.78f, 0.86f),  // pale blue tint
        96.0f,                            // high shininess
        0.20f,                            // weak specular
        glm::vec3(0.20f, 0.24f, 0.28f),   // emission to keep it readable
        texGlass.id, 1.0f);

    // Top pane
    drawMesh(shader, cube,
        glm::vec3(0.0f, (yT + (yB + yT) * 0.5f) * 0.5f, zFrame + 0.005f),
        glm::vec3(xR - xL - 0.05f, yT - (yB + yT) * 0.5f - 0.02f, 0.04f),
        glassMat);
    // Bottom pane
    drawMesh(shader, cube,
        glm::vec3(0.0f, (yB + (yB + yT) * 0.5f) * 0.5f, zFrame + 0.005f),
        glm::vec3(xR - xL - 0.05f, (yB + yT) * 0.5f - yB - 0.02f, 0.04f),
        glassMat);
}

void Scene::DrawExterior(Shader& shader)
{
    // The "exterior" visible through the window.
    // 1) A big sky background plane far behind the window.
    // 2) A grass strip below it.
    // We render this without any lighting (set emission-only material).

    // Sky panel (centered behind window at z = -40)
    Material skyMat = Material::Make(
        glm::vec3(1.0f), 1.0f, 0.0f,
        glm::vec3(1.0f), texSky.id, 1.0f);
    // emissive so it's not affected by indoor light (override behavior below
    // is achieved by setting emission to 1.0 - the ambient term still applies,
    // which is fine here).
    drawMesh(shader, cube,
        glm::vec3(0.0f, 8.0f, -40.0f),
        glm::vec3(60.0f, 30.0f, 0.1f),
        skyMat);

    // Ground / grass below the sky horizon
    Material grassMat = Material::Make(
        glm::vec3(1.0f), 4.0f, 0.0f,
        glm::vec3(0.0f), texGrass.id, 1.0f);
    drawMesh(shader, cube,
        glm::vec3(0.0f, -2.0f, -40.0f),
        glm::vec3(60.0f, 16.0f, 0.1f),
        grassMat);

    // A subtle distant tree silhouette line
    drawMesh(shader, cube,
        glm::vec3(-12.0f, 5.0f, -39.5f),
        glm::vec3(4.0f, 8.0f, 0.1f),
        Material::Make(glm::vec3(0.20f, 0.36f, 0.22f), 4.0f, 0.0f));
    drawMesh(shader, cube,
        glm::vec3(11.0f, 4.5f, -39.5f),
        glm::vec3(5.0f, 7.0f, 0.1f),
        Material::Make(glm::vec3(0.20f, 0.36f, 0.22f), 4.0f, 0.0f));
}

void Scene::DrawFurniture(Shader& shader)
{
    // ============== Table ==============
    // Tabletop (light wood)
    drawMesh(shader, cube,
        glm::vec3(8.0f, 2.05f, -5.0f),
        glm::vec3(4.0f, 0.15f, 3.0f),
        Material::Make(glm::vec3(1.0f), 32.0f, 0.30f,
                       glm::vec3(0.0f), texLightWood.id, 2.0f));
    // Apron strip under the tabletop
    drawMesh(shader, cube,
        glm::vec3(8.0f, 1.92f, -5.0f),
        glm::vec3(3.9f, 0.10f, 2.9f),
        Material::Make(glm::vec3(0.65f, 0.45f, 0.28f), 24.0f, 0.25f,
                       glm::vec3(0.0f), texDarkWood.id, 2.0f));
    // Legs (dark wood)
    auto tableLeg = Material::Make(glm::vec3(1.0f), 32.0f, 0.30f,
                                   glm::vec3(0.0f), texDarkWood.id, 1.0f);
    drawMesh(shader, cube, glm::vec3(9.8f, 1.0f, -3.7f), glm::vec3(0.2f, 1.9f, 0.2f), tableLeg);
    drawMesh(shader, cube, glm::vec3(9.8f, 1.0f, -6.3f), glm::vec3(0.2f, 1.9f, 0.2f), tableLeg);
    drawMesh(shader, cube, glm::vec3(6.2f, 1.0f, -3.7f), glm::vec3(0.2f, 1.9f, 0.2f), tableLeg);
    drawMesh(shader, cube, glm::vec3(6.2f, 1.0f, -6.3f), glm::vec3(0.2f, 1.9f, 0.2f), tableLeg);

    // ============== Chairs ==============
    // Chair 1 (closer to camera, north side of table)
    auto chairWood = Material::Make(glm::vec3(1.0f), 32.0f, 0.30f,
                                    glm::vec3(0.0f), texDarkWood.id, 1.0f);
    auto chairSeat = Material::Make(glm::vec3(0.55f, 0.30f, 0.20f),
                                    16.0f, 0.18f,
                                    glm::vec3(0.0f), texFabric.id, 2.0f);

    // Seat
    drawMesh(shader, cube, glm::vec3(8.0f, 1.2f, -2.5f),
        glm::vec3(1.2f, 0.15f, 1.2f), chairSeat);
    // Seat frame (under the cushion)
    drawMesh(shader, cube, glm::vec3(8.0f, 1.10f, -2.5f),
        glm::vec3(1.2f, 0.05f, 1.2f), chairWood);
    // Top rail
    drawMesh(shader, cube, glm::vec3(8.0f, 2.7f, -1.95f),
        glm::vec3(1.2f, 0.18f, 0.13f), chairWood);
    // Stiles
    drawMesh(shader, cube, glm::vec3(8.525f, 2.05f, -1.95f),
        glm::vec3(0.13f, 1.5f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.475f, 2.05f, -1.95f),
        glm::vec3(0.13f, 1.5f, 0.13f), chairWood);
    // Slats
    drawMesh(shader, cube, glm::vec3(8.25f, 2.05f, -1.95f),
        glm::vec3(0.08f, 1.5f, 0.10f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.0f, 2.05f, -1.95f),
        glm::vec3(0.08f, 1.5f, 0.10f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.75f, 2.05f, -1.95f),
        glm::vec3(0.08f, 1.5f, 0.10f), chairWood);
    // Legs
    drawMesh(shader, cube, glm::vec3(8.525f, 0.6f, -1.975f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.475f, 0.6f, -1.975f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.525f, 0.6f, -3.025f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.475f, 0.6f, -3.025f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);

    // Chair 2 (south side of table)
    drawMesh(shader, cube, glm::vec3(8.0f, 1.2f, -7.5f),
        glm::vec3(1.2f, 0.15f, 1.2f), chairSeat);
    drawMesh(shader, cube, glm::vec3(8.0f, 1.10f, -7.5f),
        glm::vec3(1.2f, 0.05f, 1.2f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.0f, 2.7f, -8.05f),
        glm::vec3(1.2f, 0.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.525f, 2.05f, -8.05f),
        glm::vec3(0.13f, 1.5f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.475f, 2.05f, -8.05f),
        glm::vec3(0.13f, 1.5f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.25f, 2.05f, -8.05f),
        glm::vec3(0.08f, 1.5f, 0.10f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.0f, 2.05f, -8.05f),
        glm::vec3(0.08f, 1.5f, 0.10f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.75f, 2.05f, -8.05f),
        glm::vec3(0.08f, 1.5f, 0.10f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.525f, 0.6f, -6.975f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.475f, 0.6f, -6.975f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(8.525f, 0.6f, -8.025f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);
    drawMesh(shader, cube, glm::vec3(7.475f, 0.6f, -8.025f),
        glm::vec3(0.13f, 1.18f, 0.13f), chairWood);

    // ============== Almirahs ==============
    // Wood-textured almirah with door panels and handles.
    auto almirahBody = Material::Make(glm::vec3(0.65f, 0.42f, 0.26f),
                                      16.0f, 0.20f,
                                      glm::vec3(0.0f), texDarkWood.id, 2.0f);
    auto almirahDoor = Material::Make(glm::vec3(0.75f, 0.55f, 0.34f),
                                      16.0f, 0.20f,
                                      glm::vec3(0.0f), texLightWood.id, 2.0f);
    auto handleMat   = Material::Make(glm::vec3(0.85f, 0.78f, 0.45f),
                                      64.0f, 0.7f); // brass-ish

    // Almirah 1 (left back corner)
    drawMesh(shader, cube, glm::vec3(-10.0f, 4.0f, -13.0f),
        glm::vec3(4.0f, 8.0f, 2.0f), almirahBody);
    // Doors
    drawMesh(shader, cube, glm::vec3(-10.98f, 4.0f, -11.95f),
        glm::vec3(1.95f, 7.8f, 0.08f), almirahDoor);
    drawMesh(shader, cube, glm::vec3(-9.02f, 4.0f, -11.95f),
        glm::vec3(1.95f, 7.8f, 0.08f), almirahDoor);
    // Door panel inset (subtle relief)
    drawMesh(shader, cube, glm::vec3(-10.98f, 4.0f, -11.90f),
        glm::vec3(1.6f, 6.5f, 0.02f),
        Material::Make(glm::vec3(0.55f, 0.36f, 0.20f), 12.0f, 0.18f,
                       glm::vec3(0.0f), texDarkWood.id, 1.0f));
    drawMesh(shader, cube, glm::vec3(-9.02f, 4.0f, -11.90f),
        glm::vec3(1.6f, 6.5f, 0.02f),
        Material::Make(glm::vec3(0.55f, 0.36f, 0.20f), 12.0f, 0.18f,
                       glm::vec3(0.0f), texDarkWood.id, 1.0f));
    // Handles (brass)
    drawMesh(shader, cylinder, glm::vec3(-10.2f, 4.0f, -11.85f),
        glm::vec3(0.08f, 0.6f, 0.08f), handleMat, glm::vec3(90.0f, 0.0f, 0.0f));
    drawMesh(shader, cylinder, glm::vec3(-9.8f, 4.0f, -11.85f),
        glm::vec3(0.08f, 0.6f, 0.08f), handleMat, glm::vec3(90.0f, 0.0f, 0.0f));

    // Almirah 2 (right back corner)
    drawMesh(shader, cube, glm::vec3(10.0f, 4.0f, -13.0f),
        glm::vec3(4.0f, 8.0f, 2.0f), almirahBody);
    drawMesh(shader, cube, glm::vec3(9.02f, 4.0f, -11.95f),
        glm::vec3(1.95f, 7.8f, 0.08f), almirahDoor);
    drawMesh(shader, cube, glm::vec3(10.98f, 4.0f, -11.95f),
        glm::vec3(1.95f, 7.8f, 0.08f), almirahDoor);
    drawMesh(shader, cube, glm::vec3(9.02f, 4.0f, -11.90f),
        glm::vec3(1.6f, 6.5f, 0.02f),
        Material::Make(glm::vec3(0.55f, 0.36f, 0.20f), 12.0f, 0.18f,
                       glm::vec3(0.0f), texDarkWood.id, 1.0f));
    drawMesh(shader, cube, glm::vec3(10.98f, 4.0f, -11.90f),
        glm::vec3(1.6f, 6.5f, 0.02f),
        Material::Make(glm::vec3(0.55f, 0.36f, 0.20f), 12.0f, 0.18f,
                       glm::vec3(0.0f), texDarkWood.id, 1.0f));
    drawMesh(shader, cylinder, glm::vec3(9.8f, 4.0f, -11.85f),
        glm::vec3(0.08f, 0.6f, 0.08f), handleMat, glm::vec3(90.0f, 0.0f, 0.0f));
    drawMesh(shader, cylinder, glm::vec3(10.2f, 4.0f, -11.85f),
        glm::vec3(0.08f, 0.6f, 0.08f), handleMat, glm::vec3(90.0f, 0.0f, 0.0f));
}

void Scene::DrawGameSetups(Shader& shader)
{
    // ============== Carrom Board 1 (left, near south side) ==============
    auto carromSurface = Material::Make(glm::vec3(0.92f, 0.78f, 0.50f),
                                        32.0f, 0.30f,
                                        glm::vec3(0.0f), texLightWood.id, 1.0f);
    auto carromFrame   = Material::Make(glm::vec3(0.55f, 0.30f, 0.15f),
                                        24.0f, 0.30f,
                                        glm::vec3(0.0f), texDarkWood.id, 1.0f);
    auto carromLegMat  = Material::Make(glm::vec3(0.40f, 0.22f, 0.10f),
                                        16.0f, 0.20f,
                                        glm::vec3(0.0f), texDarkWood.id, 1.0f);

    drawMesh(shader, cube, glm::vec3(-8.0f, 3.5f, 5.0f),
        glm::vec3(3.0f, 0.10f, 3.0f), carromSurface);
    drawMesh(shader, cube, glm::vec3(-9.4f, 3.65f, 5.0f),
        glm::vec3(0.2f, 0.20f, 3.0f), carromFrame);
    drawMesh(shader, cube, glm::vec3(-6.6f, 3.65f, 5.0f),
        glm::vec3(0.2f, 0.20f, 3.0f), carromFrame);
    drawMesh(shader, cube, glm::vec3(-8.0f, 3.65f, 3.6f),
        glm::vec3(2.6f, 0.20f, 0.2f), carromFrame);
    drawMesh(shader, cube, glm::vec3(-8.0f, 3.65f, 6.4f),
        glm::vec3(2.6f, 0.20f, 0.2f), carromFrame);
    // Corner pockets (dark cylinders, slightly recessed)
    auto pocketMat = Material::Make(glm::vec3(0.05f, 0.05f, 0.05f), 16.0f, 0.05f);
    drawMesh(shader, cylinder, glm::vec3(-9.2f, 3.56f, 3.8f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cylinder, glm::vec3(-9.2f, 3.56f, 6.2f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cylinder, glm::vec3(-6.8f, 3.56f, 3.8f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cylinder, glm::vec3(-6.8f, 3.56f, 6.2f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    // Legs
    drawMesh(shader, cube, glm::vec3(-9.4f, 1.75f, 3.6f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    drawMesh(shader, cube, glm::vec3(-9.4f, 1.75f, 6.4f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    drawMesh(shader, cube, glm::vec3(-6.6f, 1.75f, 3.6f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    drawMesh(shader, cube, glm::vec3(-6.6f, 1.75f, 6.4f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    // Shared carrom piece materials
    auto whiteCoinMat = Material::Make(glm::vec3(0.95f, 0.92f, 0.80f), 64.0f, 0.50f);
    auto blackCoinMat = Material::Make(glm::vec3(0.10f, 0.10f, 0.12f), 64.0f, 0.50f);
    auto queenMat     = Material::Make(glm::vec3(0.88f, 0.12f, 0.14f), 64.0f, 0.60f);
    auto strikerMat   = Material::Make(glm::vec3(0.85f, 0.35f, 0.12f), 64.0f, 0.65f);
    glm::vec3 coinScale(0.16f, 0.05f, 0.16f);
    glm::vec3 strikerScale(0.22f, 0.06f, 0.22f);

    // Carrom Board 1 Pieces: Classic circular opening break formation
    // Center: Red Queen
    drawMesh(shader, cylinder, glm::vec3(-8.000f, 3.575f, 5.000f), coinScale, queenMat);
    // Inner ring (radius 0.17m, 6 coins alternating white & black)
    drawMesh(shader, cylinder, glm::vec3(-7.830f, 3.575f, 5.000f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-7.915f, 3.575f, 5.147f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.085f, 3.575f, 5.147f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.170f, 3.575f, 5.000f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.085f, 3.575f, 4.853f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-7.915f, 3.575f, 4.853f), coinScale, blackCoinMat);
    // Outer ring (radius 0.33m, 6 coins alternating black & white)
    drawMesh(shader, cylinder, glm::vec3(-7.714f, 3.575f, 5.165f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.000f, 3.575f, 5.330f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.286f, 3.575f, 5.165f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.286f, 3.575f, 4.835f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-8.000f, 3.575f, 4.670f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(-7.714f, 3.575f, 4.835f), coinScale, whiteCoinMat);
    // Striker ready on baseline
    drawMesh(shader, cylinder, glm::vec3(-8.000f, 3.580f, 6.000f), strikerScale, strikerMat);

    // ============== Carrom Board 2 (right, near south side) ==============
    drawMesh(shader, cube, glm::vec3(8.0f, 3.5f, 5.0f),
        glm::vec3(3.0f, 0.10f, 3.0f), carromSurface);
    drawMesh(shader, cube, glm::vec3(6.6f, 3.65f, 5.0f),
        glm::vec3(0.2f, 0.20f, 3.0f), carromFrame);
    drawMesh(shader, cube, glm::vec3(9.4f, 3.65f, 5.0f),
        glm::vec3(0.2f, 0.20f, 3.0f), carromFrame);
    drawMesh(shader, cube, glm::vec3(8.0f, 3.65f, 3.6f),
        glm::vec3(2.6f, 0.20f, 0.2f), carromFrame);
    drawMesh(shader, cube, glm::vec3(8.0f, 3.65f, 6.4f),
        glm::vec3(2.6f, 0.20f, 0.2f), carromFrame);
    drawMesh(shader, cylinder, glm::vec3(6.8f, 3.56f, 3.8f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cylinder, glm::vec3(6.8f, 3.56f, 6.2f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cylinder, glm::vec3(9.2f, 3.56f, 3.8f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cylinder, glm::vec3(9.2f, 3.56f, 6.2f), glm::vec3(0.3f, 0.10f, 0.3f), pocketMat);
    drawMesh(shader, cube, glm::vec3(6.6f, 1.75f, 3.6f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    drawMesh(shader, cube, glm::vec3(6.6f, 1.75f, 6.4f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    drawMesh(shader, cube, glm::vec3(9.4f, 1.75f, 3.6f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);
    drawMesh(shader, cube, glm::vec3(9.4f, 1.75f, 6.4f), glm::vec3(0.15f, 3.4f, 0.15f), carromLegMat);

    // Carrom Board 2 Pieces: Active mid-game scattered play
    // Queen
    drawMesh(shader, cylinder, glm::vec3(7.85f, 3.575f, 4.80f), coinScale, queenMat);
    // White coins (in play)
    drawMesh(shader, cylinder, glm::vec3(7.15f, 3.575f, 4.10f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(8.85f, 3.575f, 5.90f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(8.40f, 3.575f, 5.25f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(7.60f, 3.575f, 5.35f), coinScale, whiteCoinMat);
    drawMesh(shader, cylinder, glm::vec3(8.15f, 3.575f, 4.35f), coinScale, whiteCoinMat);
    // Black coins (in play)
    drawMesh(shader, cylinder, glm::vec3(8.85f, 3.575f, 4.15f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(7.20f, 3.575f, 5.85f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(7.95f, 3.575f, 4.50f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(8.45f, 3.575f, 4.75f), coinScale, blackCoinMat);
    drawMesh(shader, cylinder, glm::vec3(7.70f, 3.575f, 5.05f), coinScale, blackCoinMat);
    // Striker angled for a pocket shot
    drawMesh(shader, cylinder, glm::vec3(7.50f, 3.580f, 6.000f), strikerScale, strikerMat);

    // ============== Table Tennis ==============
    // Tabletop with blue textured surface and white border
    drawMesh(shader, cube, glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(8.0f, 0.20f, 5.0f),
        Material::Make(glm::vec3(1.0f), 32.0f, 0.25f,
                       glm::vec3(0.0f), texTableTennis.id, 1.0f));
    // Edge band: a tournament-blue edge cube right under the top
    drawMesh(shader, cube, glm::vec3(0.0f, 2.92f, 0.0f),
        glm::vec3(8.0f, 0.05f, 5.0f),
        Material::Make(glm::vec3(0.05f, 0.15f, 0.38f), 24.0f, 0.20f,
                       glm::vec3(0.0f), texDarkWood.id, 1.0f));

    // Net (white cloth) and posts
    auto netCloth = Material::Make(glm::vec3(0.92f, 0.94f, 0.96f), 16.0f, 0.10f);
    auto netPost  = Material::Make(glm::vec3(0.18f, 0.18f, 0.20f), 64.0f, 0.40f);
    drawMesh(shader, cube, glm::vec3(0.0f, 3.4f, 0.0f),
        glm::vec3(0.04f, 0.30f, 5.0f), netCloth);
    // Top white tape
    drawMesh(shader, cube, glm::vec3(0.0f, 3.56f, 0.0f),
        glm::vec3(0.05f, 0.02f, 5.0f), netCloth);
    // Bottom white tape
    drawMesh(shader, cube, glm::vec3(0.0f, 3.24f, 0.0f),
        glm::vec3(0.05f, 0.02f, 5.0f), netCloth);
    // Posts at each end of the net
    drawMesh(shader, cylinder, glm::vec3(0.0f, 3.4f, 2.5f),
        glm::vec3(0.06f, 0.45f, 0.06f), netPost);
    drawMesh(shader, cylinder, glm::vec3(0.0f, 3.4f, -2.5f),
        glm::vec3(0.06f, 0.45f, 0.06f), netPost);

    // Legs
    auto tableLegMat = Material::Make(glm::vec3(0.30f, 0.30f, 0.32f), 32.0f, 0.30f);
    drawMesh(shader, cube, glm::vec3( 3.8f, 1.5f,  2.3f), glm::vec3(0.2f, 2.9f, 0.2f), tableLegMat);
    drawMesh(shader, cube, glm::vec3(-3.8f, 1.5f,  2.3f), glm::vec3(0.2f, 2.9f, 0.2f), tableLegMat);
    drawMesh(shader, cube, glm::vec3( 3.8f, 1.5f, -2.3f), glm::vec3(0.2f, 2.9f, 0.2f), tableLegMat);
    drawMesh(shader, cube, glm::vec3(-3.8f, 1.5f, -2.3f), glm::vec3(0.2f, 2.9f, 0.2f), tableLegMat);

    // Paddles and ball (resting on the table)
    auto redPaddle  = Material::Make(glm::vec3(0.80f, 0.12f, 0.12f), 32.0f, 0.30f);
    auto bluePaddle = Material::Make(glm::vec3(0.10f, 0.20f, 0.78f), 32.0f, 0.30f);
    auto handleMat2 = Material::Make(glm::vec3(0.40f, 0.25f, 0.15f), 32.0f, 0.25f,
                                     glm::vec3(0.0f), texLightWood.id, 2.0f);

    // Red paddle (lying flat on top of the table at X = 2.0, tabletop surface is Y = 3.10)
    drawMesh(shader, cylinder, glm::vec3(2.0f, 3.125f, 0.0f),
        glm::vec3(0.40f, 0.04f, 0.40f), redPaddle, glm::vec3(0.0f, 0.0f, 0.0f));
    // Red handle (grip attached to blade, resting on the table)
    drawMesh(shader, cylinder, glm::vec3(2.0f, 3.125f, 0.31f),
        glm::vec3(0.08f, 0.32f, 0.04f), handleMat2, glm::vec3(90.0f, 0.0f, 0.0f));

    // Blue paddle (lying flat on top of the table at X = -2.0)
    drawMesh(shader, cylinder, glm::vec3(-2.0f, 3.125f, 0.0f),
        glm::vec3(0.40f, 0.04f, 0.40f), bluePaddle, glm::vec3(0.0f, 0.0f, 0.0f));
    // Blue handle (grip attached to blade, resting on the table)
    drawMesh(shader, cylinder, glm::vec3(-2.0f, 3.125f, -0.31f),
        glm::vec3(0.08f, 0.32f, 0.04f), handleMat2, glm::vec3(90.0f, 0.0f, 0.0f));

    // Ball (white sphere resting on the table)
    drawMesh(shader, sphere, glm::vec3(0.5f, 3.155f, 0.5f),
        glm::vec3(0.10f, 0.10f, 0.10f),
        Material::Make(glm::vec3(1.0f, 0.98f, 0.92f), 64.0f, 0.60f));
}

void Scene::Draw(Shader& phongShader, Shader& lightShader,
                  const glm::mat4& projection, const glm::mat4& view,
                  const glm::vec3& cameraPos)
{
    // ---- Pass 1: everything that uses the Phong shader ----
    phongShader.use();
    phongShader.setMat4("projection", projection);
    phongShader.setMat4("view", view);
    phongShader.setVec3("lightPos",    lightPos);
    phongShader.setVec3("lightColor",  lightColor);
    phongShader.setVec3("viewPos",     cameraPos);
    phongShader.setFloat("lightConstant",  lightConstant);
    phongShader.setFloat("lightLinear",    lightLinear);
    phongShader.setFloat("lightQuadratic", lightQuadratic);

    // Prepare the unlit "light" shader with the same view/projection so the
    // bulb halo pass inside DrawCeilingLight renders in the correct place.
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    lightShader.setVec3("lightColor", lightColor);

    // Now switch back to the Phong shader and draw everything.
    phongShader.use();
    DrawExterior(phongShader);
    DrawEnvironment(phongShader);
    DrawDoor(phongShader);
    DrawWindow(phongShader);
    DrawFurniture(phongShader);
    DrawGameSetups(phongShader);
    DrawCeilingLight(phongShader, lightShader);
}
