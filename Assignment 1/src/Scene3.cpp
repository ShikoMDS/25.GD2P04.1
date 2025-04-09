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
    : QuadShader("resources/shaders/QuadVertexShader.vert", "resources/shaders/QuadFragmentShader.frag"),
    AnimationShader("resources/shaders/AnimationVertexShader.vert", "resources/shaders/AnimationFragmentShader.frag"),
    GCamera(camera),
    GLightManager(lightManager),
    // Seed Perlin noise with current time for uniqueness
    perlinGenerator(static_cast<unsigned int>(std::time(nullptr))),
    // Initialize empty terrain (just to satisfy the requirement)
    noiseTerrain(HeightMapInfo{ "resources/heightmap/Heightmap0.raw", 512, 512, 1.0f }),
    // Initialize quads
    staticNoiseQuad(),
    animatedNoiseQuad()
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

    noiseTexture = 0;
    animatedNoiseTexture = 0;
}

void Scene3::load() {
    std::cout << "Loading resources for Scene3..." << std::endl;

    // Generate Perlin noise
    generatePerlinNoise();
    noiseGenerated = true;

    std::cout << "Perlin noise generated and saved successfully." << std::endl;
}

void Scene3::generatePerlinNoise() {
    std::cout << "Generating Perlin noise..." << std::endl;

    try {
        // Generate the noise map with good distribution
        noiseMap = perlinGenerator.generateNoiseMap(
            noiseWidth,
            noiseHeight,
            50.0f,     // Larger scale for broader features
            3,         // Fewer octaves for less detail
            0.5f,      // Persistence
            2.0f       // Lacunarity
        );

        // Ensure directories exist before saving files
        std::string rawFilePath = "resources/heightmap/perlin_noise.raw";
        std::string jpgFilePath = "resources/textures/perlin_noise.jpg";

        ensureDirectoryExists(rawFilePath);
        ensureDirectoryExists(jpgFilePath);

        // Save the noise map as a RAW file for terrain heightmap (requirement #4)
        std::cout << "Saving RAW heightmap to: " << rawFilePath << std::endl;
        perlinGenerator.saveAsRaw(noiseMap, noiseWidth, noiseHeight, rawFilePath);

        // Save the noise map as a JPG file for visualization (requirement #2)
        std::cout << "Saving JPG visualization to: " << jpgFilePath << std::endl;
        perlinGenerator.saveAsJpg(noiseMap, noiseWidth, noiseHeight, jpgFilePath, fireColorGradient);

        // Delete any existing textures before creating new ones
        if (noiseTexture != 0) {
            glDeleteTextures(1, &noiseTexture);
            noiseTexture = 0;
        }

        // Create OpenGL texture for the static noise quad
        noiseTexture = perlinGenerator.createNoiseTexture(noiseMap, noiseWidth, noiseHeight, fireColorGradient);
        if (noiseTexture != 0) {
            std::cout << "Created static noise texture with ID: " << noiseTexture << std::endl;
        }
        else {
            std::cerr << "FAILED to create static noise texture!" << std::endl;
        }

        // Initialize animated noise with the same parameters
        animatedNoiseMap = noiseMap; // Start with the same noise

        // Delete any existing animated texture before creating a new one
        if (animatedNoiseTexture != 0) {
            glDeleteTextures(1, &animatedNoiseTexture);
            animatedNoiseTexture = 0;
        }

        animatedNoiseTexture = perlinGenerator.createNoiseTexture(animatedNoiseMap, noiseWidth, noiseHeight, fireColorGradient);
        if (animatedNoiseTexture != 0) {
            std::cout << "Created animated noise texture with ID: " << animatedNoiseTexture << std::endl;
        }
        else {
            std::cerr << "FAILED to create animated noise texture!" << std::endl;
        }

        std::cout << "Perlin noise generated and saved successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error generating Perlin noise: " << e.what() << std::endl;
    }
}

void Scene3::update(float deltaTime) {
    // Update animation time
    animationTime += deltaTime;

    // Update animated texture every 0.1 seconds
    static float timeSinceLastUpdate = 0.0f;
    timeSinceLastUpdate += deltaTime;

    if (timeSinceLastUpdate > 0.1f) {
        updateAnimatedNoise(timeSinceLastUpdate);
        timeSinceLastUpdate = 0.0f;
    }
}

void Scene3::updateAnimatedNoise(float deltaTime) {
    // Only update if we have valid noise data
    if (animatedNoiseMap.empty()) {
        return;
    }

    // Create time-dependent offset for the noise
    glm::vec2 offset(
        animationTime * 0.5f,            // Steady horizontal movement
        sin(animationTime * 0.3f) * 3.0f // Oscillating vertical movement
    );

    try {
        // Generate new noise with time-based offset and parameters
        animatedNoiseMap = perlinGenerator.generateNoiseMap(
            noiseWidth,
            noiseHeight,
            50.0f + sin(animationTime * 0.2f) * 10.0f,  // Varying scale
            3,
            0.5f,
            2.0f,
            offset
        );

        // Delete the old texture first if it exists
        if (glIsTexture(animatedNoiseTexture)) {
            glDeleteTextures(1, &animatedNoiseTexture);
        }

        // Create the new texture - use a more dramatic fire gradient for animation
        std::vector<glm::vec3> animatedGradient = {
            glm::vec3(0.0f, 0.0f, 0.0f),      // Black (low values)
            glm::vec3(0.5f, 0.0f, 0.0f),      // Dark red
            glm::vec3(0.7f, 0.0f, 0.0f),      // Red
            glm::vec3(1.0f, 0.3f, 0.0f),      // Dark orange
            glm::vec3(1.0f, 0.5f, 0.0f),      // Orange
            glm::vec3(1.0f, 0.7f, 0.0f),      // Light orange
            glm::vec3(1.0f, 1.0f, 0.3f),      // Yellow
            glm::vec3(1.0f, 1.0f, 0.7f),      // Light yellow
            glm::vec3(1.0f, 1.0f, 1.0f)       // White (high values)
        };

        animatedNoiseTexture = perlinGenerator.createNoiseTexture(
            animatedNoiseMap, noiseWidth, noiseHeight, animatedGradient
        );
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating animated noise: " << e.what() << std::endl;
    }
}

void Scene3::render() {
    // Clear the screen with a dark blue background
    glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset all OpenGL errors before starting
    while (glGetError() != GL_NO_ERROR);

    // Disable depth testing and face culling for 2D rendering
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Set up matrices for 2D rendering
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    // DRAW FIRST QUAD (STATIC NOISE) - TOP HALF
    if (noiseTexture > 0 && glIsTexture(noiseTexture)) {
        // Make sure the shader is valid
        if (QuadShader.getId() != 0) {
            QuadShader.use();

            // Check for errors after shader activation
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "Error after QuadShader.use(): " << err << std::endl;
            }

            // Set uniforms
            QuadShader.setMat4("model", glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 1.0f)));
            QuadShader.setMat4("view", view);
            QuadShader.setMat4("projection", projection);

            // Explicitly set the texture unit and bind
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            QuadShader.setInt("texture1", 0);

            // Draw the quad
            staticNoiseQuad.draw(QuadShader, noiseTexture);

            // Check for errors after drawing
            err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "Error after drawing static quad: " << err << std::endl;
            }
        }
        else {
            std::cerr << "QuadShader is invalid!" << std::endl;
        }
    }
    else {
        std::cerr << "Static noise texture " << noiseTexture << " is invalid" << std::endl;
    }

    // DRAW SECOND QUAD (ANIMATED NOISE) - BOTTOM HALF
    if (animatedNoiseTexture > 0 && glIsTexture(animatedNoiseTexture)) {
        // Make sure the shader is valid
        if (AnimationShader.getId() != 0) {
            AnimationShader.use();

            // Check for errors after shader activation
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "Error after AnimationShader.use(): " << err << std::endl;
            }

            // Set uniforms
            AnimationShader.setMat4("model", glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 1.0f)));
            AnimationShader.setMat4("view", view);
            AnimationShader.setMat4("projection", projection);
            AnimationShader.setFloat("time", animationTime);

            // Explicitly set the texture unit and bind
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, animatedNoiseTexture);
            AnimationShader.setInt("texture1", 0);

            // Draw the quad
            animatedNoiseQuad.draw(AnimationShader, animatedNoiseTexture);

            // Check for errors after drawing
            err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "Error after drawing animated quad: " << err << std::endl;
            }
        }
        else {
            std::cerr << "AnimationShader is invalid!" << std::endl;
        }
    }
    else {
        std::cerr << "Animated noise texture " << animatedNoiseTexture << " is invalid" << std::endl;
    }
}


void Scene3::cleanup() {
    std::cout << "Cleaning up Scene3 resources..." << std::endl;

    // Clean up shaders
    if (QuadShader.getId() != 0) {
        glDeleteProgram(QuadShader.getId());
    }

    if (AnimationShader.getId() != 0) {
        glDeleteProgram(AnimationShader.getId());
    }

    // Clean up textures - check if they exist first
    if (glIsTexture(noiseTexture)) {
        glDeleteTextures(1, &noiseTexture);
        noiseTexture = 0;
    }

    if (glIsTexture(animatedNoiseTexture)) {
        glDeleteTextures(1, &animatedNoiseTexture);
        animatedNoiseTexture = 0;
    }

    // Clean up quads
    staticNoiseQuad.cleanup();
    animatedNoiseQuad.cleanup();

    // Release heightmap data
    noiseMap.clear();
    animatedNoiseMap.clear();

    // Clean up skybox
    LSkybox.cleanup();

    std::cout << "Scene3 cleanup completed" << std::endl;
}