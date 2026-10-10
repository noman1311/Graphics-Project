#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// Camera uniforms
uniform vec3  camPos;
uniform vec3  camFront;
uniform vec3  camRight;
uniform vec3  camUp;
uniform float fovDegrees;
uniform float aspectRatio;

// Light uniforms
uniform vec3  lightPos;
uniform vec3  lightColor;
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;

// Dynamics & Environment
uniform float time;
uniform int   ttActive;
uniform vec3  ttBallPos;
uniform vec3  ttPlayerPaddlePos;
uniform vec3  ttAiPaddlePos;

// -------------------------------------------------------------
// Data Structures
// -------------------------------------------------------------
struct Ray {
    vec3 origin;
    vec3 dir;
};

struct HitInfo {
    float t;
    vec3  p;
    vec3  normal;
    vec3  albedo;
    float shininess;
    float specularStrength;
    float reflectivity;
    vec3  emission;
    int   id; // Procedural texture ID
};

// -------------------------------------------------------------
// Ray-Primitive Analytic Intersections
// -------------------------------------------------------------

// Ray vs Sphere
bool intersectSphere(Ray r, vec3 center, float radius, inout HitInfo best,
                     vec3 albedo, float shininess, float specStr, float refl, vec3 emission, int id)
{
    vec3 oc = r.origin - center;
    float b = dot(oc, r.dir);
    float c = dot(oc, oc) - radius * radius;
    float disc = b * b - c;
    if (disc < 0.0) return false;

    float sqrtDisc = sqrt(disc);
    float t = -b - sqrtDisc;
    if (t < 0.002) t = -b + sqrtDisc;
    if (t > 0.002 && t < best.t) {
        best.t = t;
        best.p = r.origin + t * r.dir;
        best.normal = normalize(best.p - center);
        best.albedo = albedo;
        best.shininess = shininess;
        best.specularStrength = specStr;
        best.reflectivity = refl;
        best.emission = emission;
        best.id = id;
        return true;
    }
    return false;
}

// Ray vs Axis-Aligned Bounding Box (AABB)
bool intersectAABB(Ray r, vec3 bMin, vec3 bMax, inout HitInfo best,
                   vec3 albedo, float shininess, float specStr, float refl, vec3 emission, int id)
{
    vec3 invD = 1.0 / (abs(r.dir) + vec3(1e-8)) * sign(r.dir);
    vec3 t0 = (bMin - r.origin) * invD;
    vec3 t1 = (bMax - r.origin) * invD;
    vec3 tSmaller = min(t0, t1);
    vec3 tBigger = max(t0, t1);

    float tEnter = max(max(tSmaller.x, tSmaller.y), tSmaller.z);
    float tExit = min(min(tBigger.x, tBigger.y), tBigger.z);

    if (tEnter > tExit || tExit < 0.002) return false;

    float t = tEnter;
    if (t < 0.002) {
        t = tExit;
        if (t < 0.002) return false;
    }

    if (t < best.t) {
        best.t = t;
        best.p = r.origin + t * r.dir;

        // Normal determination via dominant axis distance from center
        vec3 center = (bMin + bMax) * 0.5;
        vec3 halfSize = (bMax - bMin) * 0.5;
        vec3 d = (best.p - center) / halfSize;
        vec3 absD = abs(d);

        if (absD.x > absD.y && absD.x > absD.z)
            best.normal = vec3(sign(d.x), 0.0, 0.0);
        else if (absD.y > absD.z)
            best.normal = vec3(0.0, sign(d.y), 0.0);
        else
            best.normal = vec3(0.0, 0.0, sign(d.z));

        best.albedo = albedo;
        best.shininess = shininess;
        best.specularStrength = specStr;
        best.reflectivity = refl;
        best.emission = emission;
        best.id = id;
        return true;
    }
    return false;
}

// Ray vs Finite Cylinder along Y-axis
bool intersectCylinderY(Ray r, vec3 centerXZ, float radius, float yMin, float yMax, inout HitInfo best,
                        vec3 albedo, float shininess, float specStr, float refl, vec3 emission, int id)
{
    vec2 ro = r.origin.xz - centerXZ.xz;
    vec2 rd = r.dir.xz;
    float a = dot(rd, rd);
    if (a < 1e-7) return false;

    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;
    float disc = b * b - a * c;
    bool hit = false;

    if (disc >= 0.0) {
        float sqrtDisc = sqrt(disc);
        float t = (-b - sqrtDisc) / a;
        if (t > 0.002 && t < best.t) {
            float y = r.origin.y + t * r.dir.y;
            if (y >= yMin && y <= yMax) {
                best.t = t;
                best.p = r.origin + t * r.dir;
                best.normal = normalize(vec3(best.p.x - centerXZ.x, 0.0, best.p.z - centerXZ.z));
                best.albedo = albedo;
                best.shininess = shininess;
                best.specularStrength = specStr;
                best.reflectivity = refl;
                best.emission = emission;
                best.id = id;
                hit = true;
            }
        }
    }

    // Top and Bottom flat circular caps
    if (abs(r.dir.y) > 1e-6) {
        float tTop = (yMax - r.origin.y) / r.dir.y;
        if (tTop > 0.002 && tTop < best.t) {
            vec3 p = r.origin + tTop * r.dir;
            if (length(p.xz - centerXZ.xz) <= radius) {
                best.t = tTop;
                best.p = p;
                best.normal = vec3(0.0, 1.0, 0.0);
                best.albedo = albedo;
                best.shininess = shininess;
                best.specularStrength = specStr;
                best.reflectivity = refl;
                best.emission = emission;
                best.id = id;
                hit = true;
            }
        }
        float tBot = (yMin - r.origin.y) / r.dir.y;
        if (tBot > 0.002 && tBot < best.t) {
            vec3 p = r.origin + tBot * r.dir;
            if (length(p.xz - centerXZ.xz) <= radius) {
                best.t = tBot;
                best.p = p;
                best.normal = vec3(0.0, -1.0, 0.0);
                best.albedo = albedo;
                best.shininess = shininess;
                best.specularStrength = specStr;
                best.reflectivity = refl;
                best.emission = emission;
                best.id = id;
                hit = true;
            }
        }
    }
    return hit;
}

// Ray vs Finite Cylinder along Z-axis (for Wall Clock bezel & dial)
bool intersectCylinderZ(Ray r, vec2 centerXY, float radius, float zMin, float zMax, inout HitInfo best,
                        vec3 albedo, float shininess, float specStr, float refl, vec3 emission, int id)
{
    vec2 ro = r.origin.xy - centerXY;
    vec2 rd = r.dir.xy;
    float a = dot(rd, rd);
    if (a < 1e-7) return false;

    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;
    float disc = b * b - a * c;
    bool hit = false;

    if (disc >= 0.0) {
        float sqrtDisc = sqrt(disc);
        float t = (-b - sqrtDisc) / a;
        if (t > 0.002 && t < best.t) {
            float z = r.origin.z + t * r.dir.z;
            if (z >= zMin && z <= zMax) {
                best.t = t;
                best.p = r.origin + t * r.dir;
                best.normal = normalize(vec3(best.p.x - centerXY.x, best.p.y - centerXY.y, 0.0));
                best.albedo = albedo;
                best.shininess = shininess;
                best.specularStrength = specStr;
                best.reflectivity = refl;
                best.emission = emission;
                best.id = id;
                hit = true;
            }
        }
    }

    // Front cap (facing +Z into the room)
    if (abs(r.dir.z) > 1e-6) {
        float tFront = (zMax - r.origin.z) / r.dir.z;
        if (tFront > 0.002 && tFront < best.t) {
            vec3 p = r.origin + tFront * r.dir;
            if (length(p.xy - centerXY) <= radius) {
                best.t = tFront;
                best.p = p;
                best.normal = vec3(0.0, 0.0, 1.0);
                best.albedo = albedo;
                best.shininess = shininess;
                best.specularStrength = specStr;
                best.reflectivity = refl;
                best.emission = emission;
                best.id = id;
                hit = true;
            }
        }
    }
    return hit;
}

// -------------------------------------------------------------
// Fast Shadow Occlusion Checkers
// -------------------------------------------------------------
bool occludeSphere(Ray r, vec3 center, float radius, float maxDist)
{
    vec3 oc = r.origin - center;
    float b = dot(oc, r.dir);
    float c = dot(oc, oc) - radius * radius;
    float disc = b * b - c;
    if (disc < 0.0) return false;
    float sqrtDisc = sqrt(disc);
    float t = -b - sqrtDisc;
    if (t < 0.002) t = -b + sqrtDisc;
    return (t > 0.002 && t < maxDist);
}

bool occludeAABB(Ray r, vec3 bMin, vec3 bMax, float maxDist)
{
    vec3 invD = 1.0 / (abs(r.dir) + vec3(1e-8)) * sign(r.dir);
    vec3 t0 = (bMin - r.origin) * invD;
    vec3 t1 = (bMax - r.origin) * invD;
    vec3 tSmaller = min(t0, t1);
    vec3 tBigger = max(t0, t1);

    float tEnter = max(max(tSmaller.x, tSmaller.y), tSmaller.z);
    float tExit = min(min(tBigger.x, tBigger.y), tBigger.z);

    if (tEnter > tExit || tExit < 0.002) return false;
    float t = (tEnter >= 0.002) ? tEnter : tExit;
    return (t > 0.002 && t < maxDist);
}

bool occludeCylinderY(Ray r, vec3 centerXZ, float radius, float yMin, float yMax, float maxDist)
{
    vec2 ro = r.origin.xz - centerXZ.xz;
    vec2 rd = r.dir.xz;
    float a = dot(rd, rd);
    if (a < 1e-7) return false;

    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;
    float disc = b * b - a * c;
    if (disc >= 0.0) {
        float sqrtDisc = sqrt(disc);
        float t = (-b - sqrtDisc) / a;
        if (t > 0.002 && t < maxDist) {
            float y = r.origin.y + t * r.dir.y;
            if (y >= yMin && y <= yMax) return true;
        }
    }
    if (abs(r.dir.y) > 1e-6) {
        float tTop = (yMax - r.origin.y) / r.dir.y;
        if (tTop > 0.002 && tTop < maxDist) {
            vec3 p = r.origin + tTop * r.dir;
            if (length(p.xz - centerXZ.xz) <= radius) return true;
        }
    }
    return false;
}

// -------------------------------------------------------------
// Scene Geometry: Hit Detection
// -------------------------------------------------------------
bool intersectScene(Ray r, inout HitInfo hit)
{
    bool hasHit = false;

    // Room Shell (30m wide, 15m tall, 30m long: X in [-15,15], Y in [0,15], Z in [-15,15])
    // 1. Floor (Y = 0.0, procedural hardwood planks)
    if (intersectAABB(r, vec3(-15.0, -0.2, -15.0), vec3(15.0, 0.0, 15.0), hit,
                      vec3(0.55, 0.35, 0.20), 24.0, 0.35, 0.18, vec3(0.0), 1)) hasHit = true;

    // 2. Ceiling (Y = 15.0)
    if (intersectAABB(r, vec3(-15.0, 15.0, -15.0), vec3(15.0, 15.2, 15.0), hit,
                      vec3(0.92, 0.92, 0.94), 8.0, 0.04, 0.0, vec3(0.0), 0)) hasHit = true;

    // 3. Walls (Left X = -15, Right X = +15, Front Z = +15)
    vec3 plasterCol = vec3(0.93, 0.91, 0.86);
    if (intersectAABB(r, vec3(-15.2, 0.0, -15.0), vec3(-15.0, 15.0, 15.0), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(15.0, 0.0, -15.0), vec3(15.2, 15.0, 15.0), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-15.0, 0.0, 15.0), vec3(15.0, 15.0, 15.2), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;

    // 4. Back Wall (Z = -15) with Window cutout: X in [-3, 3], Y in [4, 8]
    // Left segment
    if (intersectAABB(r, vec3(-15.0, 0.0, -15.2), vec3(-3.0, 15.0, -15.0), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;
    // Right segment
    if (intersectAABB(r, vec3(3.0, 0.0, -15.2), vec3(15.0, 15.0, -15.0), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;
    // Top segment
    if (intersectAABB(r, vec3(-3.0, 8.0, -15.2), vec3(3.0, 15.0, -15.0), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;
    // Bottom segment
    if (intersectAABB(r, vec3(-3.0, 0.0, -15.2), vec3(3.0, 4.0, -15.0), hit,
                      plasterCol, 8.0, 0.03, 0.0, vec3(0.0), 0)) hasHit = true;

    // 5. Window Frame & Glass Pane
    vec3 frameWood = vec3(0.86, 0.78, 0.62);
    // Outer window frame
    if (intersectAABB(r, vec3(-3.12, 8.0, -15.0), vec3(3.12, 8.12, -14.75), hit,
                      frameWood, 24.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-3.25, 3.85, -15.0), vec3(3.25, 4.0, -14.60), hit,
                      frameWood, 24.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true; // Sill
    if (intersectAABB(r, vec3(-3.12, 4.0, -15.0), vec3(-3.0, 8.0, -14.75), hit,
                      frameWood, 24.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(3.0, 4.0, -15.0), vec3(3.12, 8.0, -14.75), hit,
                      frameWood, 24.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;
    // Mullion
    if (intersectAABB(r, vec3(-3.0, 5.97, -14.95), vec3(3.0, 6.03, -14.85), hit,
                      frameWood, 24.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;

    // Window Glass Pane (pale blue tint, high specular reflection, semi-translucent)
    if (intersectAABB(r, vec3(-2.98, 4.02, -14.92), vec3(2.98, 7.98, -14.90), hit,
                      vec3(0.70, 0.85, 0.95), 128.0, 0.80, 0.45, vec3(0.08, 0.12, 0.16), 5)) hasHit = true;

    // Exterior Landscape Backdrop (visible through the window at Z = -40)
    if (intersectAABB(r, vec3(-30.0, 4.0, -40.1), vec3(30.0, 30.0, -40.0), hit,
                      vec3(0.55, 0.75, 0.98), 4.0, 0.0, 0.0, vec3(0.40, 0.60, 0.85), 0)) hasHit = true; // Sky
    if (intersectAABB(r, vec3(-30.0, -10.0, -40.1), vec3(30.0, 4.0, -40.0), hit,
                      vec3(0.20, 0.50, 0.22), 4.0, 0.0, 0.0, vec3(0.12, 0.28, 0.14), 0)) hasHit = true; // Grass

    // 6. Door (West wall, centered at Z = -3.0, X = -14.94)
    vec3 doorWood = vec3(0.68, 0.46, 0.28);
    if (intersectAABB(r, vec3(-14.98, 0.0, -4.30), vec3(-14.92, 6.45, -1.70), hit,
                      doorWood, 24.0, 0.25, 0.08, vec3(0.0), 0)) hasHit = true;
    // Brass handle & lever
    if (intersectAABB(r, vec3(-14.92, 3.10, -2.18), vec3(-14.86, 3.45, -2.02), hit,
                      vec3(0.92, 0.82, 0.40), 64.0, 0.80, 0.60, vec3(0.0), 0)) hasHit = true;

    // 7. Wall Clock (Centered on back wall at X = 0.0, Y = 10.8, Z = -14.8)
    // Bezel
    if (intersectCylinderZ(r, vec2(0.0, 10.8), 1.25, -14.98, -14.80, hit,
                           vec3(0.18, 0.14, 0.11), 32.0, 0.35, 0.05, vec3(0.0), 0)) hasHit = true;
    // Brass ring
    if (intersectCylinderZ(r, vec2(0.0, 10.8), 1.18, -14.80, -14.77, hit,
                           vec3(0.85, 0.72, 0.35), 64.0, 0.70, 0.55, vec3(0.0), 0)) hasHit = true;
    // Porcelain Dial face with functional live running hands
    if (intersectCylinderZ(r, vec2(0.0, 10.8), 1.12, -14.77, -14.75, hit,
                           vec3(0.98, 0.98, 0.96), 32.0, 0.25, 0.10, vec3(0.0), 4)) hasHit = true;
    // Center brass dome hub
    if (intersectSphere(r, vec3(0.0, 10.8, -14.72), 0.08, hit,
                        vec3(0.88, 0.78, 0.38), 64.0, 0.75, 0.65, vec3(0.0), 0)) hasHit = true;

    // 8. Ceiling Light Fixture (Bulb center at lightPos: (0.0, 14.55, -2.0))
    // Mount & socket
    if (intersectCylinderY(r, lightPos, 0.30, 14.90, 15.00, hit,
                           vec3(0.20, 0.18, 0.16), 32.0, 0.30, 0.0, vec3(0.0), 0)) hasHit = true;
    if (intersectCylinderY(r, lightPos, 0.09, 14.75, 14.90, hit,
                           vec3(0.85, 0.82, 0.78), 32.0, 0.30, 0.0, vec3(0.0), 0)) hasHit = true;
    // Luminous Bulb Sphere
    if (intersectSphere(r, lightPos, 0.34, hit,
                        lightColor, 64.0, 0.50, 0.0, lightColor * 2.2, 0)) hasHit = true;

    // 9. Table Tennis Table (Center (0, 3.0, 0), size 8.0 x 0.2 x 5.0)
    // Tabletop
    if (intersectAABB(r, vec3(-4.0, 2.90, -2.5), vec3(4.0, 3.10, 2.5), hit,
                      vec3(0.07, 0.28, 0.58), 32.0, 0.30, 0.15, vec3(0.0), 3)) hasHit = true;
    // 4 Legs
    vec3 ttLegCol = vec3(0.28, 0.28, 0.30);
    if (intersectAABB(r, vec3(-3.9, 0.0, -2.4), vec3(-3.7, 2.90, -2.2), hit, ttLegCol, 32.0, 0.3, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3( 3.7, 0.0, -2.4), vec3( 3.9, 2.90, -2.2), hit, ttLegCol, 32.0, 0.3, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-3.9, 0.0,  2.2), vec3(-3.7, 2.90,  2.4), hit, ttLegCol, 32.0, 0.3, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3( 3.7, 0.0,  2.2), vec3( 3.9, 2.90,  2.4), hit, ttLegCol, 32.0, 0.3, 0.05, vec3(0.0), 0)) hasHit = true;
    // Net
    if (intersectAABB(r, vec3(-0.02, 3.10, -2.5), vec3(0.02, 3.56, 2.5), hit,
                      vec3(0.92, 0.94, 0.96), 16.0, 0.10, 0.0, vec3(0.0), 0)) hasHit = true;
    // Net posts
    if (intersectCylinderY(r, vec3(0.0, 0.0, -2.5), 0.035, 3.10, 3.58, hit, vec3(0.2), 64.0, 0.5, 0.1, vec3(0.0), 0)) hasHit = true;
    if (intersectCylinderY(r, vec3(0.0, 0.0,  2.5), 0.035, 3.10, 3.58, hit, vec3(0.2), 64.0, 0.5, 0.1, vec3(0.0), 0)) hasHit = true;

    // Ping Pong Ball (High-gloss reflective sphere)
    vec3 curBallPos = (ttActive == 1) ? ttBallPos : vec3(0.5, 3.155, 0.5);
    if (intersectSphere(r, curBallPos, 0.05, hit,
                        vec3(0.98, 0.98, 0.95), 128.0, 0.80, 0.35, vec3(0.0), 6)) hasHit = true;

    // Table Tennis Paddles
    if (ttActive == 1) {
        // Live animated paddles
        if (intersectCylinderY(r, ttPlayerPaddlePos, 0.20, ttPlayerPaddlePos.y - 0.02, ttPlayerPaddlePos.y + 0.02, hit,
                               vec3(0.85, 0.12, 0.12), 48.0, 0.40, 0.10, vec3(0.0), 0)) hasHit = true;
        if (intersectCylinderY(r, ttAiPaddlePos, 0.20, ttAiPaddlePos.y - 0.02, ttAiPaddlePos.y + 0.02, hit,
                               vec3(0.12, 0.25, 0.85), 48.0, 0.40, 0.10, vec3(0.0), 0)) hasHit = true;
    } else {
        // Resting paddles
        if (intersectCylinderY(r, vec3(2.0, 0.0, 0.0), 0.20, 3.10, 3.14, hit,
                               vec3(0.85, 0.12, 0.12), 48.0, 0.40, 0.10, vec3(0.0), 0)) hasHit = true;
        if (intersectCylinderY(r, vec3(-2.0, 0.0, 0.0), 0.20, 3.10, 3.14, hit,
                               vec3(0.12, 0.25, 0.85), 48.0, 0.40, 0.10, vec3(0.0), 0)) hasHit = true;
    }

    // 10. Dining / Chess Table (Center (8.0, 2.05, -5.0), size 4.0 x 0.15 x 3.0)
    // Tabletop (polished wood with reflections)
    if (intersectAABB(r, vec3(6.0, 1.975, -6.5), vec3(10.0, 2.125, -3.5), hit,
                      vec3(0.72, 0.52, 0.32), 32.0, 0.35, 0.20, vec3(0.0), 0)) hasHit = true;
    // 4 Legs
    vec3 dLegCol = vec3(0.38, 0.22, 0.12);
    if (intersectAABB(r, vec3(6.1, 0.0, -6.4), vec3(6.3, 1.975, -6.2), hit, dLegCol, 24.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(9.7, 0.0, -6.4), vec3(9.9, 1.975, -6.2), hit, dLegCol, 24.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(6.1, 0.0, -3.8), vec3(6.3, 1.975, -3.6), hit, dLegCol, 24.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(9.7, 0.0, -3.8), vec3(9.9, 1.975, -3.6), hit, dLegCol, 24.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;

    // Chess Board (Center (8.0, 2.15, -5.0), size 1.6 x 0.05 x 1.6, checkered pattern)
    if (intersectAABB(r, vec3(7.20, 2.125, -5.80), vec3(8.80, 2.175, -4.20), hit,
                      vec3(0.92, 0.88, 0.78), 48.0, 0.45, 0.28, vec3(0.0), 2)) hasHit = true;

    // Chess Clock (Body on table)
    if (intersectAABB(r, vec3(6.78, 2.125, -5.14), vec3(6.92, 2.265, -4.86), hit,
                      vec3(0.40, 0.22, 0.12), 24.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;

    // Chess Pieces (Featured pieces on board: Kings, Queens, Rooks)
    vec3 whitePieceCol = vec3(0.96, 0.94, 0.88);
    vec3 blackPieceCol = vec3(0.12, 0.10, 0.10);
    // White King at e1 / g1
    if (intersectCylinderY(r, vec3(8.34, 0.0, -5.60), 0.045, 2.175, 2.325, hit,
                           whitePieceCol, 64.0, 0.60, 0.20, vec3(0.0), 0)) hasHit = true;
    if (intersectSphere(r, vec3(8.34, 2.34, -5.60), 0.035, hit,
                        whitePieceCol, 64.0, 0.60, 0.25, vec3(0.0), 0)) hasHit = true;
    // White Queen at d1
    if (intersectCylinderY(r, vec3(7.83, 0.0, -5.60), 0.043, 2.175, 2.315, hit,
                           whitePieceCol, 64.0, 0.60, 0.20, vec3(0.0), 0)) hasHit = true;
    if (intersectSphere(r, vec3(7.83, 2.33, -5.60), 0.032, hit,
                        whitePieceCol, 64.0, 0.60, 0.25, vec3(0.0), 0)) hasHit = true;
    // Black King at e8 / g8
    if (intersectCylinderY(r, vec3(8.34, 0.0, -4.40), 0.045, 2.175, 2.325, hit,
                           blackPieceCol, 64.0, 0.60, 0.20, vec3(0.0), 0)) hasHit = true;
    if (intersectSphere(r, vec3(8.34, 2.34, -4.40), 0.035, hit,
                        blackPieceCol, 64.0, 0.60, 0.25, vec3(0.0), 0)) hasHit = true;
    // Black Queen at d8
    if (intersectCylinderY(r, vec3(7.83, 0.0, -4.40), 0.043, 2.175, 2.315, hit,
                           blackPieceCol, 64.0, 0.60, 0.20, vec3(0.0), 0)) hasHit = true;
    if (intersectSphere(r, vec3(7.83, 2.33, -4.40), 0.032, hit,
                        blackPieceCol, 64.0, 0.60, 0.25, vec3(0.0), 0)) hasHit = true;

    // Chairs (North & South of table)
    vec3 chairWood = vec3(0.42, 0.25, 0.14);
    vec3 chairSeat = vec3(0.55, 0.30, 0.20);
    // Chair North (Z = -2.5)
    if (intersectAABB(r, vec3(7.4, 1.125, -3.1), vec3(8.6, 1.275, -1.9), hit, chairSeat, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(7.4, 1.275, -2.0), vec3(8.6, 2.700, -1.9), hit, chairWood, 24.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    // Chair South (Z = -7.5)
    if (intersectAABB(r, vec3(7.4, 1.125, -8.1), vec3(8.6, 1.275, -6.9), hit, chairSeat, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(7.4, 1.275, -8.1), vec3(8.6, 2.700, -8.0), hit, chairWood, 24.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;

    // 11. Carrom Board 1 (West: Center (-8.0, 3.5, 5.0), size 3.0 x 0.1 x 3.0)
    // Board surface
    if (intersectAABB(r, vec3(-9.5, 3.45, 3.5), vec3(-6.5, 3.55, 6.5), hit,
                      vec3(0.92, 0.86, 0.72), 32.0, 0.35, 0.20, vec3(0.0), 9)) hasHit = true;
    // Frame borders
    vec3 carromBorder = vec3(0.55, 0.30, 0.15);
    if (intersectAABB(r, vec3(-9.6, 3.55, 3.4), vec3(-9.4, 3.75, 6.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-6.6, 3.55, 3.4), vec3(-6.4, 3.75, 6.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-9.4, 3.55, 3.4), vec3(-6.6, 3.75, 3.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-9.4, 3.55, 6.4), vec3(-6.6, 3.75, 6.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    // Legs
    if (intersectAABB(r, vec3(-9.5, 0.0, 3.5), vec3(-9.3, 3.45, 3.7), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-6.7, 0.0, 3.5), vec3(-6.5, 3.45, 3.7), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-9.5, 0.0, 6.3), vec3(-9.3, 3.45, 6.5), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(-6.7, 0.0, 6.3), vec3(-6.5, 3.45, 6.5), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    // Center Red Queen coin & Striker
    if (intersectCylinderY(r, vec3(-8.0, 0.0, 5.0), 0.08, 3.55, 3.60, hit,
                           vec3(0.88, 0.12, 0.14), 64.0, 0.60, 0.25, vec3(0.0), 0)) hasHit = true;
    if (intersectCylinderY(r, vec3(-8.0, 0.0, 6.0), 0.11, 3.55, 3.61, hit,
                           vec3(0.85, 0.35, 0.12), 64.0, 0.65, 0.25, vec3(0.0), 0)) hasHit = true;

    // 12. Carrom Board 2 (East: Center (8.0, 3.5, 5.0), size 3.0 x 0.1 x 3.0)
    if (intersectAABB(r, vec3(6.5, 3.45, 3.5), vec3(9.5, 3.55, 6.5), hit,
                      vec3(0.92, 0.86, 0.72), 32.0, 0.35, 0.20, vec3(0.0), 9)) hasHit = true;
    if (intersectAABB(r, vec3(6.4, 3.55, 3.4), vec3(6.6, 3.75, 6.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(9.4, 3.55, 3.4), vec3(9.6, 3.75, 6.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(6.6, 3.55, 3.4), vec3(9.4, 3.75, 3.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(6.6, 3.55, 6.4), vec3(9.4, 3.75, 6.6), hit, carromBorder, 24.0, 0.3, 0.08, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(6.5, 0.0, 3.5), vec3(6.7, 3.45, 3.7), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(9.3, 0.0, 3.5), vec3(9.5, 3.45, 3.7), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(6.5, 0.0, 6.3), vec3(6.7, 3.45, 6.5), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(9.3, 0.0, 6.3), vec3(9.5, 3.45, 6.5), hit, dLegCol, 16.0, 0.2, 0.05, vec3(0.0), 0)) hasHit = true;

    // 13. Almirahs (Left: (-10, 4, -13), Right: (10, 4, -13))
    vec3 almWood = vec3(0.65, 0.42, 0.26);
    if (intersectAABB(r, vec3(-12.0, 0.0, -14.0), vec3(-8.0, 8.0, -12.0), hit, almWood, 20.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;
    if (intersectAABB(r, vec3(  8.0, 0.0, -14.0), vec3(12.0, 8.0, -12.0), hit, almWood, 20.0, 0.25, 0.05, vec3(0.0), 0)) hasHit = true;

    return hasHit;
}

// -------------------------------------------------------------
// Scene Geometry: Shadow Occlusion Test
// -------------------------------------------------------------
bool intersectSceneOcclusion(Ray r, float maxDist)
{
    // Floor & ceiling don't occlude the downward point light from room objects
    // Test primary room occluders that cast prominent shadows:

    // 1. Table Tennis Table & Net
    if (occludeAABB(r, vec3(-4.0, 2.90, -2.5), vec3(4.0, 3.10, 2.5), maxDist)) return true;
    if (occludeAABB(r, vec3(-0.02, 3.10, -2.5), vec3(0.02, 3.56, 2.5), maxDist)) return true;

    // 2. Table Tennis Legs
    if (occludeAABB(r, vec3(-3.9, 0.0, -2.4), vec3(-3.7, 2.90, -2.2), maxDist)) return true;
    if (occludeAABB(r, vec3( 3.7, 0.0, -2.4), vec3( 3.9, 2.90, -2.2), maxDist)) return true;
    if (occludeAABB(r, vec3(-3.9, 0.0,  2.2), vec3(-3.7, 2.90,  2.4), maxDist)) return true;
    if (occludeAABB(r, vec3( 3.7, 0.0,  2.2), vec3( 3.9, 2.90,  2.4), maxDist)) return true;

    // 3. Ping Pong Ball
    vec3 curBallPos = (ttActive == 1) ? ttBallPos : vec3(0.5, 3.155, 0.5);
    if (occludeSphere(r, curBallPos, 0.05, maxDist)) return true;

    // 4. Dining / Chess Table & Board
    if (occludeAABB(r, vec3(6.0, 1.975, -6.5), vec3(10.0, 2.125, -3.5), maxDist)) return true;
    if (occludeAABB(r, vec3(7.20, 2.125, -5.80), vec3(8.80, 2.175, -4.20), maxDist)) return true;
    // Dining table legs
    if (occludeAABB(r, vec3(6.1, 0.0, -6.4), vec3(6.3, 1.975, -6.2), maxDist)) return true;
    if (occludeAABB(r, vec3(9.7, 0.0, -6.4), vec3(9.9, 1.975, -6.2), maxDist)) return true;
    if (occludeAABB(r, vec3(6.1, 0.0, -3.8), vec3(6.3, 1.975, -3.6), maxDist)) return true;
    if (occludeAABB(r, vec3(9.7, 0.0, -3.8), vec3(9.9, 1.975, -3.6), maxDist)) return true;

    // 5. Chairs
    if (occludeAABB(r, vec3(7.4, 1.125, -3.1), vec3(8.6, 1.275, -1.9), maxDist)) return true;
    if (occludeAABB(r, vec3(7.4, 1.275, -2.0), vec3(8.6, 2.700, -1.9), maxDist)) return true;
    if (occludeAABB(r, vec3(7.4, 1.125, -8.1), vec3(8.6, 1.275, -6.9), maxDist)) return true;
    if (occludeAABB(r, vec3(7.4, 1.275, -8.1), vec3(8.6, 2.700, -8.0), maxDist)) return true;

    // 6. Carrom Boards 1 & 2
    if (occludeAABB(r, vec3(-9.5, 3.45, 3.5), vec3(-6.5, 3.55, 6.5), maxDist)) return true;
    if (occludeAABB(r, vec3( 6.5, 3.45, 3.5), vec3( 9.5, 3.55, 6.5), maxDist)) return true;

    // 7. Almirahs
    if (occludeAABB(r, vec3(-12.0, 0.0, -14.0), vec3(-8.0, 8.0, -12.0), maxDist)) return true;
    if (occludeAABB(r, vec3(  8.0, 0.0, -14.0), vec3(12.0, 8.0, -12.0), maxDist)) return true;

    return false;
}

// -------------------------------------------------------------
// Procedural Shader Enhancements for Hits
// -------------------------------------------------------------
void applyProceduralMaterial(inout HitInfo hit)
{
    if (hit.id == 1) {
        // Floor: Procedural Hardwood Planks
        float plankWidth = 0.5;
        float plankLen = 2.5;
        float plankIdx = floor(hit.p.x / plankWidth);
        float stagger = fract(plankIdx * 0.37) * plankLen;
        float u = fract(hit.p.x / plankWidth);
        float v = fract((hit.p.z + stagger) / plankLen);

        // Seams
        float seamX = smoothstep(0.0, 0.035, u) * smoothstep(1.0, 0.965, u);
        float seamZ = smoothstep(0.0, 0.015, v) * smoothstep(1.0, 0.985, v);
        float seam = seamX * seamZ;

        // Wood tone variation per plank
        float tone = fract(plankIdx * 0.23 + floor((hit.p.z + stagger) / plankLen) * 0.17);
        vec3 woodTone = mix(vec3(0.52, 0.33, 0.19), vec3(0.66, 0.44, 0.27), tone);
        hit.albedo = woodTone * (0.35 + 0.65 * seam);
    }
    else if (hit.id == 2) {
        // Chess Board: 8x8 Checkered Grid
        vec2 boardUV = (hit.p.xz - vec2(8.0, -5.0)) / 1.36 + 0.5;
        if (boardUV.x >= 0.0 && boardUV.x <= 1.0 && boardUV.y >= 0.0 && boardUV.y <= 1.0) {
            vec2 grid = floor(boardUV * 8.0);
            float check = mod(grid.x + grid.y, 2.0);
            hit.albedo = (check > 0.5) ? vec3(0.92, 0.88, 0.78) : vec3(0.24, 0.14, 0.08);
            hit.reflectivity = 0.28; // Polished chess board
        }
    }
    else if (hit.id == 3) {
        // Table Tennis Surface: Crisp White Border & Center Line
        bool border = (abs(hit.p.x) > 3.92) || (abs(hit.p.z) > 2.42);
        bool centerLine = (abs(hit.p.z) < 0.02);
        if (border || centerLine) {
            hit.albedo = vec3(0.95, 0.95, 0.95);
            hit.reflectivity = 0.08;
        } else {
            hit.albedo = vec3(0.07, 0.28, 0.58);
            hit.reflectivity = 0.15;
        }
    }
    else if (hit.id == 4) {
        // Wall Clock Face: Live Functional Hands & Railway Track
        vec2 dialCenter = vec2(0.0, 10.8);
        vec2 relP = hit.p.xy - dialCenter;
        float r = length(relP);
        float angle = atan(relP.x, relP.y); // 0 at 12 o'clock, +clockwise
        if (angle < 0.0) angle += 6.2831853;

        // Outer minute ring track (r in [0.95, 1.00])
        vec3 dialColor = vec3(0.98, 0.98, 0.96);
        if (r > 0.94 && r < 0.98) {
            // Minute / second ticks
            float tickFrac = fract(angle / (6.2831853 / 60.0));
            if (tickFrac < 0.15 || tickFrac > 0.85) dialColor = vec3(0.15);
        }
        // Bold Hour Markers (12 markers)
        if (r > 0.82 && r < 0.98) {
            float hourFrac = fract(angle / (6.2831853 / 12.0));
            if (hourFrac < 0.07 || hourFrac > 0.93) dialColor = vec3(0.05);
        }

        // Functional live hands computed from `time`
        float secAngle = mod(time * (6.2831853 / 60.0), 6.2831853);
        float minAngle = mod((time / 60.0) * (6.2831853 / 60.0), 6.2831853);
        float hourAngle = mod((time / 3600.0) * (6.2831853 / 12.0), 6.2831853);

        // Hour Hand (shorter & broader)
        float dHour = abs(sin(angle - hourAngle)) * r;
        if (r < 0.58 && dHour < 0.038) dialColor = vec3(0.06);

        // Minute Hand (longer)
        float dMin = abs(sin(angle - minAngle)) * r;
        if (r < 0.88 && dMin < 0.026) dialColor = vec3(0.06);

        // Second Hand (scarlet red, slender)
        float dSec = abs(sin(angle - secAngle)) * r;
        if (r < 0.95 && dSec < 0.012) dialColor = vec3(0.95, 0.10, 0.10);

        hit.albedo = dialColor;
    }
}

// -------------------------------------------------------------
// Optical Ray Tracing Core: Direct Lighting, Shadows & Reflections
// -------------------------------------------------------------
vec3 traceRay(Ray initialRay)
{
    vec3 finalColor = vec3(0.0);
    vec3 throughput = vec3(1.0);
    Ray r = initialRay;

    // Trace primary ray + up to 2 recursive specular reflection bounces
    for (int bounce = 0; bounce < 3; ++bounce)
    {
        HitInfo hit;
        hit.t = 1e9;
        if (!intersectScene(r, hit)) {
            // Miss: background ambient void
            vec3 voidColor = vec3(0.05, 0.05, 0.07);
            finalColor += throughput * voidColor;
            break;
        }

        // Apply procedural textures (hardwood floor, chess grid, TT lines, clock dial)
        applyProceduralMaterial(hit);

        // Vector to light source
        vec3 lightVec = lightPos - hit.p;
        float distToLight = length(lightVec);
        vec3 lightDir = lightVec / distToLight;

        // Attenuation
        float atten = 1.0 / (lightConstant + lightLinear * distToLight + lightQuadratic * distToLight * distToLight);

        // Ray-Casted Optical Occlusion Shadow
        Ray shadowRay;
        shadowRay.origin = hit.p + hit.normal * 0.003;
        shadowRay.dir = lightDir;
        float shadowFactor = 1.0;
        if (intersectSceneOcclusion(shadowRay, distToLight - 0.05)) {
            shadowFactor = 0.0; // Point is in true optical occlusion shadow!
        }

        // Ambient Lighting: Dual-Hemisphere Fill
        float skyFactor = clamp(hit.normal.y * 0.5 + 0.5, 0.0, 1.0);
        vec3 skyAmbient = vec3(0.38, 0.40, 0.44);
        vec3 groundAmbient = vec3(0.32, 0.29, 0.25);
        vec3 ambientColor = mix(groundAmbient, skyAmbient, skyFactor) * lightColor;
        vec3 ambient = ambientColor * hit.albedo;

        // Diffuse Direct Illumination
        float diff = max(dot(hit.normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * hit.albedo * atten * shadowFactor;

        // Specular Highlights (Blinn-Phong)
        vec3 viewDir = -r.dir;
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(hit.normal, halfDir), 0.0), hit.shininess);
        vec3 specular = hit.specularStrength * spec * lightColor * atten * shadowFactor;

        // Direct Surface Luminescence
        vec3 surfaceColor = ambient + diffuse + specular + hit.emission;

        // If surface has no specular reflectivity or we reached max bounce, blend and terminate
        if (hit.reflectivity <= 0.02 || bounce == 2) {
            finalColor += throughput * surfaceColor;
            break;
        }

        // Fresnel-Schlick Reflectivity Blending
        float cosTheta = clamp(dot(viewDir, hit.normal), 0.0, 1.0);
        float fresnel = hit.reflectivity + (1.0 - hit.reflectivity) * pow(1.0 - cosTheta, 5.0);
        fresnel = clamp(fresnel, 0.0, 0.95);

        // Accumulate unreflected surface radiance
        finalColor += throughput * surfaceColor * (1.0 - fresnel);

        // Attenuate throughput for reflected ray
        throughput *= fresnel;

        // Spawn secondary specular reflection ray
        r.origin = hit.p + hit.normal * 0.003;
        r.dir = reflect(r.dir, hit.normal);
    }

    return finalColor;
}

// -------------------------------------------------------------
// Main Shader Entry Point
// -------------------------------------------------------------
void main()
{
    // Generate camera ray matching OpenGL perspective view matrix exactly
    vec2 ndc = TexCoord * 2.0 - 1.0;
    float tanFov = tan(radians(fovDegrees * 0.5));
    vec3 rayDir = normalize(camFront + ndc.x * camRight * (tanFov * aspectRatio) + ndc.y * camUp * tanFov);

    Ray primaryRay;
    primaryRay.origin = camPos;
    primaryRay.dir = rayDir;

    vec3 color = traceRay(primaryRay);

    // Subtle Reinhard tone mapping & gamma correction for beautiful HDR contrast
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 1.15));

    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
