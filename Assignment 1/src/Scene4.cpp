// Scene4.cpp
#include "Scene4.h"

// Scale factors for models
constexpr float ModelScaleFactor = 0.01f;
constexpr float PlantScaleFactor = 0.005f;
constexpr float SphereScaleFactor = 0.5f;

Scene4::Scene4(Camera& camera, LightManager& lightManager)
	: LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
	  SkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
	  TerrainShader("resources/shaders/TerrainVertexShader.vert", "resources/shaders/TerrainFragmentShader.frag"),
	  // Terrain shader
	  GardenPlant("resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj",
	              "PolygonAncientWorlds_Texture_01_A.png"),
	  Tree("resources/models/AncientEmpire/SM_Env_Tree_Palm_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  Statue("resources/models/AncientEmpire/SM_Prop_Statue_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  Sphere("resources/models/Sphere/Sphere_HighPoly.obj", "PolygonAncientWorlds_Texture_01_A.png"),
	  GCamera(camera),
	  GLightManager(lightManager),
	  material(), 
    terrain(HeightMapInfo{ "resources/heightmap/Heightmap0.raw", 100, 100, 1.0f })
{
	std::cout << "Scene4 constructor called" << std::endl;
}

void Scene4::load() {
    std::cout << "Loading resources for Scene4..." << std::endl;
    // Initialize lighting
    GLightManager.initialize();

    // Set material properties
    material.Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.Shininess = 32.0f;
}

void Scene4::update(float deltaTime) {
    // Input handling and light updates can go here
}

void Scene4::render() {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the terrain shader and set view/projection matrices
    TerrainShader.use();  // Use the terrain shader
    TerrainShader.setMat4("view", GCamera.getViewMatrix());
    TerrainShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));

    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.05f, 0.1f)); // Scale down height (Y) more than width/depth
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust terrain position if necessary
    TerrainShader.setMat4("model", modelMatrix);

    terrain.DrawTerrain();  // Draw terrain

    glCullFace(GL_BACK);
    // Global translation to move models by 15 units towards the positive Z axis
    glm::mat4 globalTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 15.0f));

    // Switch to the lighting shader for other objects
    LightingShader.use();
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    LightingShader.setVec3("viewPos", GCamera.VPosition);

    // Set spotlight properties based on camera position and direction
    GLightManager.setSpotLightPosition(GCamera.VPosition);
    GLightManager.setSpotLightDirection(GCamera.VFront);

    // Set material properties for objects
    LightingShader.setMaterial(material);

    // Render garden plants as ground
    LightingShader.setBool("useTexture", true);
    modelMatrix = glm::mat4(1.0f);

    for (int X = -5; X <= 5; X++) {
        for (int Z = -5; Z <= 5; Z++) {
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(X, 0.0f, Z * 0.8f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(PlantScaleFactor));
            modelMatrix = globalTranslation * modelMatrix;  // Apply global translation
            LightingShader.setMat4("model", modelMatrix);
            GardenPlant.draw(LightingShader);
        }
    }

    // Render trees
    glm::vec3 TreePositions[] = {
        {-6.0f, 0.0f, -5.0f}, {6.0f, 0.0f, -5.0f},
        {-6.0f, 0.0f, 5.0f}, {6.0f, 0.0f, 5.0f}
    };

    for (glm::vec3 Pos : TreePositions) {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, Pos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
        modelMatrix = globalTranslation * modelMatrix;  // Apply global translation
        LightingShader.setMat4("model", modelMatrix);
        Tree.draw(LightingShader);
    }

    // Render statue
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
    modelMatrix = globalTranslation * modelMatrix;  // Apply global translation
    LightingShader.setMat4("model", modelMatrix);
    Statue.draw(LightingShader);

    // Render point light spheres
    glm::vec3 SpherePositions[] = {
        glm::vec3(-2.0f, 1.5f, 0.0f),
        glm::vec3(2.0f, 1.5f, 0.0f)
    };

    // Apply global translation to the point lights
    LightingShader.setBool("useTexture", false);

    for (int I = 0; I < 2; I++) {
        // Create a non-const copy of the position before applying the translation
        glm::vec3 TranslatedSpherePosition = SpherePositions[I] + glm::vec3(0.0f, 0.0f, 15.0f);  // Move light positions by 15 units in Z

        // Update light position in shader
        GLightManager.getPointLight(I).Position = TranslatedSpherePosition;
        LightingShader.setVec3("pointLights[" + std::to_string(I) + "].position", TranslatedSpherePosition);

        // Set up the sphere model for rendering
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, TranslatedSpherePosition);  // Use translated position for sphere rendering
        modelMatrix = glm::scale(modelMatrix, glm::vec3(SphereScaleFactor));
        LightingShader.setMat4("model", modelMatrix);

        glm::vec3 SphereColor = GLightManager.isPointLightsOn() ? GLightManager.getPointLight(I).Colour : glm::vec3(0.0f);
        LightingShader.setVec3("solidColor", SphereColor);

        Sphere.draw(LightingShader);
    }


    GLightManager.updateLighting(LightingShader);  // Update lighting for shaders

    // Render skybox
    LSkybox.render(SkyboxShader, GCamera, 800, 600);
}

void Scene4::cleanup() {
    std::cout << "Cleaning up Scene4 resources..." << std::endl;

    // 1. Clean up shaders
    if (LightingShader.getId() != 0) {
        glDeleteProgram(LightingShader.getId());  // Assuming getID() returns the shader program ID
    }
    if (SkyboxShader.getId() != 0) {
        glDeleteProgram(SkyboxShader.getId());
    }

    // 2. Clean up models (GardenPlant, Tree, Statue, Sphere)
    GardenPlant.cleanup();  // Assuming Model::cleanup() is implemented
    Tree.cleanup();
    Statue.cleanup();
    Sphere.cleanup();

    // 3. Clean up skybox resources if necessary
    LSkybox.cleanup();  // Assuming Skybox has a cleanup method that deletes its VAO, textures, etc.
}
