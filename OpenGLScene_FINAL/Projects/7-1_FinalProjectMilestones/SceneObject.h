///////////////////////////////////////////////////////////////////////////////
// SceneObject.h
// ============
// manage the preparing and rendering of complex objects in 3D scenes
//
//  AUTHOR: Jeremy Brown
///////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <string>

/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/

 // Enum to define the type of primitive shape to render
 // This allows a single render function to handle multiple object types
enum class ShapeType {
    PLANE,
    BOX,
    SPHERE,
    CYLINDER,
    CONE
};


// Structure representing a single object in the scene
struct SceneObject {

    // ===== Object Identity =====

    // Type of geometric shape (cube, sphere, cylinder, etc.)
    ShapeType shape;


    // ===== Transformation Properties =====

    // Position of the object in 3D space
    float posX;
    float posY;
    float posZ;

    // Rotation of the object (in degrees) around each axis
    float rotX;
    float rotY;
    float rotZ;

    // Scale of the object along each axis
    float scaleX;
    float scaleY;
    float scaleZ;


    // ===== Rendering Properties =====

    // Name of the texture to apply (can map to texture manager)
    std::string textureName;

    // Material or shader identifier (optional, depending on your system)
    std::string materialName;


    // ===== Constructor =====
    // Default constructor initializes a basic object
    SceneObject()
        : shape(ShapeType::BOX),
        posX(0.0f), posY(0.0f), posZ(0.0f),
        rotX(0.0f), rotY(0.0f), rotZ(0.0f),
        scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f),
        textureName(""),
        materialName("")
    {
    }

    // Parameterized constructor for quick object creation
    SceneObject(
        ShapeType shapeType,
        float x, float y, float z,
        float rX, float rY, float rZ,
        float sX, float sY, float sZ,
        std::string texture,
        std::string material
    )
        : shape(shapeType),
        posX(x), posY(y), posZ(z),
        rotX(rX), rotY(rY), rotZ(rZ),
        scaleX(sX), scaleY(sY), scaleZ(sZ),
        textureName(texture),
        materialName(material)
    {
    }
};


#endif // SCENEOBJECT_H