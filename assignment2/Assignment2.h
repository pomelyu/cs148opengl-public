#pragma once

#ifndef __ASSIGNMENT_2__
#define __ASSIGNMENT_2__

#include "common/Application.h"

class Assignment2: public Application
{
public:
    Assignment2(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera);

    static std::unique_ptr<Application> CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera);
    static std::shared_ptr<class Camera> CreateCamera();

    virtual glm::vec2 GetWindowSize() const;

    virtual void HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime);
    virtual void Tick(double deltaTime);

    GLuint programObject = 0;
    GLuint vao = 0;
    GLuint glTime = 0;
    float time = 0;
    
protected:
    virtual void HandleWindowResize(float x, float y);

private:
    virtual void SetupScene();

    virtual void SetupExample1();

    virtual void SetupCamera();

    std::shared_ptr<class SceneObject> sceneObject;

    std::shared_ptr<class Light> pointLight;

    std::vector<glm::vec4> vertexPositions;
};

#endif
