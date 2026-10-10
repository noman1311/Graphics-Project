# Object Transformations for Indoor Games Room

This document keeps track of the transformations (Translation, Rotation, and Scaling) applied to the basic geometric primitives used to build the objects in the 3D scene.

All transformations are given as vectors in the format `(x, y, z)`.

## Environment

### Floor
*   **Primitive**: Cube
*   **Scaling**: `(30.0, 0.1, 30.0)`
*   **Translation**: `(0.0, -0.05, 0.0)`
*   **Rotation**: `(0.0, 0.0, 0.0)`
*   **Color**: `(0.6, 0.4, 0.2)`

### Walls
**Left Wall**
*   **Primitive**: Cube
*   **Scaling**: `(0.1, 15.0, 30.0)`
*   **Translation**: `(-15.0, 7.5, 0.0)`

**Right Wall**
*   **Primitive**: Cube
*   **Scaling**: `(0.1, 15.0, 30.0)`
*   **Translation**: `(15.0, 7.5, 0.0)`

**Back Wall**
*   **Primitive**: Cube
*   **Scaling**: `(30.0, 15.0, 0.1)`
*   **Translation**: `(0.0, 7.5, -15.0)`

**Front Wall**
*   **Primitive**: Cube
*   **Scaling**: `(30.0, 15.0, 0.1)`
*   **Translation**: `(0.0, 7.5, 15.0)`

### Ceiling
*   **Primitive**: Cube
*   **Scaling**: `(30.0, 0.1, 30.0)`
*   **Translation**: `(0.0, 15.0, 0.0)`
*   **Color**: `(0.9, 0.9, 0.9)`

### Door (West / Left Wall)
*   **Door Leaf**: Cube, Scale `(0.06, 6.4, 2.54)`, Trans `(-14.94, 3.25, -3.0)`
*   **Frame Jambs (x2)**: Cubes, Scale `(0.12, 6.5, 0.16)`, Trans `(-14.93, 3.25, -4.35)` and `(-14.93, 3.25, -1.65)`
*   **Frame Header**: Cube, Scale `(0.12, 0.18, 2.86)`, Trans `(-14.93, 6.56, -3.0)`
*   **Threshold / Saddle**: Cube, Scale `(0.14, 0.06, 2.70)`, Trans `(-14.92, 0.03, -3.0)`
*   **Recessed Inset Panels (x4)**: Cubes, Upper Scale `(0.02, 2.2, 0.90)` at `(-14.918, 4.75, -3.65)` and `(-14.918, 4.75, -2.35)`; Lower Scale `(0.02, 2.0, 0.90)` at `(-14.918, 1.75, -3.65)` and `(-14.918, 1.75, -2.35)`
*   **Brass Backplate**: Cube, Scale `(0.02, 0.55, 0.12)`, Trans `(-14.902, 3.20, -2.05)`
*   **Brass Lever Handle**: Cylinders, Stem Scale `(0.04, 0.09, 0.04)` at `(-14.86, 3.30, -2.05)` Rot `(0, 0, 90)`; Grip Scale `(0.035, 0.24, 0.035)` at `(-14.81, 3.30, -2.15)` Rot `(90, 0, 0)`

## Lighting Setup (Balanced Indoor Illumination)

The room features a carefully calibrated, high-efficiency lighting model designed for a large $30\text{m} \times 30\text{m} \times 15\text{m}$ athletic sports hall. It increases overall brightness and visibility across all corners and gaming setups while strictly protecting against overexposure, highlight burnout, and flat contrast.

*   **Primary Point Light Source (Central High-Bay Luminaire)**:
    *   **Position**: `(0.0, 14.55, -2.0)` situated just below the ceiling directly above the table tennis table.
    *   **Light Color**: `(1.00, 0.96, 0.88)` (clean, high-CRI daylight-balanced warm white ~3800K).
    *   **Attenuation Model**:
        $$Attenuation = \frac{1.0}{1.0 + 0.022 \cdot d + 0.0020 \cdot d^2}$$
        Provides smooth, physically plausible falloff: $0.64$ at table level ($12\text{m}$), $0.56$ at floor level ($15\text{m}$), and maintaining a clean $0.44$ across the far walls and corners ($20\text{m}-26\text{m}$).
*   **Dual-Hemisphere Ambient Fill (Phong Fragment Shader)**:
    *   **Sky Ambient Term**: `vec3(0.40, 0.42, 0.45) * lightColor` (simulates diffuse light bouncing off ceiling and entering from north window).
    *   **Ground Ambient Term**: `vec3(0.35, 0.32, 0.28) * lightColor` (simulates warm bounce light reflecting from the maple hardwood floor).
    *   **Normal Blend**: Modulated per-fragment by $\text{clamp}(N_y \cdot 0.5 + 0.5, 0.0, 1.0)$ to provide natural 3D curvature and depth even in indirect lighting.
*   **Highlight Protection & Color Clamping**:
    *   Shader clamps final composite colors `clamp(ambient + diffuse + specular + emission, 0.0, 1.0)` to preserve natural contrast and avoid washed-out clipping.
*   **Ceiling Fixture Components**:
    *   **Mounting Base**: Cylinder, Scale `(0.60, 0.05, 0.60)`, Translation `(0.0, 14.95, -2.0)`. Dark steel finish.
    *   **Lamp Housing**: Cylinder, Scale `(0.18, 0.20, 0.18)`, Translation `(0.0, 14.80, -2.0)`. Brushed aluminum.
    *   **Diffuser Bulb**: Sphere, Scale `(0.64, 0.64, 0.64)`, Translation `(0.0, 14.55, -2.0)`. Luminous frosted glass (`emission: lightColor * 1.5`).
    *   **Bulb Halo**: Sphere, Scale `(0.736, 0.736, 0.736)`, Translation `(0.0, 14.55, -2.0)`. Unlit shader glow pass.

---

## Furniture

### Table
**Tabletop**
*   **Primitive**: Cube
*   **Scaling**: `(4.0, 0.2, 3.0)`
*   **Translation**: `(8.0, 2.0, -5.0)`

**Legs (x4)**
*   **Primitive**: Cube
*   **Scaling**: `(0.2, 2.0, 0.2)`
*   **Translations**: 
    *   Leg 1: `(9.8, 1.0, -3.7)`
    *   Leg 2: `(9.8, 1.0, -6.3)`
    *   Leg 3: `(6.2, 1.0, -3.7)`
    *   Leg 4: `(6.2, 1.0, -6.3)`

### Chairs (for the table)
**Chair 1 (Front)**
*   **Seat**: Cube, Scale `(1.2, 0.2, 1.2)`, Trans `(8.0, 1.2, -2.5)`
*   **Top Rail**: Cube, Scale `(1.2, 0.2, 0.15)`, Trans `(8.0, 2.7, -1.95)`
*   **Left Stile**: Cube, Scale `(0.15, 1.5, 0.15)`, Trans `(8.525, 2.05, -1.95)`
*   **Right Stile**: Cube, Scale `(0.15, 1.5, 0.15)`, Trans `(7.475, 2.05, -1.95)`
*   **Slats (x3)**: Cubes, Scale `(0.1, 1.5, 0.1)`, Trans `(8.25, 2.05, -1.95)`, `(8.0, 2.05, -1.95)`, `(7.75, 2.05, -1.95)`
*   **Legs**: Cubes, Scale `(0.15, 1.2, 0.15)`, Trans `(8.525, 0.6, -1.975)`, `(7.475, 0.6, -1.975)`, `(8.525, 0.6, -3.025)`, `(7.475, 0.6, -3.025)`

**Chair 2 (Back)**
*   **Seat**: Cube, Scale `(1.2, 0.2, 1.2)`, Trans `(8.0, 1.2, -7.5)`
*   **Top Rail**: Cube, Scale `(1.2, 0.2, 0.15)`, Trans `(8.0, 2.7, -8.05)`
*   **Left Stile**: Cube, Scale `(0.15, 1.5, 0.15)`, Trans `(8.525, 2.05, -8.05)`
*   **Right Stile**: Cube, Scale `(0.15, 1.5, 0.15)`, Trans `(7.475, 2.05, -8.05)`
*   **Slats (x3)**: Cubes, Scale `(0.1, 1.5, 0.1)`, Trans `(8.25, 2.05, -8.05)`, `(8.0, 2.05, -8.05)`, `(7.75, 2.05, -8.05)`
*   **Legs**: Cubes, Scale `(0.15, 1.2, 0.15)`, Trans `(8.525, 0.6, -6.975)`, `(7.475, 0.6, -6.975)`, `(8.525, 0.6, -8.025)`, `(7.475, 0.6, -8.025)`

### Almirahs (x2)
**Almirah 1**
*   **Body**: Cube, Scale `(4.0, 8.0, 2.0)`, Trans `(-10.0, 4.0, -13.0)`
*   **Doors (x2)**: Cubes, Scale `(1.95, 7.8, 0.1)`, Trans `(-10.98, 4.0, -11.95)` and `(-9.02, 4.0, -11.95)`
*   **Handles (x2)**: Cubes, Scale `(0.1, 0.6, 0.1)`, Trans `(-10.2, 4.0, -11.85)` and `(-9.8, 4.0, -11.85)`

**Almirah 2**
*   **Body**: Cube, Scale `(4.0, 8.0, 2.0)`, Trans `(10.0, 4.0, -13.0)`
*   **Doors (x2)**: Cubes, Scale `(1.95, 7.8, 0.1)`, Trans `(9.02, 4.0, -11.95)` and `(10.98, 4.0, -11.95)`
*   **Handles (x2)**: Cubes, Scale `(0.1, 0.6, 0.1)`, Trans `(9.8, 4.0, -11.85)` and `(10.2, 4.0, -11.85)`

---

## Game Setups

### Carrom Boards (x2)
**Carrom Board 1**
*   **Board Base**: Cube, Scale `(3.0, 0.1, 3.0)`, Trans `(-8.0, 3.5, 5.0)`
*   **Left Border**: Cube, Scale `(0.2, 0.2, 3.0)`, Trans `(-9.4, 3.65, 5.0)`
*   **Right Border**: Cube, Scale `(0.2, 0.2, 3.0)`, Trans `(-6.6, 3.65, 5.0)`
*   **Top Border**: Cube, Scale `(2.6, 0.2, 0.2)`, Trans `(-8.0, 3.65, 3.6)`
*   **Bottom Border**: Cube, Scale `(2.6, 0.2, 0.2)`, Trans `(-8.0, 3.65, 6.4)`
*   **Pockets (x4)**: Cylinders, Scale `(0.3, 0.1, 0.3)`, Trans `(-9.2, 3.56, 3.8)`, `(-9.2, 3.56, 6.2)`, `(-6.8, 3.56, 3.8)`, `(-6.8, 3.56, 6.2)`
*   **Legs (x4)**: Cubes, Scale `(0.15, 3.5, 0.15)`, Trans `(-9.4, 1.75, 3.6)`, `(-9.4, 1.75, 6.4)`, `(-6.6, 1.75, 3.6)`, `(-6.6, 1.75, 6.4)`
*   **Pieces (Opening Break Circle)**:
    *   **Queen (Red)**: Cylinder, Scale `(0.16, 0.05, 0.16)`, Trans `(-8.000, 3.575, 5.000)`
    *   **Inner Ring (x6 alternating White/Black, r=0.17)**: Cylinders, Scale `(0.16, 0.05, 0.16)`, Trans `(-7.830, 3.575, 5.000)` (W), `(-7.915, 3.575, 5.147)` (B), `(-8.085, 3.575, 5.147)` (W), `(-8.170, 3.575, 5.000)` (B), `(-8.085, 3.575, 4.853)` (W), `(-7.915, 3.575, 4.853)` (B)
    *   **Outer Ring (x6 alternating Black/White, r=0.33)**: Cylinders, Scale `(0.16, 0.05, 0.16)`, Trans `(-7.714, 3.575, 5.165)` (B), `(-8.000, 3.575, 5.330)` (W), `(-8.286, 3.575, 5.165)` (B), `(-8.286, 3.575, 4.835)` (W), `(-8.000, 3.575, 4.670)` (B), `(-7.714, 3.575, 4.835)` (W)
    *   **Striker**: Cylinder, Scale `(0.22, 0.06, 0.22)`, Trans `(-8.000, 3.580, 6.000)`

**Carrom Board 2**
*   **Board Base**: Cube, Scale `(3.0, 0.1, 3.0)`, Trans `(8.0, 3.5, 5.0)`
*   **Left Border**: Cube, Scale `(0.2, 0.2, 3.0)`, Trans `(6.6, 3.65, 5.0)`
*   **Right Border**: Cube, Scale `(0.2, 0.2, 3.0)`, Trans `(9.4, 3.65, 5.0)`
*   **Top Border**: Cube, Scale `(2.6, 0.2, 0.2)`, Trans `(8.0, 3.65, 3.6)`
*   **Bottom Border**: Cube, Scale `(2.6, 0.2, 0.2)`, Trans `(8.0, 3.65, 6.4)`
*   **Pockets (x4)**: Cylinders, Scale `(0.3, 0.1, 0.3)`, Trans `(6.8, 3.56, 3.8)`, `(6.8, 3.56, 6.2)`, `(9.2, 3.56, 3.8)`, `(9.2, 3.56, 6.2)`
*   **Legs (x4)**: Cubes, Scale `(0.15, 3.5, 0.15)`, Trans `(6.6, 1.75, 3.6)`, `(6.6, 1.75, 6.4)`, `(9.4, 1.75, 3.6)`, `(9.4, 1.75, 6.4)`
*   **Pieces (Active Mid-Game Scatter)**:
    *   **Queen (Red)**: Cylinder, Scale `(0.16, 0.05, 0.16)`, Trans `(7.85, 3.575, 4.80)`
    *   **White Coins (x5)**: Cylinders, Scale `(0.16, 0.05, 0.16)`, Trans `(7.15, 3.575, 4.10)`, `(8.85, 3.575, 5.90)`, `(8.40, 3.575, 5.25)`, `(7.60, 3.575, 5.35)`, `(8.15, 3.575, 4.35)`
    *   **Black Coins (x5)**: Cylinders, Scale `(0.16, 0.05, 0.16)`, Trans `(8.85, 3.575, 4.15)`, `(7.20, 3.575, 5.85)`, `(7.95, 3.575, 4.50)`, `(8.45, 3.575, 4.75)`, `(7.70, 3.575, 5.05)`
    *   **Striker**: Cylinder, Scale `(0.22, 0.06, 0.22)`, Trans `(7.50, 3.580, 6.00)`

### Table Tennis Board
**Tabletop**
*   **Primitive**: Cube
*   **Scaling**: `(8.0, 0.2, 5.0)`
*   **Translation**: `(0.0, 3.0, 0.0)`
*   **Color / Texture**: ITTF Tournament Blue procedural surface with crisp white boundary and center lines, dark blue edge apron `(0.05, 0.15, 0.38)`

**Net**
*   **Primitive**: Cube
*   **Scaling**: `(0.1, 0.5, 5.0)`
*   **Translation**: `(0.0, 3.35, 0.0)`

**Legs (x4)**
*   **Primitive**: Cube
*   **Scaling**: `(0.2, 3.0, 0.2)`
*   **Translations**: `(3.8, 1.5, 2.3)`, `(-3.8, 1.5, 2.3)`, `(3.8, 1.5, -2.3)`, `(-3.8, 1.5, -2.3)`

### Table Tennis Equipment
**Paddle 1 (Red)**
*   **Blade**: Cylinder, Scale `(0.40, 0.04, 0.40)`, Trans `(2.0, 3.125, 0.0)`, Rotation `(0.0, 0.0, 0.0)`
*   **Handle**: Cylinder, Scale `(0.08, 0.32, 0.04)`, Trans `(2.0, 3.125, 0.31)`, Rotation `(90.0, 0.0, 0.0)`

**Paddle 2 (Blue)**
*   **Blade**: Cylinder, Scale `(0.40, 0.04, 0.40)`, Trans `(-2.0, 3.125, 0.0)`, Rotation `(0.0, 0.0, 0.0)`
*   **Handle**: Cylinder, Scale `(0.08, 0.32, 0.04)`, Trans `(-2.0, 3.125, -0.31)`, Rotation `(90.0, 0.0, 0.0)`

**TT Ball**
*   **Primitive**: Sphere
*   **Scaling**: `(0.10, 0.10, 0.10)`
*   **Translation**: `(0.5, 3.155, 0.5)`

### Chess Set (on Dining / Games Table)

**Chess Board**
*   **Plinth / Base Molding**: Cube, Scale `(1.64, 0.016, 1.64)`, Trans `(8.0, 2.133, -5.0)`
*   **Board Main Body**: Cube, Scale `(1.60, 0.034, 1.60)`, Trans `(8.0, 2.158, -5.0)`
*   **Surface Texture**: 1024x1024 procedural tournament surface with 8x8 alternating curly maple and dark walnut squares, brass/gold inlay stringing lines, and polished rosewood border.
*   **Playing Grid Dimensions**: 8x8 squares spanning $1.36\text{m} \times 1.36\text{m}$ (each square $0.170\text{m} \times 0.170\text{m}$), centered at `(8.0, 2.175, -5.0)`. Files a..h run along $X \in [7.405, 8.595]$, Ranks 1..8 run along $Z \in [-5.595, -4.405]$.

**Dual-Timer Analog Chess Clock**
*   **Clock Casing**: Cube, Scale `(0.15, 0.14, 0.28)`, Trans `(6.85, 2.195, -5.0)`
*   **Clock Dials (x2)**: Cylinders, Scale `(0.076, 0.004, 0.076)`, Trans `(6.930, 2.195, -5.06)` and `(6.930, 2.195, -4.94)`, Rotation `(0.0, 0.0, 90.0)`
*   **Dial Bezels (x2)**: Cylinders, Scale `(0.082, 0.005, 0.082)`, Trans `(6.927, 2.195, -5.06)` and `(6.927, 2.195, -4.94)`, Rotation `(0.0, 0.0, 90.0)`
*   **Rocker Buttons (x2)**: Cylinders, Scale `(0.032, 0.016, 0.032)` at `(6.85, 2.273, -5.06)` and Scale `(0.032, 0.036, 0.032)` at `(6.85, 2.283, -4.94)`

**Staunton Chess Pieces (Component Primitives)**
*   **Pawn (Height 0.10m)**:
    *   Base: Cylinder, Scale `(0.070, 0.016, 0.070)` at $+0.008$
    *   Body: Cylinder, Scale `(0.046, 0.046, 0.046)` at $+0.038$
    *   Collar: Cylinder, Scale `(0.050, 0.008, 0.050)` at $+0.063$
    *   Head: Sphere, Scale `(0.036, 0.036, 0.036)` at $+0.080$
*   **Rook (Height 0.12m)**:
    *   Base: Cylinder, Scale `(0.080, 0.020, 0.080)` at $+0.010$
    *   Shaft: Cylinder, Scale `(0.060, 0.070, 0.060)` at $+0.055$
    *   Collar: Cylinder, Scale `(0.068, 0.012, 0.068)` at $+0.095$
    *   Turret: Cylinder, Scale `(0.074, 0.022, 0.074)` at $+0.112$
*   **Knight (Height 0.13m)**:
    *   Base: Cylinder, Scale `(0.080, 0.020, 0.080)` at $+0.010$
    *   Pedestal: Cylinder, Scale `(0.058, 0.030, 0.058)` at $+0.035$
    *   Head: Cube, Scale `(0.046, 0.055, 0.065)` at $+0.075$
    *   Ears / Mane: Cube, Scale `(0.035, 0.025, 0.040)` at $+0.105$
*   **Bishop (Height 0.16m)**:
    *   Base: Cylinder, Scale `(0.080, 0.020, 0.080)` at $+0.010$
    *   Body: Cylinder, Scale `(0.052, 0.080, 0.052)` at $+0.060$
    *   Collar: Cylinder, Scale `(0.058, 0.010, 0.058)` at $+0.105$
    *   Miter Head: Sphere, Scale `(0.044, 0.052, 0.044)` at $+0.125$
    *   Finial: Sphere, Scale `(0.016, 0.016, 0.016)` at $+0.155$
*   **Queen (Height 0.17m)**:
    *   Base: Cylinder, Scale `(0.086, 0.024, 0.086)` at $+0.012$
    *   Body: Cylinder, Scale `(0.056, 0.095, 0.056)` at $+0.070$
    *   Collar: Cylinder, Scale `(0.064, 0.012, 0.064)` at $+0.122$
    *   Crown: Cylinder, Scale `(0.068, 0.024, 0.068)` at $+0.140$
    *   Finial Orb: Sphere, Scale `(0.022, 0.022, 0.022)` at $+0.160$
*   **King (Height 0.19m)**:
    *   Base: Cylinder, Scale `(0.088, 0.024, 0.088)` at $+0.012$
    *   Body: Cylinder, Scale `(0.060, 0.102, 0.060)` at $+0.075$
    *   Collar: Cylinder, Scale `(0.066, 0.012, 0.066)` at $+0.130$
    *   Crown: Cylinder, Scale `(0.070, 0.028, 0.070)` at $+0.150$
    *   Finial Cross Vertical: Cube, Scale `(0.012, 0.024, 0.010)` at $+0.174$
    *   Finial Cross Arm: Cube, Scale `(0.024, 0.008, 0.010)` at $+0.178$

**Piece Placements (Exploration Display Setup, Board Surface $Y_0 = 2.175$)**
*   **White Pieces (South / Rank 1-4)**:
    *   Rooks (x2): `(7.405, 2.175, -5.595)` [a1], `(8.255, 2.175, -5.595)` [f1]
    *   Knights (x2): `(7.575, 2.175, -5.595)` [b1], `(8.255, 2.175, -5.255)` [f3]
    *   Bishops (x2): `(7.745, 2.175, -5.595)` [c1], `(7.745, 2.175, -5.085)` [c4]
    *   Queen: `(7.915, 2.175, -5.595)` [d1]
    *   King: `(8.425, 2.175, -5.595)` [g1]
    *   Pawns (x8): `(7.405, 2.175, -5.425)` [a2], `(7.575, 2.175, -5.425)` [b2], `(7.745, 2.175, -5.255)` [c3], `(7.915, 2.175, -5.255)` [d3], `(8.085, 2.175, -5.085)` [e4], `(8.255, 2.175, -5.425)` [f2], `(8.425, 2.175, -5.425)` [g2], `(8.595, 2.175, -5.425)` [h2]
*   **Black Pieces (North / Rank 5-8)**:
    *   Rooks (x2): `(7.405, 2.175, -4.405)` [a8], `(8.255, 2.175, -4.405)` [f8]
    *   Knights (x2): `(7.745, 2.175, -4.745)` [c6], `(8.255, 2.175, -4.745)` [f6]
    *   Bishops (x2): `(7.745, 2.175, -4.405)` [c8], `(7.745, 2.175, -4.915)` [c5]
    *   Queen: `(7.915, 2.175, -4.405)` [d8]
    *   King: `(8.425, 2.175, -4.405)` [g8]
    *   Pawns (x8): `(7.405, 2.175, -4.575)` [a7], `(7.575, 2.175, -4.575)` [b7], `(7.745, 2.175, -4.575)` [c7], `(7.915, 2.175, -4.745)` [d6], `(8.085, 2.175, -4.915)` [e5], `(8.255, 2.175, -4.575)` [f7], `(8.425, 2.175, -4.575)` [g7], `(8.595, 2.175, -4.575)` [h7]

**Playable Chess Mini-Game (Player vs AI)**
*   **Interaction Trigger**: In Room Exploration mode, approaching the games table (within $3.2\text{m}$) activates the HUD prompt (`CHESS GAME: Press 1 to Play`).
*   **Sides**: User plays as **White** (South, Rank 1); opponent is a **Simple AI** playing as **Black** (North, Rank 8).
*   **Gameplay Camera**: Eye position `(8.00, 3.42, -6.65)`, target `(8.00, 2.175, -5.00)`, downward angle $35^\circ$ overlooking the board from the player's chair. Mouse cursor is unlocked into OS pointer mode.
*   **AI Engine**: Minimax search with Alpha-Beta pruning (2-ply depth), material valuation (P:100, N:320, B:330, R:500, Q:900, K:20000), PeSTO Piece-Square Tables for positional development and center control, and natural thinking delay ($0.50\text{s}$).
*   **Dynamic Visual Elements**:
    *   *Hover Square*: Translucent light-blue outline pad `(0.162, 0.002, 0.162)`.
    *   *Selected Square*: Glowing golden pad `(0.162, 0.003, 0.162)` at $+0.003\text{m}$ above board.
    *   *Legal Move Targets*: Emerald green cylindrical dots `(0.065, 0.004, 0.065)` for open squares; crimson capture pads `(0.145, 0.004, 0.145)` for enemy piece captures.
    *   *Last Move Markers*: Warm amber pads at both origin and destination squares.
    *   *Check Indicator*: Vivid red glowing pad beneath checked King.
    *   *Smooth Move Animation*: In-flight piece arced trajectory ($Y(t) = 2.175 + \sin(\pi t) \times 0.09\text{m}$) with smoothstep ease across $0.28\text{s}$.
    *   *Captured Pieces Display*: Physically lined up in 3D along table flanks (White captures on East flank $X \approx 8.95..9.13$, Black captures on West flank $X \approx 6.87..7.05$).
*   **Controls**:
    *   `Left Click`: Select piece / Move to legal square.
    *   `Right Click`: Deselect piece.
    *   `R`: Restart new game.
    *   `E` / `Escape`: Exit mini-game back to Room Exploration.

### Functional Round Wall Clock (North / Back Wall)

Mounted centrally on the Back / North Wall above the central window at $X = 0.0\text{m}, Y = 10.8\text{m}, Z = -15.0\text{m}$. Scaled to an architectural $2.50\text{m}$ diameter with high-contrast numerals and vivid hands so that it is clearly readable across the entire 28m hall from any camera position.

*   **Outer Bezel / Casing**: Cylinder, Scale `(2.50, 0.18, 2.50)`, Translation `(0.0, 10.8, -14.90)`, Rotation `(90.0, 0.0, 0.0)`. Walnut wood grain texture with bronze sheen (`shininess: 32.0, specular: 0.35`).
*   **Inner Stepped Bezel**: Cylinder, Scale `(2.36, 0.04, 2.36)`, Translation `(0.0, 10.8, -14.83)`, Rotation `(90.0, 0.0, 0.0)`. Polished warm brass (`shininess: 64.0, specular: 0.70`).
*   **Clock Dial Face**: Cylinder, Scale `(2.24, 0.02, 2.24)`, Translation `(0.0, 10.8, -14.81)`, Rotation `(90.0, 0.0, 0.0)`. Front dial plane lies at $Z = -14.80\text{m}$.
    *   **Dial Surface Texture**: 1024x1024 procedural antique porcelain dial featuring vector strokes for hour numerals 1 through 12, double outer railway track rings, 60-minute division tick marks, 12 bold hour batons, inner chapter ring ($r = 0.240$), and central brass accent ring ($r = 0.035$).
*   **Real-Time Kinematics Engine**:
    *   Queried from the system local wall-clock time at launch (`localtime_s` / `localtime_r`), advanced monotonically per-frame via high-resolution `glfwGetTime()`.
    *   All three hands sweep continuously without jitter or occlusion.
*   **Hour Hand (Black)**:
    *   **Primitive**: Cube
    *   **Dimensions**: Scale `(0.090, 0.72, 0.024)` (9cm wide, 0.58m forward reach, 0.14m counterbalance tail).
    *   **Transformation**: Position `(0.0 + 0.22 * sin(radHour), 10.8 + 0.22 * cos(radHour), -14.784)`, Rotation `(0.0, 0.0, -degrees(radHour))`.
*   **Minute Hand (Black)**:
    *   **Primitive**: Cube
    *   **Dimensions**: Scale `(0.068, 1.12, 0.024)` (6.8cm wide, 0.94m forward reach, 0.18m counterbalance tail).
    *   **Transformation**: Position `(0.0 + 0.38 * sin(radMin), 10.8 + 0.38 * cos(radMin), -14.762)`, Rotation `(0.0, 0.0, -degrees(radMin))`.
*   **Second Hand (Scarlet Red & Lollipop Signal Tip)**:
    *   **Hand Shaft**: Cube, Scale `(0.028, 1.26, 0.018)` (2.8cm wide, 1.02m reach, 0.24m tail). Position `(0.0 + 0.39 * sin(radSec), 10.8 + 0.39 * cos(radSec), -14.742)`, Rotation `(0.0, 0.0, -degrees(radSec))`.
    *   **Tail Counterweight Disc**: Cylinder, Scale `(0.13, 0.020, 0.13)` (diameter 0.13m), Position `(0.0 - 0.18 * sin(radSec), 10.8 - 0.18 * cos(radSec), -14.741)`, Rotation `(90.0, 0.0, 0.0)`.
    *   **Tip Signal Lollipop Disc**: Cylinder, Scale `(0.12, 0.020, 0.12)` (diameter 0.12m), Position `(0.0 + 0.86 * sin(radSec), 10.8 + 0.86 * cos(radSec), -14.741)`, Rotation `(90.0, 0.0, 0.0)`.
*   **Center Hub & Cap**:
    *   **Base Hub Disc**: Cylinder, Scale `(0.16, 0.024, 0.16)`, Translation `(0.0, 10.8, -14.728)`, Rotation `(90.0, 0.0, 0.0)`. Brass material.
    *   **Domed Center Cap**: Sphere, Scale `(0.10, 0.10, 0.10)`, Translation `(0.0, 10.8, -14.716)`. Brass material.


