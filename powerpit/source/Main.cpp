#include "PCH.h"

#include "VertexArrayObject.h"
#include "SafeCall.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"

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
    int screenWidth = 1200;
    int screenHeight = 1000;

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

    Camera* camera = new Camera(gm::vec3(0.0f, 0.0f, 3.0f), gm::vec3(0.0f, 1.0f, 0.0f));

    Model* backpack = new Model("assets/models/backpack/backpack.obj");

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
    unsigned int l_VAO, l_VBO, l_IBO;
    GLcall(glGenVertexArrays(1, &l_VAO));
    GLcall(glBindVertexArray(l_VAO));
    GLcall(glGenBuffers(1, &l_VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, l_VBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(plainVertices), plainVertices, GL_STATIC_DRAW));
    GLcall(glEnableVertexAttribArray(0));
    GLcall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLcall(glBindVertexArray(0));



    // Directional light
    shader->SetUniform3f("dirLight.direction", { 0.f, -1.f, 0.f });
    shader->SetUniform3f("dirLight.ambient", { 0.05f, 0.05f, 0.05f });
    shader->SetUniform3f("dirLight.diffuse", { 0.4f, 0.4f, 0.4f });
    shader->SetUniform3f("dirLight.specular", { 0.5f, 0.5f, 0.5f });

    // Spotligt
    /*shader->SetUniform3f("spotLight.ambient", { 0.8f, 0.8f, 0.8f });
    shader->SetUniform3f("spotLight.diffuse", { 0.4f, 0.4f, 0.4f });
    shader->SetUniform3f("spotLight.specular", { 0.5f, 0.5f, 0.5f });
    shader->SetUniform3f("spotLight.position", camera->GetPosition());
    shader->SetUniform3f("spotLight.direction", camera->GetFront());
    shader->SetUniform1f("spotLight.innerCutoff", gm::cos(gm::radians(12.5f)));
    shader->SetUniform1f("spotLight.outerCutoff", gm::cos(gm::radians(17.5f)));
    shader->SetUniform1f("spotLight.quadratic_val", 0.032f);
    shader->SetUniform1f("spotLight.linear_val", 0.09f);
    shader->SetUniform1f("spotLight.constant_val", 1.0f);*/

   // Point lights
    for (int i = 0; i < 4; ++i) {
        std::string lightIndex = "pointLights[" + std::to_string(i) + "]";
        shader->SetUniform3f(lightIndex + ".position", {-2.f, 0.f, 5.f * i});
        shader->SetUniform3f(lightIndex + ".ambient", { 0.05f, 0.05f, 0.05f });
        shader->SetUniform3f(lightIndex + ".diffuse", { 0.9f, 0.9f, 0.9f });
        shader->SetUniform3f(lightIndex + ".specular", { 1.0f, 1.0f, 1.0f });
        shader->SetUniform1f(lightIndex + ".quadratic_val", 0.032f);
        shader->SetUniform1f(lightIndex + ".linear_val", 0.09f);
        shader->SetUniform1f(lightIndex + ".constant_val", 1.0f);
    }


    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float time = (float)glfwGetTime();
    double lastMouseX, lastMouseY;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    gm::mat4 projection = gm::perspective(gm::radians(60.0f), (float)width / height, 0.1f, 100.0f);

    float moveLight = 0;
    bool up = true;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float deltaTime = CalculateDeltaTime(time);
        
        ProcessMouseInput(window, camera, deltaTime, lastMouseX, lastMouseY);
        ProcessInput(window, camera, deltaTime);

        gm::mat4 view = camera->GetViewMatrix();

        shader->SetUniform3f("spotLight.position", camera->GetPosition());
        shader->SetUniform3f("spotLight.direction", camera->GetFront());

        shader->SetUniformMat4f("view", view);
        shader->SetUniformMat4f("projection", projection);

        // Draw lights
        lightShader->SetUniformMat4f("projection", projection);
        lightShader->SetUniformMat4f("view", view);
        lightShader->Bind();

        if (up)
            moveLight += 0.01f;
        else
            moveLight -= 0.01f;
        if (moveLight < 0 || moveLight > 4.f)
            up = !up;

        GLcall(glBindVertexArray(l_VAO));
        for (int i = 0; i < 4; ++i) {
            std::string lightIndex = "pointLights[" + std::to_string(i) + "]";
            shader->SetUniform3f(lightIndex + ".position", { moveLight - 2.f, 0.f, 5.f * i });
            
            gm::mat4 l_model = gm::translate(gm::mat4(1.f), { moveLight -2.f, 0.f, 5.f * i });
            l_model = gm::scale(l_model, { 0.2f });
            lightShader->SetUniformMat4f("model", l_model);
            lightShader->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        lightShader->Unbind();
        GLcall(glBindVertexArray(0));

        // Draw models
        for (int i = 0; i < 10; ++i) {
            gm::mat4 test_model = gm::translate(gm::mat4(1.f), { 0.f, 0.f, 4.f * i });
            shader->SetUniformMat4f("model", test_model);
            backpack->Draw(*shader);
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    
    delete shader;
    delete lightShader;
    delete camera;
    delete backpack;
    
    glfwTerminate();
    return 0;
}
