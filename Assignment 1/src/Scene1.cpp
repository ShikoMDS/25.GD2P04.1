#include "Scene1.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

// Debug helper (if desired)
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
    // Clear all buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Global translation for the scene
    glm::mat4 globalTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 15.0f));

    // PASS 1: Draw Skybox first
    // ----------------------------------
    glDisable(GL_STENCIL_TEST);  // Ensure stencil test is disabled for skybox
    // Render Skybox
    LSkybox.render(SkyboxShader, GCamera, 800, 600);

    // PASS 2: Draw the normal scene
    // ----------------------------------
    LightingShader.use();
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    LightingShader.setVec3("viewPos", GCamera.VPosition);
    LightingShader.setMaterial(material);
    LightingShader.setBool("useTexture", true);
    GLightManager.updateLighting(LightingShader);

    // Render garden plants
    for (int X = -5; X <= 5; X++) {
        for (int Z = -5; Z <= 5; Z++) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(X, 0.0f, Z * 0.8f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(PlantScaleFactor));
            modelMatrix = globalTranslation * modelMatrix;
            LightingShader.setMat4("model", modelMatrix);
            glActiveTexture(GL_TEXTURE0);
            GardenPlant.draw(LightingShader);
        }
    }

    // Render Trees (except the one we'll outline)
    glm::vec3 treePositions[] = {
        {-6.0f, 0.0f, -5.0f}, { 6.0f, 0.0f, -5.0f},
        {-6.0f, 0.0f,  5.0f}, { 6.0f, 0.0f,  5.0f}
    };

    // Draw all trees except the first one (which we'll outline)
    for (int i = 1; i < 4; i++) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, treePositions[i]);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
        modelMatrix = globalTranslation * modelMatrix;
        LightingShader.setMat4("model", modelMatrix);
        glActiveTexture(GL_TEXTURE0);
        Tree.draw(LightingShader);
    }

    // PASS 3: Stencil Pass - fill stencil buffer with objects to outline
    // ----------------------------------
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);  // Always pass stencil test, write 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // Replace stencil value on depth pass
    glStencilMask(0xFF);  // Enable writing to stencil buffer
    glDepthMask(GL_TRUE);  // Keep writing to depth buffer
    glClear(GL_STENCIL_BUFFER_BIT);  // Clear stencil to 0

    // Draw statue to stencil buffer AND color buffer
    LightingShader.use();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(mStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
    modelMatrix = globalTranslation * modelMatrix;
    LightingShader.setMat4("model", modelMatrix);
    Statue.draw(LightingShader);

    // Draw the first tree to stencil buffer AND color buffer
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, treePositions[0]);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
    modelMatrix = globalTranslation * modelMatrix;
    LightingShader.setMat4("model", modelMatrix);
    Tree.draw(LightingShader);

    // PASS 4: Draw outlines
    // ----------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);  // Only draw where stencil is NOT 1
    glStencilMask(0x00);  // Disable writing to stencil buffer
    glDepthFunc(GL_LEQUAL);  // Less or equal depth test (for outlines to appear on edges)

    OutlineShader.use();
    OutlineShader.setMat4("view", GCamera.getViewMatrix());
    OutlineShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    OutlineShader.setVec3("outlineColor", glm::vec3(0.0f, 0.0f, 1.0f));

    // Draw statue outline
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(mStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor * 1.05f));  // Slightly larger
    modelMatrix = globalTranslation * modelMatrix;
    OutlineShader.setMat4("model", modelMatrix);
    Statue.draw(OutlineShader);

    // Draw tree outline
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, treePositions[0]);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor * 1.05f));  // Slightly larger
    modelMatrix = globalTranslation * modelMatrix;
    OutlineShader.setMat4("model", modelMatrix);
    Tree.draw(OutlineShader);

    // Reset state
    glDepthFunc(GL_LESS);  // Reset to default depth function
    glStencilMask(0xFF);   // Reset stencil mask
    glDisable(GL_STENCIL_TEST);  // Disable stencil testing
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
