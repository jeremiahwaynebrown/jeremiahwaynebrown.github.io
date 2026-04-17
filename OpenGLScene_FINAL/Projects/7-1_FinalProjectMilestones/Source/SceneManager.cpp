///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();

	// initialize the texture collection
	for (int i = 0; i < 16; i++)
	{
		m_textureIDs[i].tag = "/0";
		m_textureIDs[i].ID = -1;
	}
	m_loadedTextures = 0;
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
	// destroy the created OpenGL textures
	DestroyGLTextures();
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename
			<< ", width:" << width
			<< ", height:" << height
			<< ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glDeleteTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < static_cast<int>(m_objectMaterials.size())) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
			material.tag = m_objectMaterials[index].tag;
		}
		else
		{
			index++;
		}
	}

	return(bFound);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
  *  LoadSceneTextures()
  *
  *  This method is used for preparing the 3D scene by loading
  *  the shapes, textures in memory to support the 3D scene
  *  rendering
  ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;
	bReturn = CreateGLTexture(
		"../../Utilities/textures/knife_handle.jpg",
		"cone");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/seamless-wood3.jpg",
		"cylinder");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/road.jpg",
		"plane");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/blueTape.jpg",
		"tape");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/cardboard.jpg",
		"cardboard");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/drywall.jpg",
		"chapstick");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/pen.jpg",
		"pen");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/stainless.jpg",
		"solo");
	bReturn = CreateGLTexture(
		"../../Utilities/textures/napkinfinance.jpg",
		"book");

	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

void SceneManager::DefineObjectMaterials()
{
	// Wood (for the rack tiers and top)
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientColor = glm::vec3(0.3f, 0.2f, 0.1f);
	woodMaterial.ambientStrength = 0.25f;
	woodMaterial.diffuseColor = glm::vec3(0.4f, 0.25f, 0.15f);
	woodMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.2f);
	woodMaterial.shininess = 8.0f;
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);

	// Cement for the floor
	OBJECT_MATERIAL cementMaterial;
	cementMaterial.ambientColor = glm::vec3(0.4f, 0.4f, 0.4f);
	cementMaterial.ambientStrength = 0.4f;
	cementMaterial.diffuseColor = glm::vec3(0.6f, 0.6f, 0.6f);
	cementMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.2f);
	cementMaterial.shininess = 4.0f;
	cementMaterial.tag = "cement";
	m_objectMaterials.push_back(cementMaterial);

	OBJECT_MATERIAL blueTape;
	blueTape.ambientColor = glm::vec3(0.1f, 0.2f, 0.5f);
	blueTape.ambientStrength = 0.3f;
	blueTape.diffuseColor = glm::vec3(0.1f, 0.3f, 0.9f);
	blueTape.specularColor = glm::vec3(0.2f, 0.4f, 1.0f);
	blueTape.shininess = 16.0f;
	blueTape.tag = "blue_tape";
	m_objectMaterials.push_back(blueTape);

	OBJECT_MATERIAL cardboard;
	cardboard.ambientColor = glm::vec3(0.25f, 0.2f, 0.15f);
	cardboard.ambientStrength = 0.2f;
	cardboard.diffuseColor = glm::vec3(0.45f, 0.35f, 0.25f);
	cardboard.specularColor = glm::vec3(0.05f, 0.05f, 0.05f);
	cardboard.shininess = 4.0f;
	cardboard.tag = "cardboard";
	m_objectMaterials.push_back(cardboard);

	OBJECT_MATERIAL chapstick;
	chapstick.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
	chapstick.ambientStrength = 0.3f;
	chapstick.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	chapstick.specularColor = glm::vec3(0.6f, 0.6f, 0.6f);
	chapstick.shininess = 32.0f;
	chapstick.tag = "chapstick";
	m_objectMaterials.push_back(chapstick);

	OBJECT_MATERIAL penBody;
	penBody.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	penBody.ambientStrength = 0.3f;
	penBody.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);
	penBody.specularColor = glm::vec3(0.4f, 0.4f, 0.4f);
	penBody.shininess = 12.0f;
	penBody.tag = "pen";
	m_objectMaterials.push_back(penBody);

	OBJECT_MATERIAL cupMaterial;
	cupMaterial.ambientColor = glm::vec3(0.8f, 0.0f, 0.1f);
	cupMaterial.ambientStrength = 0.25f;
	cupMaterial.diffuseColor = glm::vec3(0.75f, 0.0f, 0.04f);
	cupMaterial.specularColor = glm::vec3(0.3f, 0.2f, 0.2f);
	cupMaterial.shininess = 8.0f;
	cupMaterial.tag = "solo";
	m_objectMaterials.push_back(cupMaterial);

	OBJECT_MATERIAL bookMaterial;
	bookMaterial.ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	bookMaterial.ambientStrength = 0.4f;
	bookMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	bookMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	bookMaterial.shininess = 8.0f;
	bookMaterial.tag = "book";
	m_objectMaterials.push_back(bookMaterial);
}

void SceneManager::SetupSceneLights()
{
	// Tell shader to use lighting system
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// Directional Light (soft fill light from above-left)
	m_pShaderManager->setVec3Value("directionalLight.direction", -0.3f, -1.0f, -0.2f);
	m_pShaderManager->setVec3Value("directionalLight.ambient", 0.3f, 0.2f, 0.2f);
	m_pShaderManager->setVec3Value("directionalLight.diffuse", 1.0f, 0.9f, 0.9f);
	m_pShaderManager->setVec3Value("directionalLight.specular", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setBoolValue("directionalLight.bActive", true);

	// Point Light 0 (front of structure — reduced to avoid washing out the ground)
	m_pShaderManager->setVec3Value("pointLights[0].position", 2.0f, 6.0f, 6.0f);
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.03f, 0.025f, 0.025f);
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 0.7f, 0.5f, 0.5f);
	m_pShaderManager->setVec3Value("pointLights[0].specular", 0.6f, 0.4f, 0.4f);
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true);

	// Point Light 1 (back-right fill light)
	m_pShaderManager->setVec3Value("pointLights[1].position", -3.0f, 6.0f, -2.0f);
	m_pShaderManager->setVec3Value("pointLights[1].ambient", 0.02f, 0.02f, 0.03f);
	m_pShaderManager->setVec3Value("pointLights[1].diffuse", 0.5f, 0.5f, 0.6f);
	m_pShaderManager->setVec3Value("pointLights[1].specular", 0.4f, 0.4f, 0.5f);
	m_pShaderManager->setBoolValue("pointLights[1].bActive", true);

	// Point Light 2 (above top tier highlight)
	m_pShaderManager->setVec3Value("pointLights[2].position", -5.0f, 12.0f, -3.0f);
	m_pShaderManager->setVec3Value("pointLights[2].ambient", 0.03f, 0.025f, 0.025f);
	m_pShaderManager->setVec3Value("pointLights[2].diffuse", 0.8f, 0.7f, 0.7f);
	m_pShaderManager->setVec3Value("pointLights[2].specular", 1.2f, 1.0f, 1.0f);
	m_pShaderManager->setBoolValue("pointLights[2].bActive", true);
}

void SceneManager::RenderCylinder(
	const glm::vec3& scale,
	float rotX, float rotY, float rotZ,
	const glm::vec3& position,
	const std::string& materialTag,
	const std::string& textureTag,
	bool useTexture)
{
	SetTransformations(scale, rotX, rotY, rotZ, position);
	SetShaderMaterial(materialTag);

	if (useTexture)
	{
		SetShaderTexture(textureTag);
	}
	else if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
	}

	m_basicMeshes->DrawCylinderMesh();
}

void SceneManager::RenderCone(
	const glm::vec3& scale,
	float rotX, float rotY, float rotZ,
	const glm::vec3& position,
	const std::string& materialTag,
	const std::string& textureTag,
	bool useTexture)
{
	SetTransformations(scale, rotX, rotY, rotZ, position);
	SetShaderMaterial(materialTag);

	if (useTexture)
	{
		SetShaderTexture(textureTag);
	}
	else if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
	}

	m_basicMeshes->DrawConeMesh();
}

void SceneManager::RenderBox(
	const glm::vec3& scale,
	float rotX, float rotY, float rotZ,
	const glm::vec3& position,
	const std::string& materialTag,
	const std::string& textureTag,
	bool useTexture)
{
	SetTransformations(scale, rotX, rotY, rotZ, position);
	SetShaderMaterial(materialTag);

	if (useTexture)
	{
		SetShaderTexture(textureTag);
	}
	else if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
	}

	m_basicMeshes->DrawBoxMesh();
}

void SceneManager::RenderPlane(
	const glm::vec3& scale,
	float rotX, float rotY, float rotZ,
	const glm::vec3& position,
	const std::string& materialTag,
	const std::string& textureTag,
	bool useTexture)
{
	SetTransformations(scale, rotX, rotY, rotZ, position);
	SetShaderMaterial(materialTag);

	if (useTexture)
	{
		SetShaderTexture(textureTag);
	}
	else if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
	}

	m_basicMeshes->DrawPlaneMesh();
}

void SceneManager::RenderTaperedCylinder(
	const glm::vec3& scale,
	float rotX, float rotY, float rotZ,
	const glm::vec3& position,
	const std::string& materialTag,
	const std::string& textureTag,
	bool useTexture)
{
	SetTransformations(scale, rotX, rotY, rotZ, position);
	SetShaderMaterial(materialTag);

	if (useTexture)
	{
		SetShaderTexture(textureTag);
	}
	else if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
	}

	m_basicMeshes->DrawTaperedCylinderMesh();
}

void SceneManager::RenderSphere(
	const glm::vec3& scale,
	float rotX, float rotY, float rotZ,
	const glm::vec3& position,
	const std::string& materialTag,
	const std::string& textureTag,
	bool useTexture)
{
	SetTransformations(scale, rotX, rotY, rotZ, position);
	SetShaderMaterial(materialTag);

	if (useTexture)
	{
		SetShaderTexture(textureTag);
	}
	else if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
	}

	m_basicMeshes->DrawSphereMesh();
}

void SceneManager::InitializeSceneObjects()
{
	// clear scene in case this is called more than once
	m_sceneObjects.clear();

	// Ground
	m_sceneObjects.push_back({
		ShapeType::PLANE,
		{ 20.0f, 1.0f, 10.0f },
		0.0f, 0.0f, 0.0f,
		{ 0.0f, 0.0f, 0.0f },
		"cement",
		"plane",
		true
		});

	// Masking tape + inner liner
	m_sceneObjects.push_back({
		ShapeType::CYLINDER,
		{ 1.0f, 1.0f, 1.0f },
		0.0f, 0.0f, 0.0f,
		{ 1.1f, 0.0f, 1.5f },
		"blue_tape",
		"tape",
		true
		});

	m_sceneObjects.push_back({
		ShapeType::CYLINDER,
		{ 0.8f, 1.02f, 0.8f },
		0.0f, 0.0f, 0.0f,
		{ 1.1f, 0.0f, 1.5f },
		"cardboard",
		"cardboard",
		true
		});

	// Chapstick
	m_sceneObjects.push_back({
		ShapeType::CYLINDER,
		{ 0.25f, 1.5f, 0.20f },
		90.0f, 110.0f, 0.0f,
		{ 0.0f, 0.20f, 3.5f },
		"chapstick",
		"chapstick",
		true
		});

	// Pen body
	m_sceneObjects.push_back({
		ShapeType::CYLINDER,
		{ 0.15f, 2.5f, 0.15f },
		0.0f, 0.0f, 90.0f,
		{ -5.0f, 0.15f, 3.0f },
		"pen",
		"pen",
		true
		});

	// Pen tip
	m_sceneObjects.push_back({
		ShapeType::CONE,
		{ 0.15f, 0.4f, 0.15f },
		0.0f, 0.0f, 270.0f,
		{ -5.0f, 0.15f, 3.0f },
		"pen",
		"pen",
		true
		});

	// Pen clicker
	m_sceneObjects.push_back({
		ShapeType::SPHERE,
		{ 0.1f, 0.3f, 0.1f },
		0.0f, 0.0f, 90.0f,
		{ -7.5f, 0.15f, 3.0f },
		"pen",
		"pen",
		true
		});

	// Solo cup
	m_sceneObjects.push_back({
		ShapeType::TAPERED_CYLINDER,
		{ 1.4f, 3.0f, 1.4f },
		0.0f, 0.0f, 0.0f,
		{ 2.4f, 0.0f, -2.0f },
		"solo",
		"solo",
		true
		});

	// Book
	m_sceneObjects.push_back({
		ShapeType::BOX,
		{ 6.0f, 6.0f, 0.5f },
		0.0f, -25.0f, 0.0f,
		{ 4.0f, 3.0f, -3.4f },
		"book",
		"book",
		true
		});
}

void SceneManager::RenderSceneObject(const SceneObject& obj)
{
	switch (obj.shapeType)
	{
	case ShapeType::PLANE:
		RenderPlane(obj.scale, obj.rotX, obj.rotY, obj.rotZ,
			obj.position, obj.materialTag, obj.textureTag, obj.useTexture);
		break;

	case ShapeType::CYLINDER:
		RenderCylinder(obj.scale, obj.rotX, obj.rotY, obj.rotZ,
			obj.position, obj.materialTag, obj.textureTag, obj.useTexture);
		break;

	case ShapeType::CONE:
		RenderCone(obj.scale, obj.rotX, obj.rotY, obj.rotZ,
			obj.position, obj.materialTag, obj.textureTag, obj.useTexture);
		break;

	case ShapeType::BOX:
		RenderBox(obj.scale, obj.rotX, obj.rotY, obj.rotZ,
			obj.position, obj.materialTag, obj.textureTag, obj.useTexture);
		break;

	case ShapeType::TAPERED_CYLINDER:
		RenderTaperedCylinder(obj.scale, obj.rotX, obj.rotY, obj.rotZ,
			obj.position, obj.materialTag, obj.textureTag, obj.useTexture);
		break;

	case ShapeType::SPHERE:
		RenderSphere(obj.scale, obj.rotX, obj.rotY, obj.rotZ,
			obj.position, obj.materialTag, obj.textureTag, obj.useTexture);
		break;
	}
}

void SceneManager::InitializeCompositeObjects()
{
	// clear grouped objects in case this is called more than once
	m_compositeObjects.clear();

	CompositeObject spiceRack;
	spiceRack.name = "Spice Rack";

	// Bottom tier
	spiceRack.parts.push_back({
		ShapeType::CYLINDER,
		{ 5.0f, 2.0f, 5.0f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 0.0f, -3.0f },
		"wood",
		"cylinder",
		true
		});

	spiceRack.parts.push_back({
		ShapeType::CONE,
		{ 1.0f, 4.0f, 1.0f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 0.0f, -3.0f },
		"wood",
		"cone",
		true
		});

	spiceRack.parts.push_back({
		ShapeType::CONE,
		{ 1.0f, 4.0f, 1.0f },
		190.0f, 0.0f, 0.0f,
		{ -5.0f, 5.0f, -3.0f },
		"wood",
		"cone",
		true
		});

	// Middle tier
	spiceRack.parts.push_back({
		ShapeType::CYLINDER,
		{ 3.5f, 2.0f, 3.5f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 4.0f, -3.0f },
		"wood",
		"cylinder",
		true
		});

	spiceRack.parts.push_back({
		ShapeType::CONE,
		{ 1.0f, 4.0f, 1.0f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 4.0f, -3.0f },
		"wood",
		"cone",
		true
		});

	spiceRack.parts.push_back({
		ShapeType::CONE,
		{ 1.0f, 4.0f, 1.0f },
		190.0f, 0.0f, 0.0f,
		{ -5.0f, 9.0f, -3.0f },
		"wood",
		"cone",
		true
		});

	// Top tier
	spiceRack.parts.push_back({
		ShapeType::CYLINDER,
		{ 2.0f, 1.5f, 2.0f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 9.0f, -3.0f },
		"wood",
		"cylinder",
		true
		});

	spiceRack.parts.push_back({
		ShapeType::CONE,
		{ 1.0f, 4.0f, 1.0f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 9.0f, -3.0f },
		"wood",
		"cone",
		true
		});

	spiceRack.parts.push_back({
		ShapeType::CYLINDER,
		{ 0.5f, 1.5f, 0.5f },
		0.0f, 0.0f, 0.0f,
		{ -5.0f, 12.0f, -3.0f },
		"wood",
		"cylinder",
		true
		});

	m_compositeObjects.push_back(spiceRack);
}

void SceneManager::RenderCompositeObject(const CompositeObject& compositeObj)
{
	for (const auto& part : compositeObj.parts)
	{
		RenderSceneObject(part);
	}
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	LoadSceneTextures();
	DefineObjectMaterials();
	SetupSceneLights();

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();

	InitializeSceneObjects();
	InitializeCompositeObjects();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Render standard scene objects
	for (const auto& obj : m_sceneObjects)
	{
		RenderSceneObject(obj);
	}

	// Render grouped composite objects
	for (const auto& compositeObj : m_compositeObjects)
	{
		RenderCompositeObject(compositeObj);
	}
}