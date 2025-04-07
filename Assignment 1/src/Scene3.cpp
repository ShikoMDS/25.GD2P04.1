#include "Scene3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

// Helper function to ensure directory exists
void ensureDirectoryExists(const std::string& path) {
    // Extract directory path
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return; // No directory component
    }

    std::string dir = path.substr(0, pos);

    // Create directory if it doesn't exist
#ifdef _WIN32
// Windows
    std::string cmd = "if not exist \"" + dir + "\" mkdir \"" + dir + "\"";
    system(cmd.c_str());
#else
// Linux/Unix
    std::string cmd = "mkdir -p \"" + dir + "\"";
    system(cmd.c_str());
#endif

    std::cout << "Ensuring directory exists: " << dir << std::endl;
}

// Constructor
Scene3::Scene3(Camera& camera, LightManager& lightManager)
    : LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
    SkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
    QuadShader("resources/shaders/QuadVertexShader.vert", "resources/shaders/QuadFragmentShader.frag"),
    AnimationShader("resources/shaders/AnimationVertexShader.vert", "resources/shaders/AnimationFragmentShader.frag"),
    TerrainShader("resources/shaders/PerlinTerrainVertexShader.vert", "resources/shaders/PerlinTerrainFragmentShader.frag"),
    GCamera(camera),
    GLightManager(lightManager),
    material(),
    // Seed Perlin noise with current time for uniqueness
    perlinGenerator(static_cast<unsigned int>(std::time(nullptr))),
    // Start with a default heightmap, we'll update this after generation
    noiseTerrain(HeightMapInfo{ "resources/heightmap/Heightmap0.raw", 512, 512, 1.0f })
{
    std::cout << "Scene3 constructor called" << std::endl;

    // Define fire color gradient for noise visualization
    fireColorGradient = {
        glm::vec3(0.0f, 0.0f, 0.0f),  // Black (low values)
        glm::vec3(0.5f, 0.0f, 0.0f),  // Dark red
        glm::vec3(1.0f, 0.0f, 0.0f),  // Red
        glm::vec3(1.0f, 0.5f, 0.0f),  // Orange
        glm::vec3(1.0f, 1.0f, 0.0f),  // Yellow
        glm::vec3(1.0f, 1.0f, 1.0f)   // White (high values)
    };
}

void Scene3::load() {
    std::cout << "Loading resources for Scene3..." << std::endl;

    // Initialize lighting
    GLightManager.initialize();

    // Set material properties
    material.Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.Shininess = 32.0f;

    // Generate Perlin noise first - this is critical
    generatePerlinNoise();
    noiseGenerated = true;

    std::cout << "Perlin noise generated and saved successfully." << std::endl;

    // IMPORTANT: Wait a brief moment to ensure file is written completely
    // Simple delay loop that doesn't require platform-specific functions
    for (volatile int i = 0; i < 10000000; i++) {
        // Empty loop to create delay
    }

    // Now set up the terrain
    std::cout << "Setting up terrain from generated noise..." << std::endl;
    noiseTerrain.SetupTerrain();
}

void Scene3::generatePerlinNoise() {
    std::cout << "Generating Perlin noise..." << std::endl;

    // Generate the static noise map
    noiseMap = perlinGenerator.generateNoiseMap(noiseWidth, noiseHeight, noiseScale, noiseOctaves, noisePersistence, noiseLacunarity);

    // Ensure directories exist before saving files
    std::string rawFilePath = "resources/heightmap/perlin_noise.raw";
    std::string jpgFilePath = "resources/textures/perlin_noise.jpg";

    ensureDirectoryExists(rawFilePath);
    ensureDirectoryExists(jpgFilePath);

    // Save the noise map as a RAW file for terrain heightmap
    std::cout << "Saving RAW heightmap to: " << rawFilePath << std::endl;
    bool rawSaved = perlinGenerator.saveAsRaw(noiseMap, noiseWidth, noiseHeight, rawFilePath);
    if (!rawSaved) {
        std::cerr << "Failed to save RAW file!" << std::endl;
    }

    // Save the noise map as a JPG file for visualization
    std::cout << "Saving JPG visualization to: " << jpgFilePath << std::endl;
    bool jpgSaved = perlinGenerator.saveAsJpg(noiseMap, noiseWidth, noiseHeight, jpgFilePath, fireColorGradient);

    // Create OpenGL texture for the static noise quad
    noiseTexture = perlinGenerator.createNoiseTexture(noiseMap, noiseWidth, noiseHeight, fireColorGradient);

    // Initialize animated noise with the same parameters but will be updated over time
    animatedNoiseMap = perlinGenerator.generateNoiseMap(noiseWidth, noiseHeight, noiseScale, noiseOctaves, noisePersistence, noiseLacunarity);
    animatedNoiseTexture = perlinGenerator.createNoiseTexture(animatedNoiseMap, noiseWidth, noiseHeight, fireColorGradient);

    // Double check that the raw file was created successfully
    std::ifstream testFile("resources/heightmap/perlin_noise.raw", std::ios::binary);
    if (!testFile.is_open()) {
        std::cerr << "WARNING: Could not open perlin_noise.raw for verification. Creating a backup copy from memory." << std::endl;
        // Create another copy of the file as a fallback
        std::string backupFile = "perlin_noise_backup.raw";
        perlinGenerator.saveAsRaw(noiseMap, noiseWidth, noiseHeight, backupFile);

        // Try to move the file to the correct location
        std::ifstream src(backupFile, std::ios::binary);
        if (src) {
            ensureDirectoryExists("resources/heightmap/perlin_noise.raw");
            std::ofstream dst("resources/heightmap/perlin_noise.raw", std::ios::binary);
            if (dst) {
                dst << src.rdbuf();
                std::cout << "Successfully created heightmap file from backup." << std::endl;
            }
            src.close();
            if (dst) dst.close();
        }

        // Create the terrain directly from our noise data in memory
        std::cout << "Creating terrain directly from noise data in memory..." << std::endl;
        // We'll reconstruct the terrain object with the right heightmap path
        noiseTerrain = Terrain(HeightMapInfo{ "resources/heightmap/perlin_noise.raw", 512, 512, 1.0f });
    }
    else {
        testFile.close();
        std::cout << "Confirmed perlin_noise.raw exists and is readable." << std::endl;

        // Re-create the terrain with the correct heightmap file
        noiseTerrain = Terrain(HeightMapInfo{ "resources/heightmap/perlin_noise.raw", 512, 512, 1.0f });
    }
}

void Scene3::update(float deltaTime) {
    // Update animation time
    animationTime += deltaTime;

    // Update animated noise texture periodically
    updateAnimatedNoise(deltaTime);
}

void Scene3::updateAnimatedNoise(float deltaTime) {
    // Update animated noise every frame with a shifting offset
    glm::vec2 offset(animationTime * 0.5f, animationTime * 0.3f);

    // Generate new noise with time-based offset
    animatedNoiseMap = perlinGenerator.generateNoiseMap(
        noiseWidth, noiseHeight, noiseScale,
        noiseOctaves, noisePersistence, noiseLacunarity,
        offset
    );

    // Update the texture
    if (animatedNoiseTexture != 0) {
        glDeleteTextures(1, &animatedNoiseTexture);
    }
    animatedNoiseTexture = perlinGenerator.createNoiseTexture(animatedNoiseMap, noiseWidth, noiseHeight, fireColorGradient);
}

void Scene3::render() {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure view and projection matrices
    glm::mat4 view = GCamera.getViewMatrix();
    glm::mat4 projection = GCamera.getProjectionMatrix(800, 600);

    // Render the skybox first
    LSkybox.render(SkyboxShader, GCamera, 800, 600);

    // Render the static noise quad in the top left
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 5.0f, -10.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 1.0f));

    QuadShader.use();
    QuadShader.setMat4("model", model);
    QuadShader.setMat4("view", view);
    QuadShader.setMat4("projection", projection);
    staticNoiseQuad.draw(QuadShader, noiseTexture);

    // Render the animated noise quad in the top right
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, 5.0f, -10.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 1.0f));

    AnimationShader.use();
    AnimationShader.setMat4("model", model);
    AnimationShader.setMat4("view", view);
    AnimationShader.setMat4("projection", projection);
    AnimationShader.setFloat("time", animationTime);
    animatedNoiseQuad.draw(AnimationShader, animatedNoiseTexture);

    // Render the terrain created from the noise data
    TerrainShader.use();
    TerrainShader.setMat4("view", view);
    TerrainShader.setMat4("projection", projection);
    TerrainShader.setVec3("viewPos", GCamera.VPosition);

    // Set directional light for the terrain
    TerrainShader.setVec3("directionalLight.direction", glm::vec3(0.2f, -1.0f, 0.3f));
    TerrainShader.setVec3("directionalLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
    TerrainShader.setFloat("directionalLight.intensity", 1.0f);

    // Set material properties for the terrain
    TerrainShader.setVec3("material.ambient", material.Ambient);
    TerrainShader.setVec3("material.diffuse", material.Diffuse);
    TerrainShader.setVec3("material.specular", material.Specular);
    TerrainShader.setFloat("material.shininess", material.Shininess);

    // Set height-based color blending parameters
    TerrainShader.setFloat("heightLevels[0]", 0.0f);    // Lowest level: Black
    TerrainShader.setFloat("heightLevels[1]", 0.3f);    // Low-mid level: Red
    TerrainShader.setFloat("heightLevels[2]", 0.6f);    // Mid-high level: Yellow
    TerrainShader.setFloat("heightLevels[3]", 0.8f);    // Highest level: White
    TerrainShader.setFloat("blendFactor", 0.1f);        // Moderate blending

    // Set fire color gradient for terrain coloring (using all 4 required colors)
    TerrainShader.setVec3("terrainColors[0]", fireColorGradient[0]);  // Black (lowest)
    TerrainShader.setVec3("terrainColors[1]", fireColorGradient[2]);  // Red
    TerrainShader.setVec3("terrainColors[2]", fireColorGradient[4]);  // Yellow
    TerrainShader.setVec3("terrainColors[3]", fireColorGradient[5]);  // White (highest)

    // Position and scale the terrain
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -5.0f, -15.0f));
    model = glm::scale(model, glm::vec3(0.05f, 0.02f, 0.05f));
    TerrainShader.setMat4("model", model);

    // Draw the terrain
    noiseTerrain.DrawTerrain();
}

void Scene3::cleanup() {
    std::cout << "Cleaning up Scene3 resources..." << std::endl;

    // Clean up shaders
    if (LightingShader.getId() != 0) {
        glDeleteProgram(LightingShader.getId());
    }
    if (SkyboxShader.getId() != 0) {
        glDeleteProgram(SkyboxShader.getId());
    }
    if (QuadShader.getId() != 0) {
        glDeleteProgram(QuadShader.getId());
    }
    if (AnimationShader.getId() != 0) {
        glDeleteProgram(AnimationShader.getId());
    }
    if (TerrainShader.getId() != 0) {
        glDeleteProgram(TerrainShader.getId());
    }

    // Clean up textures
    if (noiseTexture != 0) {
        glDeleteTextures(1, &noiseTexture);
    }
    if (animatedNoiseTexture != 0) {
        glDeleteTextures(1, &animatedNoiseTexture);
    }

    // Clean up quads
    staticNoiseQuad.cleanup();
    animatedNoiseQuad.cleanup();

    // Clean up skybox
    LSkybox.cleanup();
}