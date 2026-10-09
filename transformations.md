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
