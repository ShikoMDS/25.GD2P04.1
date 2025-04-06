#include "Scene1.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

// Debug helper (if needed)
void checkGLError(const char* label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error (" << label << "): " << err << std::endl;
    }
}

Scene1::Scene1(Camera& camera, LightManager& lightManager)
    : LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"),
    SkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag"),
    OutlineShader("resources/shaders/OutlineVertexShader.vert", "resources/shaders/OutlineFragmentShader.frag"),
    GardenPlant("resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
    Tree("resources/models/AncientEmpire/SM_Env_Tree_Palm_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
    Statue("resources/models/AncientEmpire/SM_Prop_Statue_01.obj", "PolygonAncientWorlds_Texture_01_A.png"),
    GCamera(camera),
    GLightManager(lightManager),
    mStatueRotation(0.0f)
{
    std::cout << "Scene1 constructor called" << std::endl;
}

void Scene1::load() {
    std::cout << "Loading resources for Scene1..." << std::endl;
    // Initialize lighting
    GLightManager.initialize();

    // Set material properties
    material.Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.Shininess = 32.0f;
}

void Scene1::update(float deltaTime) {
    // Rotate the statue around its local Y axis (45 degrees per second)
    mStatueRotation += 45.0f * deltaTime;
    if (mStatueRotation > 360.0f)
        mStatueRotation -= 360.0f;
}

void Scene1::render() {
    // 1) Clear color, depth, and stencil buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    // ----------------------------------------------------------------
    // (A) Colored Pass: Render the full scene normally
    // ----------------------------------------------------------------
    LightingShader.use();
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    LightingShader.setVec3("viewPos", GCamera.VPosition);
    LightingShader.setMaterial(material);
    LightingShader.setBool("useTexture", true);
    GLightManager.updateLighting(LightingShader);
    glActiveTexture(GL_TEXTURE0); // ensure texture unit 0 is active

    // Define positions for the trees surrounding the statue
    glm::vec3 treePositions[] = {
        {-1.5f, 0.0f, -1.5f},  // Tree in front-left
        {-1.5f, 0.0f, 1.5f},   // Tree in back-left
        {1.5f, 0.0f, -1.5f},   // Tree in front-right
        {1.5f, 0.0f, 1.5f}     // Tree in back-right
    };

    // Render base ground plane with plants (more densely packed)
    for (int x = -3; x <= 3; x++) {
        for (int z = -3; z <= 3; z++) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(x * 0.8f, -0.2f, z * 0.8f)); // Closer together
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f)); // Slightly smaller scale
            LightingShader.setMat4("model", modelMatrix);
            GardenPlant.draw(LightingShader);
        }
    }

    // Store model matrices for objects that will have outlines
    glm::mat4 modelMatrixStatue;
    glm::mat4 modelMatrixTrees[4];

    // Draw the center rotating statue
    modelMatrixStatue = glm::mat4(1.0f);
    modelMatrixStatue = glm::translate(modelMatrixStatue, glm::vec3(0.0f, 0.0f, 0.0f)); // Center position
    modelMatrixStatue = glm::rotate(modelMatrixStatue, glm::radians(mStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrixStatue = glm::scale(modelMatrixStatue, glm::vec3(0.015f)); // Normal scale
    LightingShader.setMat4("model", modelMatrixStatue);
    Statue.draw(LightingShader);

    // Draw 4 trees
    for (int i = 0; i < 4; i++) {
        modelMatrixTrees[i] = glm::mat4(1.0f);
        modelMatrixTrees[i] = glm::translate(modelMatrixTrees[i], treePositions[i]);
        modelMatrixTrees[i] = glm::scale(modelMatrixTrees[i], glm::vec3(0.01f)); // Normal scale
        LightingShader.setMat4("model", modelMatrixTrees[i]);
        Tree.draw(LightingShader);
    }

    // Render Skybox
    LSkybox.render(SkyboxShader, GCamera, 800, 600);

    // ----------------------------------------------------------------
    // (B) Stencil Update Pass: Mark all pixels of outlined objects
    // ----------------------------------------------------------------
    glEnable(GL_STENCIL_TEST);
    // Clear stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilMask(0xFF); // enable writing to stencil buffer

    // Disable color and depth writes and disable depth test for stencil pass
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    // Set stencil operation: always replace with 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    // Draw the outlined objects into stencil buffer
    LightingShader.use();

    // Draw statue into stencil buffer
    LightingShader.setMat4("model", modelMatrixStatue);
    Statue.draw(LightingShader);

    // Draw trees into stencil buffer
    for (int i = 0; i < 4; i++) {
        LightingShader.setMat4("model", modelMatrixTrees[i]);
        Tree.draw(LightingShader);
    }

    // Restore color and depth writes, and re-enable depth test
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    // ----------------------------------------------------------------
    // (C) Outline Pass: Render blue outline where stencil != 1
    // ----------------------------------------------------------------
    // Only draw where stencil value is NOT equal to 1 (i.e. around the borders)
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);  // disable writing to stencil
    glDepthFunc(GL_ALWAYS); // force outline to draw on top

    OutlineShader.use();
    OutlineShader.setMat4("view", GCamera.getViewMatrix());
    OutlineShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    OutlineShader.setVec3("outlineColor", glm::vec3(0.0f, 0.0f, 1.0f)); // Blue outline

    // Draw statue outline with slightly larger scale
    glm::mat4 outlineMatrixStatue = glm::scale(modelMatrixStatue, glm::vec3(1.03f));
    OutlineShader.setMat4("model", outlineMatrixStatue);
    Statue.draw(OutlineShader);

    // Draw tree outlines with slightly larger scale
    for (int i = 0; i < 4; i++) {
        glm::mat4 outlineMatrixTree = glm::scale(modelMatrixTrees[i], glm::vec3(1.03f));
        OutlineShader.setMat4("model", outlineMatrixTree);
        Tree.draw(OutlineShader);
    }

    // ----------------------------------------------------------------
    // (D) Reset State
    // ----------------------------------------------------------------
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glStencilMask(0xFF);
    glDisable(GL_STENCIL_TEST);
}

void Scene1::cleanup() {
    std::cout << "Cleaning up Scene1 resources..." << std::endl;
    if (LightingShader.getId() != 0)
        glDeleteProgram(LightingShader.getId());
    if (SkyboxShader.getId() != 0)
        glDeleteProgram(SkyboxShader.getId());
    if (OutlineShader.getId() != 0)
        glDeleteProgram(OutlineShader.getId());

    GardenPlant.cleanup();
    Tree.cleanup();
    Statue.cleanup();
    LSkybox.cleanup();
}