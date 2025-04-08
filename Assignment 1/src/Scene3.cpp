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

    // Create OpenGL texture for the static noise quad
    noiseTexture = perlinGenerator.createNoiseTexture(noiseMap, noiseWidth, noiseHeight, fireColorGradient);
    if (noiseTexture != 0) {
        std::cout << "Created static noise texture with ID: " << noiseTexture << std::endl;
    }

    // Initialize animated noise with the same parameters
    animatedNoiseMap = noiseMap; // Start with the same noise
    animatedNoiseTexture = perlinGenerator.createNoiseTexture(animatedNoiseMap, noiseWidth, noiseHeight, fireColorGradient);
    if (animatedNoiseTexture != 0) {
        std::cout << "Created animated noise texture with ID: " << animatedNoiseTexture << std::endl;
    }

    // Apply the raw file to the 3D terrain object (requirement #4)
    // We don't need to actually render this, just apply the heightmap
    noiseTerrain = Terrain(HeightMapInfo{ rawFilePath, 512, 512, 1.0f });
    std::cout << "Applied noise data to 3D terrain object" << std::endl;
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
    // Update the animated noise texture
    glm::vec2 offset(animationTime * 0.3f, animationTime * 0.2f);

    // Generate new noise with time-based offset
    animatedNoiseMap = perlinGenerator.generateNoiseMap(
        noiseWidth, noiseHeight, 50.0f,
        3, 0.5f, 2.0f, offset
    );

    // Update the texture
    if (animatedNoiseTexture != 0) {
        glDeleteTextures(1, &animatedNoiseTexture);
    }
    animatedNoiseTexture = perlinGenerator.createNoiseTexture(animatedNoiseMap, noiseWidth, noiseHeight, fireColorGradient);
}

void Scene3::render() {
    // Clear the screen with a dark blue background so we can tell rendering is happening
    glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Debug info
    std::cout << "Scene3 render - Static texture ID: " << noiseTexture
        << ", Animated texture ID: " << animatedNoiseTexture << std::endl;

    // Disable depth testing temporarily (ensures quads always render)
    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glDisable(GL_DEPTH_TEST);

    // Save current state
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Try using your Quad class first
    if (noiseTexture > 0) {
        // Set up view for quads
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.3f, -2.0f));
        model = glm::scale(model, glm::vec3(0.5f));

        QuadShader.use();
        QuadShader.setMat4("model", model);
        QuadShader.setMat4("view", glm::mat4(1.0f));  // Identity view matrix
        QuadShader.setMat4("projection", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f));

        // Try to draw using your Quad class
        staticNoiseQuad.draw(QuadShader, noiseTexture);
    }

    // Fallback rendering method - raw OpenGL
    glEnable(GL_TEXTURE_2D);

    // Static Noise Quad (top)
    if (noiseTexture > 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        // Draw quad with fixed pipeline
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Draw the quad
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // White for proper texture colors
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.7f, 0.1f, 0.0f);   // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.7f, 0.1f, 0.0f);    // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.7f, 0.9f, 0.0f);    // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.7f, 0.9f, 0.0f);   // Top left
        glEnd();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    // Animated Noise Quad (bottom)
    if (animatedNoiseTexture > 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, animatedNoiseTexture);

        // Draw quad with fixed pipeline
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Draw the quad
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // White for proper texture colors
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.7f, -0.9f, 0.0f);  // Bottom left
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.7f, -0.9f, 0.0f);   // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.7f, -0.1f, 0.0f);   // Top right
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.7f, -0.1f, 0.0f);  // Top left
        glEnd();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    // As an absolute fallback, draw a colored quad to verify rendering works at all
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw a simple colored quad as absolute fallback
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.2f, -0.2f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.2f, -0.2f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.2f, 0.2f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(-0.2f, 0.2f, 0.0f);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Restore state
    glPopAttrib();

    // Restore depth testing if it was enabled
    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
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
}