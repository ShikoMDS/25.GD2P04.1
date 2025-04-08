#include "Scene4.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>
#include <iostream>

// Scale factors for models (from Scene 1)
constexpr float ModelScaleFactor = 0.01f;
constexpr float PlantScaleFactor = 0.005f;

Scene4::Scene4(Camera& camera, LightManager& lightManager)
    : LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
    SkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
    TerrainShader("resources/shaders/TerrainVertexShader.vert", "resources/shaders/TerrainFragmentShader.frag"),
    PostProcessingShader("resources/shaders/PostProcessingVertexShader.vert", "resources/shaders/PostProcessingFragmentShader.frag"),
    GardenPlant("resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
    Tree("resources/models/AncientEmpire/SM_Env_Tree_Palm_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
    Statue("resources/models/AncientEmpire/SM_Prop_Statue_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
    GCamera(camera),
    GLightManager(lightManager),
    mStatueRotation(0.0f),
    terrain(HeightMapInfo{ "resources/heightmap/Heightmap0.raw", 512, 512, 1.0f }),
    framebuffer(0),
    textureColorBuffer(0),
    rbo(0),
    quadVAO(0),
    quadVBO(0),
    currentEffect(0),
    effectTime(0.0f),
    tabKeyPressed(false)
{
    std::cout << "Scene4 constructor called" << std::endl;

    // Load terrain textures (from Scene 2)
    terrainTextures[0] = loadTexture("resources/textures/tileable_grass_00.png");     // Grass (lowest)
    terrainTextures[1] = loadTexture("resources/textures/Dirt_04.png");              // Dirt/Soil
    terrainTextures[2] = loadTexture("resources/textures/rck_2.png");                // Rock/Stone
    terrainTextures[3] = loadTexture("resources/textures/snow01.png");               // Snow (highest)
}

void Scene4::load() {
    std::cout << "Loading resources for Scene4..." << std::endl;
    // Initialize lighting
    GLightManager.initialize();

    // Set material properties (combining settings from both scenes)
    material.Ambient = glm::vec3(0.8f, 0.8f, 0.8f);
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.Shininess = 32.0f;

    // Set up framebuffer and screen quad for post-processing
    setupFramebuffer();
    setupScreenQuad();
}

// Helper function to load textures (from Scene 2)
GLuint Scene4::loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Load texture using the existing textureFromFile function 
    textureID = textureFromFile(path.c_str(), "resources/textures", false);

    return textureID;
}

void Scene4::setupFramebuffer() {
    // Delete existing framebuffer resources if they exist
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    if (textureColorBuffer != 0) {
        glDeleteTextures(1, &textureColorBuffer);
        textureColorBuffer = 0;
    }

    if (rbo != 0) {
        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }

    // Create and bind the framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Get the current window dimensions for proper sizing
    int width = 800;  // Default fallback width
    int height = 600; // Default fallback height

    GLFWwindow* window = glfwGetCurrentContext();
    if (window) {
        glfwGetFramebufferSize(window, &width, &height);
    }

    // Create a texture to hold the color buffer
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

    // Use safe texture creation
    try {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Attach the texture to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

        // Create a renderbuffer object for depth and stencil attachments
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Attach the renderbuffer to the framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // Check if the framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;

            // Clean up resources if there's an error
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &textureColorBuffer);
            glDeleteRenderbuffers(1, &rbo);

            framebuffer = 0;
            textureColorBuffer = 0;
            rbo = 0;
        }
        else {
            std::cout << "Framebuffer set up successfully with dimensions: " << width << "x" << height << std::endl;
        }
    }
    catch (...) {
        std::cerr << "Exception during framebuffer setup!" << std::endl;

        // Clean up resources if there's an exception
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureColorBuffer);
        glDeleteRenderbuffers(1, &rbo);

        framebuffer = 0;
        textureColorBuffer = 0;
        rbo = 0;
    }

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene4::setupScreenQuad() {
    // Set up screen quad vertices (x, y, z, u, v)
    float quadVertices[] = {
        // Positions        // Texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };

    // Set up VAO and VBO for the screen quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture coords attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    std::cout << "Screen quad set up successfully" << std::endl;
}

void Scene4::cyclePostProcessingEffect() {
    // Check for Tab key press in the update function
    GLFWwindow* window = glfwGetCurrentContext();
    if (window) {
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabKeyPressed) {
            tabKeyPressed = true;
            currentEffect = (currentEffect + 1) % 5; // Cycle through 5 effects (including "no effect")

            // Print effect name for clarity
            std::string effectName;
            switch (currentEffect) {
            case 0: effectName = "Normal (no effect)"; break;
            case 1: effectName = "Color Inversion"; break;
            case 2: effectName = "Grayscale"; break;
            case 3: effectName = "Rain Effect"; break;
            case 4: effectName = "CRT Screen Effect"; break;
            default: effectName = "Unknown"; break;
            }

            std::cout << "Switched to post-processing effect: " << effectName << std::endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyPressed = false;
        }
    }
}

void Scene4::update(float deltaTime) {
    // Update effect time for animated effects
    effectTime += deltaTime;

    // Rotate the statue (from Scene 1)
    mStatueRotation += 45.0f * deltaTime;
    if (mStatueRotation > 360.0f)
        mStatueRotation -= 360.0f;

    // Check for Tab key to cycle through effects
    cyclePostProcessingEffect();
}

// Split the render function for clarity
void Scene4::render() {
    // First render the scene to the framebuffer
    renderSceneToFramebuffer();

    // Then render the framebuffer to the screen with post-processing
    renderPostProcessing();
}

void Scene4::renderSceneToFramebuffer() {
    // Bind to framebuffer and draw scene
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Clear all attached buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Enable depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Get frame buffer dimensions
    int width, height;
    GLFWwindow* window = glfwGetCurrentContext();
    if (window) {
        glfwGetFramebufferSize(window, &width, &height);
    }
    else {
        width = 800;
        height = 600;
    }

    // ---------------------------
    // RENDER SKYBOX FIRST
    // ---------------------------
    LSkybox.render(SkyboxShader, GCamera, width, height);

    // ---------------------------
    // RENDER TERRAIN (from Scene 2)
    // ---------------------------
    TerrainShader.use();
    TerrainShader.setMat4("view", GCamera.getViewMatrix());
    TerrainShader.setMat4("projection", GCamera.getProjectionMatrix(static_cast<float>(width), static_cast<float>(height)));
    TerrainShader.setVec3("viewPos", GCamera.VPosition);

    // Enhanced directional lighting for dramatic scene appearance
    TerrainShader.setVec3("directionalLight.direction", glm::vec3(0.3f, -0.9f, 0.3f)); // More from above
    TerrainShader.setVec3("directionalLight.color", glm::vec3(1.0f, 0.95f, 0.8f)); // Slightly warm sunlight
    TerrainShader.setFloat("directionalLight.intensity", 2.5f); // Increased intensity

    // Material adjustments
    TerrainShader.setVec3("material.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
    TerrainShader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    TerrainShader.setVec3("material.specular", glm::vec3(0.1f, 0.1f, 0.1f));
    TerrainShader.setFloat("material.shininess", 8.0f);

    // Try to use textures but provide color fallbacks
    TerrainShader.setBool("useTextures", true);

    // Pass terrain textures to shader
    for (int i = 0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, terrainTextures[i]);
        TerrainShader.setInt("terrainTextures[" + std::to_string(i) + "]", i);
    }

    // Update colors for more vibrancy and contrast
    TerrainShader.setVec3("terrainColors[0]", glm::vec3(0.1f, 0.7f, 0.1f));    // Brighter green for grass
    TerrainShader.setVec3("terrainColors[1]", glm::vec3(0.7f, 0.4f, 0.1f));    // Orange-brown for dirt
    TerrainShader.setVec3("terrainColors[2]", glm::vec3(0.8f, 0.8f, 0.7f));    // Light beige for rock
    TerrainShader.setVec3("terrainColors[3]", glm::vec3(1.0f, 1.0f, 1.0f));    // Pure white for snow

    // Update height thresholds for more defined transitions
    TerrainShader.setFloat("heightLevels[0]", 0.0f);   // Grass level (lowest)
    TerrainShader.setFloat("heightLevels[1]", 0.05f);   // Dirt level
    TerrainShader.setFloat("heightLevels[2]", 0.15f);   // Rock level
    TerrainShader.setFloat("heightLevels[3]", 0.225f);  // Snow level (highest)
    TerrainShader.setFloat("blendFactor", 0.1f);       // Moderate blending

    // Set terrain model matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.5f, 20.0f)); // Position at origin 
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.025f, 0.004f, 0.025f)); // Scale the terrain
    TerrainShader.setMat4("model", modelMatrix);

    // Set front face definition and enable culling
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Render the terrain
    terrain.DrawTerrain();

    // Reset texture units
    for (int i = 0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);

    // ---------------------------
    // RENDER SCENE 1 OBJECTS (WITHOUT STENCIL OUTLINE)
    // ---------------------------
    LightingShader.use();
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(static_cast<float>(width), static_cast<float>(height)));
    LightingShader.setVec3("viewPos", GCamera.VPosition);
    LightingShader.setMaterial(material);
    LightingShader.setBool("useTexture", true);
    GLightManager.updateLighting(LightingShader);
    glActiveTexture(GL_TEXTURE0);

    // Define positions for objects - place them on the flat ground in front of the mountain
    glm::vec3 treePositions[] = {
        {-3.0f, 2.55f, 22.0f},  // Tree left
        {1.0f, 2.55f, 22.0f},   // Tree right
        {-3.0f, 2.55f, 26.0f},  // Tree further left
        {1.0f, 2.55f, 26.0f}    // Tree further right
    };

    // Make trees smaller and place them on the flat ground
    for (int i = 0; i < 4; i++) {
        glm::mat4 modelMatrixTree = glm::mat4(1.0f);
        modelMatrixTree = glm::translate(modelMatrixTree, treePositions[i]);
        modelMatrixTree = glm::scale(modelMatrixTree, glm::vec3(0.004f)); // Smaller scale
        LightingShader.setMat4("model", modelMatrixTree);
        Tree.draw(LightingShader);
    }

    // Draw the statue - place it in front of the mountain on the flat ground
    glm::mat4 modelMatrixStatue = glm::mat4(1.0f);
    modelMatrixStatue = glm::translate(modelMatrixStatue, glm::vec3(-1.0f, 2.5f, 24.0f)); // Further back on flat ground
    modelMatrixStatue = glm::rotate(modelMatrixStatue, glm::radians(mStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrixStatue = glm::scale(modelMatrixStatue, glm::vec3(0.004f)); // Smaller scale
    LightingShader.setMat4("model", modelMatrixStatue);
    Statue.draw(LightingShader);

    // Make garden plants smaller and arrange them on the flat ground
    for (int x = -4; x <= 4; x++) {
        for (int z = 0; z <= 9; z++) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            // Position plants on the flat area
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-1 + x * 0.35f, 2.5f, 22.5f + z * 0.35f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.002f)); // Even smaller scale
            LightingShader.setMat4("model", modelMatrix);
            GardenPlant.draw(LightingShader);
        }
    }
}

void Scene4::renderPostProcessing() {
    // Only proceed if we have a valid framebuffer and texture
    if (framebuffer == 0 || textureColorBuffer == 0) {
        std::cerr << "Cannot render post-processing: Invalid framebuffer resources" << std::endl;
        return;
    }

    // Render to default framebuffer (screen)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Save GL state
    GLboolean depthTestEnabled;
    GLboolean cullFaceEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);

    // Disable depth test and face culling for rendering the screen-filling quad
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Check if shader is valid
    if (PostProcessingShader.getId() == 0) {
        std::cerr << "Cannot render post-processing: Invalid shader" << std::endl;
        return;
    }

    // Activate post-processing shader
    PostProcessingShader.use();

    // Set uniforms for post-processing
    PostProcessingShader.setInt("screenTexture", 0);
    PostProcessingShader.setInt("effect", currentEffect);
    PostProcessingShader.setFloat("time", effectTime);

    // Bind the framebuffer texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

    // Check if VAO exists
    if (quadVAO == 0) {
        std::cerr << "Cannot render post-processing: Invalid quad VAO" << std::endl;
        return;
    }

    // Render the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Reset state
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore previous state
    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
    if (cullFaceEnabled) glEnable(GL_CULL_FACE);
}

void Scene4::cleanup() {
    std::cout << "Cleaning up Scene4 resources..." << std::endl;

    // Clean up shaders
    if (LightingShader.getId() != 0) {
        glDeleteProgram(LightingShader.getId());
        LightingShader.Id = 0;
    }
    if (SkyboxShader.getId() != 0) {
        glDeleteProgram(SkyboxShader.getId());
        SkyboxShader.Id = 0;
    }
    if (TerrainShader.getId() != 0) {
        glDeleteProgram(TerrainShader.getId());
        TerrainShader.Id = 0;
    }
    if (PostProcessingShader.getId() != 0) {
        glDeleteProgram(PostProcessingShader.getId());
        PostProcessingShader.Id = 0;
    }

    // Clean up models
    GardenPlant.cleanup();
    Tree.cleanup();
    Statue.cleanup();
    LSkybox.cleanup();

    // Clean up terrain textures - check if they exist first
    for (int i = 0; i < 4; i++) {
        if (glIsTexture(terrainTextures[i])) {
            glDeleteTextures(1, &terrainTextures[i]);
            terrainTextures[i] = 0;
        }
    }

    // Clean up framebuffer objects
    if (glIsFramebuffer(framebuffer)) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    if (glIsTexture(textureColorBuffer)) {
        glDeleteTextures(1, &textureColorBuffer);
        textureColorBuffer = 0;
    }

    if (glIsRenderbuffer(rbo)) {
        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }

    // Clean up screen quad
    if (glIsVertexArray(quadVAO)) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }

    if (glIsBuffer(quadVBO)) {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }

    std::cout << "Scene4 cleanup complete" << std::endl;
}