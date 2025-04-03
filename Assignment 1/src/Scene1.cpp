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
    // 1) Clear color, depth, and stencil buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glCullFace(GL_BACK);

    // ----------------------------------------------------------------
    // (A) Render the entire scene normally (colored pass)
    // ----------------------------------------------------------------
    LightingShader.use();
    //LightingShader.setInt("diffuseMap", 0);
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    LightingShader.setVec3("viewPos", GCamera.VPosition);
    LightingShader.setMaterial(material);
    LightingShader.setBool("useTexture", true);

    // Make sure texture unit 0 is active
    glActiveTexture(GL_TEXTURE0);
    // (Assume your Model class's draw() method binds the model's diffuse texture to unit 0)

    // Global translation for the scene (move 15 units along +Z)
    glm::mat4 globalTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 15.0f));
    glm::mat4 modelMatrix(1.0f);

    // Render Garden Plants
    for (int X = -5; X <= 5; X++) {
        for (int Z = -5; Z <= 5; Z++) {
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(X, 0.0f, Z * 0.8f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(PlantScaleFactor));
            modelMatrix = globalTranslation * modelMatrix;
            LightingShader.setMat4("model", modelMatrix);
            // Ensure texture unit is active before drawing
            glActiveTexture(GL_TEXTURE0);
            GardenPlant.draw(LightingShader);
        }
    }

    // Render Trees
    glm::vec3 treePositions[] = {
        {-6.0f, 0.0f, -5.0f}, { 6.0f, 0.0f, -5.0f},
        {-6.0f, 0.0f,  5.0f}, { 6.0f, 0.0f,  5.0f}
    };
    for (auto& pos : treePositions) {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
        modelMatrix = globalTranslation * modelMatrix;
        LightingShader.setMat4("model", modelMatrix);
        glActiveTexture(GL_TEXTURE0);
        Tree.draw(LightingShader);
    }

    // Render Statue (colored)
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(mStatueRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ModelScaleFactor));
    modelMatrix = globalTranslation * modelMatrix;
    LightingShader.setMat4("model", modelMatrix);
    glActiveTexture(GL_TEXTURE0);
    Statue.draw(LightingShader);
     
    GLightManager.updateLighting(LightingShader);

    // Render Skybox
    LSkybox.render(SkyboxShader, GCamera, 800, 600);

    // ----------------------------------------------------------------
    // (B) Stencil Update Pass: Write statue silhouette to stencil buffer
    // ----------------------------------------------------------------
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilMask(0xFF);

    // Disable color and depth writes, and disable depth test so all fragments are written
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    // Use the same transforms so the stencil exactly matches the statue
    LightingShader.use();
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    LightingShader.setMat4("model", modelMatrix);
    Statue.draw(LightingShader);

    // Restore color and depth writes, and re-enable depth test
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    // ----------------------------------------------------------------
    // (C) Outline Pass: Draw a blue outline where stencil != 1
    // ----------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_ALWAYS);

    OutlineShader.use();
    OutlineShader.setMat4("view", GCamera.getViewMatrix());
    OutlineShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    // Scale up the model matrix slightly for the outline effect
    glm::mat4 outlineMatrix = glm::scale(modelMatrix, glm::vec3(1.03f));
    OutlineShader.setMat4("model", outlineMatrix);
    OutlineShader.setVec3("outlineColor", glm::vec3(0.0f, 0.0f, 1.0f));
    Statue.draw(OutlineShader);

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
