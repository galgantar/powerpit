#include "PCH.h"

#include "VertexArrayObject.h"
#include "SafeCall.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"


void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

float CalculateDeltaTime(float& prevTime)
{
    float newTime = glfwGetTime();
    float deltaTime = newTime - prevTime;
    prevTime = newTime;
    return deltaTime;
}

void ProcessInput(GLFWwindow* window, Camera* camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W))
        camera->MoveFront(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S))
        camera->MoveBack(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A))
        camera->MoveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D))
        camera->MoveRight(deltaTime);
}

void ProcessMouseInput(GLFWwindow* window, Camera* camera, float deltaTime, double& lastMouseX, double& lastMouseY)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    camera->Pitch((mouseY - lastMouseY) * -1.f, deltaTime); // y is reversed in glfw
    camera->Yaw(mouseX - lastMouseX, deltaTime);
    
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

int main()
{
    int screenWidth = 1000;
    int screenHeight = 800;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(screenWidth, screenHeight, "PowerPit", nullptr, nullptr);
    if (!window) {
        std::cout << "Window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize glad" << std::endl;
        return -1;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    Shader* shader = new Shader("shaders/VertexShader.shader", "shaders/FragmentShader.shader");
    Shader* lightShader = new Shader("shaders/LightSourceVertex.shader", "shaders/LightSourceFragment.shader");

    Texture* texture = new Texture("assets/textures/awesomeface.png", true);
    Texture* texture2 = new Texture("assets/textures/container.jpg", false);

    float modelVertices[] = {
        // Position              // Normals           // TexCoord
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
                                                       
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,     0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,     1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,     1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,     1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,     0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,     0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
                                                       
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
    };
    float plainVertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };



    gm::vec3 cubeTranslations[] = {
        gm::vec3(0.0f,  0.0f,  0.0f),
        gm::vec3(2.0f,  5.0f, -15.0f),
        gm::vec3(-1.5f, -2.2f, -2.5f),
        gm::vec3(-3.8f, -2.0f, -12.3f),
        gm::vec3(2.4f, -0.4f, -3.5f),
        gm::vec3(-1.7f,  3.0f, -7.5f),
        gm::vec3(1.3f, -2.0f, -2.5f),
        gm::vec3(1.5f,  2.0f, -2.5f),
        gm::vec3(1.5f,  0.2f, -1.5f),
        gm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // VAO has to be bound before VBO and EBO
    VertexArrayObject* VAO = new VertexArrayObject();
    VAO->Bind();
    unsigned int VBO;
    GLcall(glGenBuffers(1, &VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(modelVertices), modelVertices, GL_STATIC_DRAW));

    VAO->AddVertexAttribute(0, 3, GL_FLOAT, GL_FALSE);
    VAO->AddVertexAttribute(1, 3, GL_FLOAT, GL_FALSE);
    VAO->AddVertexAttribute(2, 2, GL_FLOAT, GL_FALSE);
    VAO->Build();
    VAO->Unbind();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    VertexArrayObject* lightVAO = new VertexArrayObject();
    unsigned int l_VBO;
    GLcall(glGenBuffers(1, &l_VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, l_VBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(plainVertices), plainVertices, GL_STATIC_DRAW));

    lightVAO->AddVertexAttribute(0, 3, GL_FLOAT, GL_FALSE);
    lightVAO->Build();
    lightVAO->Unbind();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader->Bind();
    shader->SetUniform3f("objectColor", { 1.0f, 0.5f, 0.31f });
    shader->SetUniform3f("lightColor", { 1.f, 1.f, 1.f });

    gm::vec3 lightPos(-1.7f, 0.0f, -1.f);

    shader->SetUniform3f("lightPos", lightPos); // uniforms are the same across shaders

    gm::mat4 light_model = gm::translate(gm::mat4(1.f), lightPos);
    light_model = gm::scale(light_model, gm::vec3(0.2f));

    Camera* camera = new Camera(gm::vec3(0.0f, 0.0f, 3.0f), gm::vec3(0.0f, 0.0f, -1.0f), gm::vec3(0.0f, 1.0f, 0.0f));

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    gm::mat4 projection = gm::perspective(gm::radians(60.0f), (float)width / height, 0.1f, 100.0f);
    shader->SetUniformMat4f("projection", projection);
    lightShader->SetUniformMat4f("projection", projection);


    float time = (float)glfwGetTime();
    double lastMouseX, lastMouseY;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    while (!glfwWindowShouldClose(window))
    {
        float deltaTime = CalculateDeltaTime(time);
        
        ProcessMouseInput(window, camera, deltaTime, lastMouseX, lastMouseY);
        ProcessInput(window, camera, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gm::mat4 view =  camera->GetViewMatrix();
        shader->SetUniformMat4f("view", view);
        shader->SetUniform3f("viewPos", camera->GetPosition());
        lightShader->SetUniformMat4f("view", view);

        // Draw
        lightShader->SetUniformMat4f("model", light_model);
        lightShader->Bind();
        lightVAO->Bind();
        GLcall(glDrawArrays(GL_TRIANGLES, 0, 36));

        VAO->Bind();
        for (int i = 0; i < 10; ++i) {
            gm::mat4 model = gm::mat4(1.0f);
            model = gm::translate(model, cubeTranslations[i]);
            float angle = 20.0f * i;
            model = gm::rotate(model, gm::vec3(1.0f, 0.3f, 0.5f), gm::radians(angle));
            
            shader->SetUniformMat4f("model", model);
            shader->Bind();
            GLcall(glDrawArrays(GL_TRIANGLES, 0, 36));
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    
    GLcall(glDeleteBuffers(1, &VBO));
    // GLcall(glDeleteBuffers(1, &EBO));

    // important for clean termination
    delete shader;
    delete texture;
    delete VAO;

    glfwTerminate();
    return 0;
}
