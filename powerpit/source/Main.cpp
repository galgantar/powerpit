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
    glDepthFunc(GL_LEQUAL); // objects in front rendered on top
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

    Shader* shader = new Shader("shaders/VertexShader.shader", "shaders/FragmentShader.shader", "shaders/DecomposeGeometry.shader");
    Shader* lightShader = new Shader("shaders/LightSourceVertex.shader", "shaders/LightSourceFragment.shader", "shaders/Geometry.shader");
    Shader* outlineShader = new Shader("shaders/VertexShader.shader", "shaders/PlainColor.shader");
    Shader* quadShader = new Shader("shaders/PostprocessingVertex.shader", "shaders/PostprocessingFragment.shader");
    Shader* skyboxShader = new Shader("shaders/SkyboxVertex.shader", "shaders/SkyboxFragment.shader");
    Shader* reflectionShader = new Shader("shaders/ENVmapVertex.shader", "shaders/ReflectionFragment.shader");
    Shader* refractionShader = new Shader("shaders/ENVmapVertex.shader", "shaders/RefractionFragment.shader");

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
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int l_VAO, l_VBO, l_IBO;
    GLcall(glGenVertexArrays(1, &l_VAO));
    GLcall(glBindVertexArray(l_VAO));
    GLcall(glGenBuffers(1, &l_VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, l_VBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW));
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

    
    // cube map
    unsigned int skybox;
    glGenTextures(1, &skybox);
    GLcall(glBindTexture(GL_TEXTURE_CUBE_MAP, skybox));
    
    std::string faces[] = { 
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    
    int i_width, i_height, i_nrChannels;
    for (int i = 0; i < 6; ++i) {
        unsigned char* data = SOIL_load_image((std::string("assets/textures/skybox/") + faces[i]).c_str(), &i_width, &i_height, &i_nrChannels, 0);
        if (data) {
            GLcall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, i_width, i_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        }
        else {
            std::cout << "sth went wrong " << (std::string("assets/textures/skybox/") + faces[i]).c_str() << std::endl;
        }
        SOIL_free_image_data(data);
    }


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    unsigned int VAO, VBO;
    GLcall(glGenVertexArrays(1, &VAO));
    GLcall(glBindVertexArray(VAO));
    GLcall(glGenBuffers(1, &VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(plainVertices), plainVertices, GL_STATIC_DRAW));

    GLcall(glEnableVertexAttribArray(0));
    GLcall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0));
    GLcall(glBindVertexArray(0));

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float time = (float)glfwGetTime();


    double lastMouseX, lastMouseY;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    gm::mat4 projection = gm::perspective(gm::radians(60.0f), (float)width / height, 0.1f, 100.0f);


    // uniform buffer object
    unsigned int uboMatrix;
    glGenBuffers(1, &uboMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrix);
    glBufferData(GL_UNIFORM_BUFFER, 2 * 64, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrix, 0, 2 * 64); // allocate entire buffer to binding point 0
    
    gm::mat4 t_projection = gm::transpose(projection);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, gm::value_ptr(t_projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    unsigned int uniformBlockIndex = glGetUniformBlockIndex(lightShader->GetID(), "Matrices");
    glUniformBlockBinding(lightShader->GetID(), uniformBlockIndex, 0); // bind uniform block to binding point 0


    while (!glfwWindowShouldClose(window))
    {
        // glClearColor(0.1f, 0.1f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        

        float deltaTime = CalculateDeltaTime(time);
        
        shader->SetUniform1f("deltaTime", (float)glfwGetTime());

        ProcessMouseInput(window, camera, deltaTime, lastMouseX, lastMouseY);
        ProcessInput(window, camera, deltaTime);

        
        gm::mat4 view = camera->GetViewMatrix();
        gm::mat4 t_view = gm::transpose(view);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrix);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, gm::value_ptr(t_view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        shader->SetUniform3f("spotLight.position", camera->GetPosition());
        shader->SetUniform3f("spotLight.direction", camera->GetFront());

        reflectionShader->SetUniform3f("cameraPos", camera->GetPosition());
        refractionShader->SetUniform3f("cameraPos", camera->GetPosition());

        // Draw lights
        lightShader->Bind();

        GLcall(glBindVertexArray(l_VAO));
        for (int i = 0; i < 4; ++i) {
            std::string lightIndex = "pointLights[" + std::to_string(i) + "]";
            shader->SetUniform3f(lightIndex + ".position", { -2.f, 0.f, 5.f * i });
            
            gm::mat4 l_model = gm::translate(gm::mat4(1.f), { -2.f, 0.f, 5.f * i });
            l_model = gm::scale(l_model, { 0.3f });
            lightShader->SetUniformMat4f("model", l_model);
            lightShader->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        lightShader->Unbind();
        GLcall(glBindVertexArray(0));

        GLcall(glBindTexture(GL_TEXTURE_CUBE_MAP, skybox));

        // Draw models
        for (int i = 0; i < 10; ++i) {
            gm::mat4 test_model = gm::translate(gm::mat4(1.f), { 0.f, 0.f, 4.f * i });
            shader->SetUniformMat4f("model", test_model);
            backpack->Draw(*shader);
        }

        // Draw skybox
        gm::mat4 sky_view = camera->GetViewMatrix();
        sky_view[3][0] = 0.f;
        sky_view[3][1] = 0.f;
        sky_view[3][2] = 0.f;
        sky_view[0][3] = 0.f;
        sky_view[1][3] = 0.f;
        sky_view[2][3] = 0.f;
        skyboxShader->SetUniformMat4f("sky_view", sky_view);
        GLcall(glBindVertexArray(VAO));
        GLcall(glBindTexture(GL_TEXTURE_CUBE_MAP, skybox));
        skyboxShader->Bind();
        GLcall(glDrawArrays(GL_TRIANGLES, 0, 36));


        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    
    delete shader;
    delete lightShader;
    delete outlineShader;
    delete camera;
    delete backpack;
    delete skyboxShader;
    delete reflectionShader;
    delete refractionShader;

    glfwTerminate();
    return 0;
}
