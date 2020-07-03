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
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); 
    //glFrontFace(GL_CCW);

    Shader* modelShader = new Shader("shaders/VertexShader.shader", "shaders/FragmentShader.shader", 0);
    Shader* lightShader = new Shader("shaders/LightSourceVertex.shader", "shaders/LightSourceFragment.shader", "shaders/Geometry.shader");
    Shader* outlineShader = new Shader("shaders/VertexShader.shader", "shaders/PlainColor.shader");
    Shader* quadShader = new Shader("shaders/PostprocessingVertex.shader", "shaders/PostprocessingFragment.shader");
    Shader* skyboxShader = new Shader("shaders/SkyboxVertex.shader", "shaders/SkyboxFragment.shader");
    Shader* reflectionShader = new Shader("shaders/ENVmapVertex.shader", "shaders/ReflectionFragment.shader");
    Shader* refractionShader = new Shader("shaders/ENVmapVertex.shader", "shaders/RefractionFragment.shader");
    Shader* plainShader = new Shader("shaders/VertexShader.shader", "shaders/PlainColor.shader");
    Shader* instanceShader = new Shader("shaders/BatchRenderingModelVertex.shader", "shaders/TestFragment.shader");

    Camera* camera = new Camera(gm::vec3(0.0f, 0.0f, 3.0f), gm::vec3(0.0f, 1.0f, 0.0f));

    Texture* grass = new Texture("assets/textures/grass.png", true, "");

    // Model* backpack = new Model("assets/models/backpack/backpack.obj");
    Model* planet = new Model("assets/models/planet/planet.obj");
    Model* rock = new Model("assets/models/rock/rock.obj");

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

    
    float vertices[] = {
         0.5f,  0.5f,  0.0f, 
         0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
        -0.5f,  0.5f,  0.0f,
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int planeVAO, VBO, EBO;
    GLcall(glGenVertexArrays(1, &planeVAO));
    GLcall(glBindVertexArray(planeVAO));
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    GLcall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));

    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GLcall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    
    glEnableVertexAttribArray(0);
    GLcall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0));
    //glEnableVertexAttribArray(1);
    //GLcall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, (void*)12));

    glBindVertexArray(0);

    // Directional light
    modelShader->SetUniform3f("dirLight.direction", { 0.f, -1.f, 0.f });
    modelShader->SetUniform3f("dirLight.ambient", { 0.05f, 0.05f, 0.05f });
    modelShader->SetUniform3f("dirLight.diffuse", { 0.4f, 0.4f, 0.4f });
    modelShader->SetUniform3f("dirLight.specular", { 0.5f, 0.5f, 0.5f });

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
        modelShader->SetUniform3f(lightIndex + ".position", {-2.f, 0.f, 5.f * i});
        modelShader->SetUniform3f(lightIndex + ".ambient", { 0.05f, 0.05f, 0.05f });
        modelShader->SetUniform3f(lightIndex + ".diffuse", { 0.9f, 0.9f, 0.9f });
        modelShader->SetUniform3f(lightIndex + ".specular", { 1.0f, 1.0f, 1.0f });
        modelShader->SetUniform1f(lightIndex + ".quadratic_val", 0.032f);
        modelShader->SetUniform1f(lightIndex + ".linear_val", 0.09f);
        modelShader->SetUniform1f(lightIndex + ".constant_val", 1.0f);
    }

    // asteroid field data
    size_t size = 10000;
    gm::mat4* model_matrices = new gm::mat4[size];
    srand(glfwGetTime());
    float radius = 50.0;
    float offset = 10.f;
    for (unsigned int i = 0; i < size; i++)
    {
        gm::mat4 model = gm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)size * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = gm::translate(model, gm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = gm::scale(model, gm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = gm::rotate(model, gm::vec3(0.4f, 0.6f, 0.8f), rotAngle);

        // 4. now add to list of matrices
        model_matrices[i] = gm::transpose(model);
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size * 64, model_matrices, GL_STATIC_DRAW);

    for (unsigned int i = 0; i < rock->GetMeshes().size(); i++)
    {
        unsigned int VAO = rock->GetMeshes()[i].GetVAO();
        glBindVertexArray(VAO);
        // vertex attributes
        std::size_t vec4Size = 16;

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
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

    // skybox
    unsigned int sky_VAO, sky_VBO;
    GLcall(glGenVertexArrays(1, &sky_VAO));
    GLcall(glBindVertexArray(sky_VAO));
    GLcall(glGenBuffers(1, &sky_VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, sky_VBO));
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        

        float deltaTime = CalculateDeltaTime(time);

        ProcessMouseInput(window, camera, deltaTime, lastMouseX, lastMouseY);
        ProcessInput(window, camera, deltaTime);

        
        gm::mat4 view = camera->GetViewMatrix();
        gm::mat4 t_view = gm::transpose(view);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrix);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, gm::value_ptr(t_view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        // Draw stuff
        
        // quad
        gm::mat4 q_model(1.f);
        q_model = gm::translate(q_model, { 0.f, -2.f, 0.f });
        q_model = gm::rotate(q_model, { 1.f, 0.f, 0.f }, gm::radians(90.f));
        q_model = gm::scale(q_model, { 10.f });
        plainShader->SetUniformMat4f("model", q_model);
        plainShader->SetUniform3f("color", { 0.f, 1.f, 0.f });
        GLcall(glBindVertexArray(planeVAO));
        plainShader->Bind();
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        // planet
        gm::mat4 planet_model(1.f);
        planet_model = gm::scale(planet_model, { 0.3f });
        planet_model = gm::translate(planet_model, { 0.f, 2.f, 1.f });
        modelShader->SetUniformMat4f("model", planet_model);
        planet->Draw(*modelShader);

        // asteroid field

        instanceShader->SetUniform1i("material.diffuseMap", 0);
        instanceShader->Bind();
        for (unsigned int i = 0; i < rock->GetMeshes().size(); i++)
        {
            glBindVertexArray(rock->GetMeshes()[i].GetVAO());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rock->GetMeshes()[i].GetTextures()[0].GetID());
            glDrawElementsInstanced(GL_TRIANGLES, rock->GetMeshes()[i].GetIndices().size(), GL_UNSIGNED_INT, 0, size);
        }


        // grass
        gm::mat4 g_model(1.f);
        g_model = gm::translate(g_model, { 0.f, -1.f, 0.f });
        plainShader->SetUniformMat4f("model", g_model);
        plainShader->SetUniform3f("color", { 1.f, 0.f, 0.f });
        GLcall(glBindVertexArray(planeVAO));
        plainShader->Bind();
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


        // Draw skybox
        gm::mat4 sky_view = view;
        sky_view[3][0] = 0.f;
        sky_view[3][1] = 0.f;
        sky_view[3][2] = 0.f;
        sky_view[0][3] = 0.f;
        sky_view[1][3] = 0.f;
        sky_view[2][3] = 0.f;
        skyboxShader->SetUniformMat4f("sky_view", sky_view);
        GLcall(glBindVertexArray(sky_VAO));
        GLcall(glBindTexture(GL_TEXTURE_CUBE_MAP, skybox));
        skyboxShader->Bind();
        GLcall(glDrawArrays(GL_TRIANGLES, 0, 36));


        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    
    delete modelShader;
    delete lightShader;
    delete outlineShader;
    delete camera;
    delete skyboxShader;
    delete reflectionShader;
    delete refractionShader;
    delete grass;

    glfwTerminate();
    return 0;
}
