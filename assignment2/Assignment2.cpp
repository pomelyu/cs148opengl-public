#include "assignment2/Assignment2.h"
#include "common/core.h" // <-- haha.
#include "common/Utility/Mesh/Simple/PrimitiveCreator.h"
#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>

const GLuint shaderFromFile(const std::string &fileName, GLenum shaderType)
{
    std::ifstream fs(fileName, std::ifstream::in);

    if (!fs.is_open()) {
        std::cerr << "ERROR: Could not read shader from " << fileName << std::endl;
        return 0;
    }
    
    // This reads in the whole file into an std::string. Google for more information!
    std::string shaderText((std::istreambuf_iterator<char>(fs)),
                            std::istreambuf_iterator<char>());

    // 因為 shaderText 指向 buffer 的 copy
    // 而 shaderText.c_str()，最後回傳指向 buffer 的 pointer，
    // 因此在函式呼叫結束後資料可能會被清掉
    // 也就是說如果在函式外呼叫 shaderText.c_str() 可能會讀取到隨機的資料，
    // 導致後續 opengl compile 上出現隨機的錯誤
    // 因此這邊需要把 shader 的檔案讀取和 compile 放在同一個函數進行
    // 參考：https://stackoverflow.com/questions/11138705/is-there-anything-wrong-with-my-glsl-shader-loading-code
    const char* shaderString = shaderText.c_str();
    const GLuint shaderObject = glCreateShader(shaderType);
    glShaderSource(shaderObject, 1, &shaderString, NULL);
    glCompileShader(shaderObject);

    return shaderObject;
}

namespace
{
const int SHADER_ERROR_LOG_SIZE = 500;
bool VerifyShaderCompile(GLuint newShaderObject)
{
    GLint compileStatus;
    OGL_CALL(glGetShaderiv(newShaderObject, GL_COMPILE_STATUS, &compileStatus));
    if (compileStatus != GL_TRUE) {
        char errorLogBuffer[SHADER_ERROR_LOG_SIZE];
        OGL_CALL(glGetShaderInfoLog(newShaderObject, SHADER_ERROR_LOG_SIZE, NULL, errorLogBuffer));
        std::cerr << "ERROR: Shader compilation failure -- " << std::endl << errorLogBuffer << std::endl;
        return false;
    }
    return true;
}

bool VerifyProgramLink(GLuint shaderProgram)
{
    GLint linkStatus;
    OGL_CALL(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus));
    if (linkStatus != GL_TRUE) {
        char errorLogBuffer[SHADER_ERROR_LOG_SIZE];
        OGL_CALL(glGetProgramInfoLog(shaderProgram, SHADER_ERROR_LOG_SIZE, NULL, errorLogBuffer));
        std::cerr << "ERROR: Program link compilation failure -- " << std::endl << errorLogBuffer << std::endl;
        return false;
    }
    return true;
}
}

Assignment2::Assignment2(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera):
    Application(std::move(inputScene), std::move(inputCamera))
{
    vertexPositions = 
        std::initializer_list<glm::vec4>({
            // Triangle 1
            {0.f, 0.f, 0.f, 1.f},
            {1.f, 0.f, 0.f, 1.f},
            {1.f, 1.f, 0.f, 1.f},
            // Triangle 2
            {0.f, 0.f, 0.f, 1.f},
            {-1.f, 1.f, 0.f, 1.f},
            {-1.f, 0.f, 0.f, 1.f}
        });
}

std::unique_ptr<Application> Assignment2::CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera)
{
    return make_unique<Assignment2>(std::move(scene), std::move(camera));
}

std::shared_ptr<class Camera> Assignment2::CreateCamera()
{
    // Specify any old aspect ratio for now, we'll update it later once the window gets made!
    // Read more about Field of View: http://rg3.name/201210281829.html!
    // Note that our field of view is the VERTICAL field of view (in degrees).
    return std::make_shared<PerspectiveCamera>(75.f, 1280.f / 720.f);
}

glm::vec2 Assignment2::GetWindowSize() const
{
    return glm::vec2(1280.f, 720.f);
}

void Assignment2::SetupScene()
{
    SetupExample1();
}

void Assignment2::SetupCamera()
{
}

void Assignment2::HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime)
{   
    Application::HandleInput(key, state, repeat, timestamp, deltaTime);
}

void Assignment2::HandleWindowResize(float x, float y)
{
    Application::HandleWindowResize(x, y);
}

void Assignment2::SetupExample1()
{
    // Insert "Load and Compile Shaders" code here.
    const std::string vertShaderFile = std::string(STRINGIFY(SHADER_PATH)) + "/hw2/hw2.vert";
    const std::string fragShaderFile = std::string(STRINGIFY(SHADER_PATH)) + "/hw2/hw2.frag";

    const GLuint vertObject = shaderFromFile(vertShaderFile, GL_VERTEX_SHADER);
    const GLuint fragObject = shaderFromFile(fragShaderFile, GL_FRAGMENT_SHADER);

    programObject = glCreateProgram();
    glAttachShader(programObject, vertObject);
    glAttachShader(programObject, fragObject);

    glLinkProgram(programObject);

    // Checkpoint 1.
    // Modify this part to contain your vertex shader ID, fragment shader ID, and shader program ID.
    const GLuint vertexShaderId = vertObject;
    const GLuint fragmentShaderId = fragObject;
    const GLuint shaderProgramId = programObject;

    // DO NOT EDIT OR REMOVE THE CODE IN THIS SECTION
    if (!VerifyShaderCompile(vertexShaderId) || !VerifyShaderCompile(fragmentShaderId) || !VerifyProgramLink(shaderProgramId)) {
        std::cout << "FAILURE: Checkpoint 1 failed." << std::endl;
    } else {
        std::cout << "SUCCESS: Checkpoint 1 completed." << std::endl;
    }

    OGL_CALL(glDetachShader(shaderProgramId, vertexShaderId));
    OGL_CALL(glDeleteShader(vertexShaderId));
    OGL_CALL(glDetachShader(shaderProgramId, fragmentShaderId));
    OGL_CALL(glDeleteShader(fragmentShaderId));
    // FINISH DO NOT EDIT OR REMOVE THE CODE IN THIS SECTION

    // Insert "Setup Buffers" code here.
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertexPositions.size(), &vertexPositions[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void Assignment2::Tick(double deltaTime)
{
    // Insert "Send Buffers to the GPU" and "Slightly-More Advanced Shaders" code here.
    glUseProgram(programObject);
    glBindVertexArray(vertexArray);
    glDrawArrays(GL_TRIANGLES, 0, vertexPositions.size());
}
