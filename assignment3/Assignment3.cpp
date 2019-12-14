#include "assignment3/Assignment3.h"
#include "common/core.h" // <-- haha.
#include "common/Utility/Mesh/Simple/PrimitiveCreator.h"
#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include <cmath>

Assignment3::Assignment3(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera):
    Application(std::move(inputScene), std::move(inputCamera))
{
}

std::unique_ptr<Application> Assignment3::CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera)
{
    return make_unique<Assignment3>(std::move(scene), std::move(camera));
}

std::shared_ptr<class Camera> Assignment3::CreateCamera()
{
    // Specify any old aspect ratio for now, we'll update it later once the window gets made!
    // Read more about Field of View: http://rg3.name/201210281829.html!
    // Note that our field of view is the VERTICAL field of view (in degrees).
    return std::make_shared<PerspectiveCamera>(75.f, 1280.f / 720.f);
}

glm::vec2 Assignment3::GetWindowSize() const
{
    return glm::vec2(1280.f, 720.f);
}

void Assignment3::SetupScene()
{
    SetupExample1();
}

void Assignment3::SetupCamera()
{
    camera->Translate(glm::vec3(0.f, 0.f, 10.f));
}

void Assignment3::HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime)
{   
    if (state != SDL_KEYDOWN) {
        return;
    }

    switch (key.sym) {
    case SDLK_1:
        if (!repeat && state == SDL_KEYDOWN) {
            SetupExample1();
        }
        break;
    case SDLK_2:
        if (!repeat && state == SDL_KEYDOWN) {
            SetupExample2();
        }
        break;
    case SDLK_UP:
        sceneObject->Rotate(glm::vec3(SceneObject::GetWorldRight()), -0.1f);
        break;
    case SDLK_DOWN:
        sceneObject->Rotate(glm::vec3(SceneObject::GetWorldRight()), 0.1f);
        break;
    case SDLK_RIGHT:
        sceneObject->Rotate(glm::vec3(SceneObject::GetWorldUp()), 0.1f);
        break;
    case SDLK_LEFT:
        sceneObject->Rotate(glm::vec3(SceneObject::GetWorldUp()), -0.1f);
        break;
    case SDLK_w:
        camera->Translate(glm::vec3(SceneObject::GetWorldForward() * -0.3f));
        break;
    case SDLK_a:
        camera->Translate(glm::vec3(SceneObject::GetWorldRight() * 0.3f));
        break;
    case SDLK_s:
        camera->Translate(glm::vec3(SceneObject::GetWorldForward() * 0.3f));
        break;
    case SDLK_d:
        camera->Translate(glm::vec3(SceneObject::GetWorldRight() * -0.3f));
        break;
    case SDLK_SPACE:
        camera->Translate(glm::vec3(SceneObject::GetWorldUp() * -0.3f));
        break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        camera->Translate(glm::vec3(SceneObject::GetWorldUp() * 0.3f));
        break;
    case SDLK_EQUALS:
        sceneObject->AddScale(0.1f);
        break;
    case SDLK_MINUS:
        sceneObject->AddScale(-0.1f);
        break;
    default:
        Application::HandleInput(key, state, repeat, timestamp, deltaTime);
        break;
    }
}

void Assignment3::HandleWindowResize(float x, float y)
{
    Application::HandleWindowResize(x, y);
    std::static_pointer_cast<PerspectiveCamera>(camera)->SetAspectRatio(x / y);
}

void Assignment3::SetupExample1()
{
    scene->ClearScene();
#ifndef DISABLE_OPENGL_SUBROUTINES
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/frag/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/frag/blinnphong.frag"}
    };
#else
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/frag/noSubroutine/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/frag/noSubroutine/blinnphong.frag"}
    };
#endif
    // std::shared_ptr 是 c++11 引入的，它會自動計算有多少個變數共用這個 pointer，
    // 當沒有任何變數使用這個 pointer 時會自動釋放記憶體
    std::shared_ptr<BlinnPhongShader> shader = std::make_shared<BlinnPhongShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetDiffuse(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
    shader->SetAmbient(glm::vec4(0.5f));

    std::shared_ptr<RenderingObject> sphereTemplate = PrimitiveCreator::CreateIcoSphere(shader, 5.f, 4);

    // Give a R/G/B color to each vertex to visualize the sphere.
    auto totalVertices = sphereTemplate->GetTotalVertices();

    // std::unique_ptr 代表這個 pointer 無法被多個變數共用（也不能複製）
    // 也就是說連直接當作函數的引數也無法（因為這也是複製 pointer）
    // 只能透過 std::move 這個函數更改擁有權
    std::unique_ptr<RenderingObject::ColorArray> vertexColors = make_unique<RenderingObject::ColorArray>();
    vertexColors->reserve(totalVertices);

    for (decltype(totalVertices) i = 0; i < totalVertices; ++i) {
        // vector::emplace_back() 作用結果和 push_back 結果相同
        // emplace_back，直接調用 constructor 建立物件並加到 vector 中（所以 emplace_back 傳入的是 constructor 的引數）
        // push_back，利用 copy constructor 把已建立好的物件加到 vector 中（所以 push_back 傳入的是物件）
        vertexColors->emplace_back(0.5f, 0.5f, 0.5f, 1.f);
    }
    sphereTemplate->SetVertexColors(std::move(vertexColors));

    // std::make_shared 就是將 unique_ptr 轉成 shared_ptr
    sceneObject = std::make_shared<SceneObject>(sphereTemplate);
    scene->AddSceneObject(sceneObject);

    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties->diffuseColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);

    pointLight = std::make_shared<Light>(std::move(lightProperties));
    pointLight->SetPosition(glm::vec3(0.f, 0.f, 10.f));
    scene->AddLight(pointLight);
}

void Assignment3::SetupExample2()
{
    scene->ClearScene();
#ifndef DISABLE_OPENGL_SUBROUTINES
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/frag/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/frag/blinnphong.frag"}
    };
#else
    std::unordered_map<GLenum, std::string> shaderSpec = {
        { GL_VERTEX_SHADER, "brdf/blinnphong/frag/noSubroutine/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/frag/noSubroutine/blinnphong.frag"}
    };
#endif
    std::shared_ptr<BlinnPhongShader> shader = std::make_shared<BlinnPhongShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetDiffuse(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
    shader->SetAmbient(glm::vec4(0.5f));

    std::vector<std::shared_ptr<RenderingObject>> meshTemplate = MeshLoader::LoadMesh(shader, "outlander/Model/Outlander_Model.obj");
    if (meshTemplate.empty()) {
        std::cerr << "ERROR: Failed to load the model. Check your paths." << std::endl;
        return;
    }

    sceneObject = std::make_shared<SceneObject>(meshTemplate);
    scene->AddSceneObject(sceneObject);

    std::unique_ptr<LightProperties> lightProperties = make_unique<LightProperties>();
    lightProperties->diffuseColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);

    pointLight = std::make_shared<Light>(std::move(lightProperties));
    pointLight->SetPosition(glm::vec3(0.f, 0.f, 10.f));
    scene->AddLight(pointLight);
}

void Assignment3::Tick(double deltaTime)
{
}
