///////////////////////////////////////////////////////////////////////////////
// scenemanager.h
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>


/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
	// constructor
	SceneManager(ShaderManager* pShaderManager);
	// destructor
	~SceneManager();

	struct TEXTURE_INFO
	{
		std::string tag;
		uint32_t ID;
	};

	struct OBJECT_MATERIAL
	{
		glm::vec3 ambientColor;
		float ambientStrength;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		float shininess;
		std::string tag;
	};

	enum class ShapeType
	{
		PLANE,
		CYLINDER,
		CONE,
		BOX,
		TAPERED_CYLINDER,
		SPHERE
	};

	struct SceneObject
	{
		ShapeType shapeType;

		glm::vec3 scale;
		float rotX;
		float rotY;
		float rotZ;
		glm::vec3 position;

		std::string materialTag;
		std::string textureTag;
		bool useTexture;
	};

	struct CompositeObject
	{
		std::string name;
		std::vector<SceneObject> parts;
	};

private:
	// pointer to shader manager object
	ShaderManager* m_pShaderManager;
	// pointer to basic shapes object
	ShapeMeshes* m_basicMeshes;
	// total number of loaded textures
	int m_loadedTextures;
	// loaded textures info
	TEXTURE_INFO m_textureIDs[16];
	// defined object materials
	std::vector<OBJECT_MATERIAL> m_objectMaterials;

	// collection of individual scene objects
	std::vector<SceneObject> m_sceneObjects;
	// collection of grouped multi-part scene objects
	std::vector<CompositeObject> m_compositeObjects;

	// load texture images and convert to OpenGL texture data
	bool CreateGLTexture(const char* filename, std::string tag);
	// bind loaded OpenGL textures to slots in memory
	void BindGLTextures();
	// free the loaded OpenGL textures
	void DestroyGLTextures();
	// find a loaded texture by tag
	int FindTextureID(std::string tag);
	int FindTextureSlot(std::string tag);
	// find a defined material by tag
	bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);

	// create and store all scene objects
	void InitializeSceneObjects();
	// render one scene object based on its stored properties
	void RenderSceneObject(const SceneObject& obj);

	// create and store grouped scene objects
	void InitializeCompositeObjects();
	// render all parts of a composite object
	void RenderCompositeObject(const CompositeObject& compositeObj);

	// set the transformation values
	// into the transform buffer
	void SetTransformations(
		glm::vec3 scaleXYZ,
		float XrotationDegrees,
		float YrotationDegrees,
		float ZrotationDegrees,
		glm::vec3 positionXYZ);

	// set the color values into the shader
	void SetShaderColor(
		float redColorValue,
		float greenColorValue,
		float blueColorValue,
		float alphaValue);

	// set the texture data into the shader
	void SetShaderTexture(
		std::string textureTag);

	// set the UV scale for the texture mapping
	void SetTextureUVScale(
		float u, float v);

	// set the object material into the shader
	void SetShaderMaterial(
		std::string materialTag);

public:

	// The following methods are for the students to
	// customize for their own 3D scene
	void PrepareScene();
	void RenderScene();

	// loads textures from image files
	void LoadSceneTextures();

	// pre-set light sources for 3D scene
	void SetupSceneLights();
	// pre-define the object materials for lighting
	void DefineObjectMaterials();

	void RenderCylinder(
		const glm::vec3& scale,
		float rotX, float rotY, float rotZ,
		const glm::vec3& position,
		const std::string& materialTag,
		const std::string& textureTag = "",
		bool useTexture = false);

	void RenderCone(
		const glm::vec3& scale,
		float rotX, float rotY, float rotZ,
		const glm::vec3& position,
		const std::string& materialTag,
		const std::string& textureTag = "",
		bool useTexture = false);

	void RenderBox(
		const glm::vec3& scale,
		float rotX, float rotY, float rotZ,
		const glm::vec3& position,
		const std::string& materialTag,
		const std::string& textureTag = "",
		bool useTexture = false);

	void RenderPlane(
		const glm::vec3& scale,
		float rotX, float rotY, float rotZ,
		const glm::vec3& position,
		const std::string& materialTag,
		const std::string& textureTag = "",
		bool useTexture = false);

	void RenderTaperedCylinder(
		const glm::vec3& scale,
		float rotX, float rotY, float rotZ,
		const glm::vec3& position,
		const std::string& materialTag,
		const std::string& textureTag = "",
		bool useTexture = false);

	void RenderSphere(
		const glm::vec3& scale,
		float rotX, float rotY, float rotZ,
		const glm::vec3& position,
		const std::string& materialTag,
		const std::string& textureTag = "",
		bool useTexture = false);
};