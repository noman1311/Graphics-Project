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

### Light Source
*   **Primitive**: Sphere
*   **Scaling**: `(1.0, 1.0, 1.0)`
*   **Translation**: `(0.0, 14.0, 0.0)`

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
*   **White Coin**: Cylinder, Scale `(0.15, 0.05, 0.15)`, Trans `(-8.0, 3.575, 5.0)`
*   **Black Coin**: Cylinder, Scale `(0.15, 0.05, 0.15)`, Trans `(-7.8, 3.575, 4.8)`
*   **Red Striker**: Cylinder, Scale `(0.2, 0.06, 0.2)`, Trans `(-8.5, 3.58, 5.8)`

**Carrom Board 2**
*   **Board Base**: Cube, Scale `(3.0, 0.1, 3.0)`, Trans `(8.0, 3.5, 5.0)`
*   **Left Border**: Cube, Scale `(0.2, 0.2, 3.0)`, Trans `(6.6, 3.65, 5.0)`
*   **Right Border**: Cube, Scale `(0.2, 0.2, 3.0)`, Trans `(9.4, 3.65, 5.0)`
*   **Top Border**: Cube, Scale `(2.6, 0.2, 0.2)`, Trans `(8.0, 3.65, 3.6)`
*   **Bottom Border**: Cube, Scale `(2.6, 0.2, 0.2)`, Trans `(8.0, 3.65, 6.4)`
*   **Pockets (x4)**: Cylinders, Scale `(0.3, 0.1, 0.3)`, Trans `(6.8, 3.56, 3.8)`, `(6.8, 3.56, 6.2)`, `(9.2, 3.56, 3.8)`, `(9.2, 3.56, 6.2)`
*   **Legs (x4)**: Cubes, Scale `(0.15, 3.5, 0.15)`, Trans `(6.6, 1.75, 3.6)`, `(6.6, 1.75, 6.4)`, `(9.4, 1.75, 3.6)`, `(9.4, 1.75, 6.4)`
*   **White Coin**: Cylinder, Scale `(0.15, 0.05, 0.15)`, Trans `(8.0, 3.575, 5.0)`
*   **Black Coin**: Cylinder, Scale `(0.15, 0.05, 0.15)`, Trans `(8.2, 3.575, 4.8)`
*   **Red Striker**: Cylinder, Scale `(0.2, 0.06, 0.2)`, Trans `(7.5, 3.58, 5.8)`

### Table Tennis Board
**Tabletop**
*   **Primitive**: Cube
*   **Scaling**: `(8.0, 0.2, 5.0)`
*   **Translation**: `(0.0, 3.0, 0.0)`

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
*   **Blade**: Cylinder, Scale `(0.4, 0.05, 0.4)`, Trans `(2.0, 3.5, 0.0)`, Rotation `(90.0, 0.0, 0.0)`
*   **Handle**: Cylinder, Scale `(0.08, 0.4, 0.08)`, Trans `(2.0, 3.1, 0.0)`

**Paddle 2 (Blue)**
*   **Blade**: Cylinder, Scale `(0.4, 0.05, 0.4)`, Trans `(-2.0, 3.5, 0.0)`, Rotation `(90.0, 0.0, 0.0)`
*   **Handle**: Cylinder, Scale `(0.08, 0.4, 0.08)`, Trans `(-2.0, 3.1, 0.0)`

**TT Ball**
*   **Primitive**: Sphere
*   **Scaling**: `(0.1, 0.1, 0.1)`
*   **Translation**: `(0.5, 3.15, 0.5)`
