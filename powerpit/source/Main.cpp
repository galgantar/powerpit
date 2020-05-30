#include "PCH.h"


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


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // objects in front rendered on top
    glEnable(GL_STENCIL_TEST);
    /*
    usless for now:
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    */

    
    //face culling messes with the 3d models
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW);

    Shader* shader = new Shader("shaders/VertexShader.shader", "shaders/FragmentShader.shader");
    Shader* lightShader = new Shader("shaders/LightSourceVertex.shader", "shaders/LightSourceFragment.shader");
    Shader* outlineShader = new Shader("shaders/VertexShader.shader", "shaders/PlainColor.shader");
    Shader* quadShader = new Shader("shaders/PostprocessingVertex.shader", "shaders/PostprocessingFragment.shader");

    Camera* camera = new Camera(gm::vec3(0.0f, 0.0f, 3.0f), gm::vec3(0.0f, 1.0f, 0.0f));

    Model* backpack = new Model("assets/models/backpack/backpack.obj");

    float plainVertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,   
         0.5f,  0.5f, -0.5f,          
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,              
        // front face
        -0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,       
         0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,   
        // left face
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        // right face
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,         
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        // bottom face      
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,   
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // top face
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

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int qVAO, qVBO;
    GLcall(glGenVertexArrays(1, &qVAO));
    GLcall(glBindVertexArray(qVAO));
    GLcall(glGenBuffers(1, &qVBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, qVBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW));
    GLcall(glEnableVertexAttribArray(0));
    GLcall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0));
    GLcall(glEnableVertexAttribArray(1));
    GLcall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8));


    int width, height;
    glfwGetWindowSize(window, &width, &height);



    // off screen rendering
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int texture;
    glGenTextures(1, &texture);
    GLcall(glBindTexture(GL_TEXTURE_2D, texture));
    GLcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
    GLcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));
    GLcall(glBindTexture(GL_TEXTURE_2D, 0));

    
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "FRAMEBUFFER INCOMPLETE" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    

    float time = (float)glfwGetTime();
    double lastMouseX, lastMouseY;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    gm::mat4 projection = gm::perspective(gm::radians(60.0f), (float)width / height, 0.1f, 100.0f);


    while (!glfwWindowShouldClose(window))
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        

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

        GLcall(glBindVertexArray(l_VAO));
        for (int i = 0; i < 4; ++i) {
            std::string lightIndex = "pointLights[" + std::to_string(i) + "]";
            shader->SetUniform3f(lightIndex + ".position", {-2.f, 0.f, 5.f * i });
            
            gm::mat4 l_model = gm::translate(gm::mat4(1.f), { -2.f, 0.f, 5.f * i });
            l_model = gm::scale(l_model, { 0.7f });
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
        

        // quad rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(qVAO);
        GLcall(glActiveTexture(GL_TEXTURE0));
        glBindTexture(GL_TEXTURE_2D, texture);
        quadShader->SetUniform1i("quad", 0);
        quadShader->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadShader->Unbind();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    
    delete shader;
    delete lightShader;
    delete outlineShader;
    delete camera;
    delete backpack;
    
    glDeleteFramebuffers(1, &FBO);
    
    glfwTerminate();
    return 0;
}
