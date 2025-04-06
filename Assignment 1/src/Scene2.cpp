#include "Scene2.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>

constexpr float ModelScaleFactor = 0.01f;

Scene2::Scene2(Camera& camera, LightManager& lightManager)
    : LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
    SkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
    TerrainShader("resources/shaders/TerrainVertexShader.vert", "resources/shaders/TerrainFragmentShader.frag"),
    Sphere("resources/models/Sphere/Sphere_HighPoly.obj", ""),
    GCamera(camera),
    GLightManager(lightManager),
    terrain(HeightMapInfo{ "resources/heightmap/Heightmap0.raw", 512, 512, 1.0f })
{
    std::cout << "Scene2 constructor called" << std::endl;
    
    // Load textures for terrain
    terrainTextures[0] = loadTexture("resources/textures/grass.jpg");      // Grass (lowest)
    terrainTextures[1] = loadTexture("resources/textures/dirt.jpg");       // Dirt/Soil
    terrainTextures[2] = loadTexture("resources/textures/rock.jpg");       // Rock/Stone
    terrainTextures[3] = loadTexture("resources/textures/snow.jpg");       // Snow (highest)
}

void Scene2::load() {
    std::cout << "Loading resources for Scene2..." << std::endl;
    
    // Initialize lighting with stronger directional light for better terrain shadows
    GLightManager.initialize();
    
    // Set the directional light to shine at an angle for better terrain definition
    // This can be done by modifying the direction in LightManager if needed
    
    // Initialize material properties for terrain
    material.Ambient = glm::vec3(0.6f, 0.6f, 0.6f);    // Brighter ambient for terrain
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.2f, 0.2f, 0.2f);   // Low specular for terrain
    material.Shininess = 16.0f;                        // Lower shininess for terrain
}

void Scene2::update(float deltaTime) {
    // Handle any updates per frame here (if needed)
    // For example, cloud movement, time of day, etc.
}

void Scene2::render() {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render skybox first
    LSkybox.render(SkyboxShader, GCamera, 800, 600);

    // Setup terrain shader
    TerrainShader.use();
    TerrainShader.setMat4("view", GCamera.getViewMatrix());
    TerrainShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    TerrainShader.setVec3("viewPos", GCamera.VPosition);

    // Pass lighting information to terrain shader
    TerrainShader.setVec3("directionalLight.direction", glm::vec3(0.2f, -1.0f, 0.3f));  // Angled light for better shadows
    TerrainShader.setVec3("directionalLight.color", glm::vec3(1.0f, 1.0f, 1.0f));      // White light
    TerrainShader.setFloat("directionalLight.intensity", 3.0f);                         // Increased intensity for better visibility

    // Pass material properties to terrain shader
    TerrainShader.setVec3("material.ambient", material.Ambient);
    TerrainShader.setVec3("material.diffuse", material.Diffuse);
    TerrainShader.setVec3("material.specular", material.Specular);
    TerrainShader.setFloat("material.shininess", material.Shininess);

    // Try to use textures but provide color fallbacks
    TerrainShader.setBool("useTextures", true);  // Try to use textures first

    // Pass terrain textures to shader
    for (int i = 0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, terrainTextures[i]);
        TerrainShader.setInt("terrainTextures[" + std::to_string(i) + "]", i);
    }

    // Pass solid colors for fallback or when textures aren't loaded properly
    TerrainShader.setVec3("terrainColors[0]", glm::vec3(0.1f, 0.6f, 0.1f));    // Grass (bright green)
    TerrainShader.setVec3("terrainColors[1]", glm::vec3(0.7f, 0.5f, 0.1f));    // Dirt/rock (brown/orange)
    TerrainShader.setVec3("terrainColors[2]", glm::vec3(0.8f, 0.7f, 0.1f));    // Higher rock (tan)
    TerrainShader.setVec3("terrainColors[3]", glm::vec3(1.0f, 1.0f, 1.0f));    // Snow (white)

    // Pass height thresholds for textures
    TerrainShader.setFloat("heightLevels[0]", 0.0f);   // Grass level (lowest)
    TerrainShader.setFloat("heightLevels[1]", 0.4f);   // Dirt level
    TerrainShader.setFloat("heightLevels[2]", 0.7f);   // Rock level
    TerrainShader.setFloat("heightLevels[3]", 0.85f);  // Snow level (highest)
    TerrainShader.setFloat("blendFactor", 0.1f);      // Controls how much blending between textures

    // Set terrain model matrix with reduced scaling
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -5.0f, 0.0f)); // Position terrain below camera
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f, 0.05f, 0.05f));   // Scale terrain down significantly
    TerrainShader.setMat4("model", modelMatrix);

    // Render the terrain
    terrain.DrawTerrain();

    // Removed point light visualization sphere
}

void Scene2::cleanup() {
    std::cout << "Cleaning up Scene2 resources..." << std::endl;
    
    // Delete shaders
    if (LightingShader.getId() != 0)
        glDeleteProgram(LightingShader.getId());
    if (SkyboxShader.getId() != 0)
        glDeleteProgram(SkyboxShader.getId());
    if (TerrainShader.getId() != 0)
        glDeleteProgram(TerrainShader.getId());
    
    // Delete textures
    for (int i = 0; i < 4; i++) {
        if (terrainTextures[i] != 0)
            glDeleteTextures(1, &terrainTextures[i]);
    }
    
    // Cleanup models
    Sphere.cleanup();
    LSkybox.cleanup();
}

// Helper function to load textures
GLuint Scene2::loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // Load texture using the existing textureFromFile function used elsewhere in the codebase
    // This is just a placeholder - the actual loading happens in textureFromFile
    textureID = textureFromFile(path.c_str(), "resources/textures", false);
    
    return textureID;
}