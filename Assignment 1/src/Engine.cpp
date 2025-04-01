#include "Engine.h"
#include <iostream>

Engine::Engine(GLFWwindow* win)
    : window(win),
    camera(glm::vec3(0.0f, 5.0f, 30.0f)),
    lightManager(),
    sceneManager(camera, lightManager),
    inputManager(camera, lightManager, sceneManager)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the GLFW user pointer to this Engine instance so we can access it in callbacks
    glfwSetWindowUserPointer(window, this);

    // Set the cursor position callback
    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(win));
        engine->inputManager.mouseCallback(win, xpos, ypos);
        });

    // Set the scroll callback
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(win));
        engine->inputManager.scrollCallback(win, xoffset, yoffset);
        });

    // Set the framebuffer size callback
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(win));
        engine->inputManager.frameBufferSizeCallback(win, width, height);
        });
}

void Engine::run() {
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        inputManager.processInput(window, deltaTime);
        sceneManager.update(deltaTime);
        sceneManager.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    sceneManager.cleanup();
}
