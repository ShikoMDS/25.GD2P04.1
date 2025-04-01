#pragma once

enum class SceneType {
    SCENE_1,
    SCENE_2,
    SCENE_3,
    SCENE_4
};

class Scene {
public:
    virtual void load() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    virtual ~Scene() = default;
};
