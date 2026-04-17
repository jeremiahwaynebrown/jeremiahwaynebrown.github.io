# Artifact 1 – 3D OpenGL Scene

## Overview
This project is a 3D scene built in C++ using OpenGL. It was originally developed in CS-330: Computational Graphics and Visualization and demonstrates rendering techniques, lighting, textures, and object transformations.

For the capstone, this artifact was enhanced to improve software design, modularity, and scalability.

---

## Original Functionality
The original project rendered a static 3D scene composed of multiple objects including:
- A spice rack
- Masking tape
- A pen
- Chapstick
- A solo cup
- A book

All objects were rendered directly within the `RenderScene()` function using individual draw calls.

---

## Enhancements

### 1. SceneObject Abstraction
A `SceneObject` structure was introduced to represent each object with:
- Shape type
- Scale
- Rotation
- Position
- Material
- Texture

This removed hardcoded rendering logic and improved code organization.

---

### 2. Composite Object System
A `CompositeObject` structure was added to group multiple objects into a single logical entity.

Example:
- The spice rack is now composed of multiple cylinders and cones grouped together.

This demonstrates hierarchical design and improves scalability.

---

### 3. Refactored SceneManager
The `SceneManager` was refactored to:
- Initialize objects dynamically
- Render objects through reusable functions
- Separate data from rendering logic

---

## Skills Demonstrated
- Object-Oriented Programming (OOP)
- Software architecture and modular design
- Graphics programming with OpenGL
- Code refactoring and maintainability

---

## How to Run
1. Open the project in Visual Studio
2. Build the solution
3. Run using Local Windows Debugger

Note: Ensure OpenGL dependencies (GLEW, GLFW, etc.) are properly configured.

---

## Screenshots
![OpenGL Scene](Screenshots/opengl-scene.png)

---

## Author
Jeremy Brown  
Southern New Hampshire University  
CS-499 Capstone
