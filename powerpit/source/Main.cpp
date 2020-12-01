 #include "PCH.h"


#include "SafeCall.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Primitives.h"
#include "Skybox.h"
#include "Renderer.h"
#include "Buffer.h"


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
        camera->MoveFront(deltaTime * 10);
    if (glfwGetKey(window, GLFW_KEY_S))
        camera->MoveBack(deltaTime * 10);
    if (glfwGetKey(window, GLFW_KEY_A))
        camera->MoveLeft(deltaTime * 10);
    if (glfwGetKey(window, GLFW_KEY_D))
        camera->MoveRight(deltaTime * 10);
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
    glfwWindowHint(GLFW_SAMPLES, 4); // Multi Sampling Anti Aliasing

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
    glEnable(GL_MULTISAMPLE);
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


    Shader basicShader = Shader("shaders/VertexShader.shader", "shaders/FragmentShader.shader");
    // Shader lightShader = Shader("shaders/LightSourceVertex.shader", "shaders/LightSourceFragment.shader", "shaders/Geometry.shader");
    Shader reflectionShader = Shader("shaders/ENVmap/ENVmapVertex.shader", "shaders/ENVmap/ReflectionFragment.shader");
    Shader plainShader = Shader("shaders/VertexShader.shader", "shaders/PlainColor.shader");
    Shader instanceShader = Shader("shaders/BatchRenderingModelVertex.shader", "shaders/TestFragment.shader");

    Camera camera = Camera(gm::vec3(0.0f, 0.0f, 3.0f), gm::vec3(0.0f, 1.0f, 0.0f));

    Model backpack = Model("assets/models/backpack/backpack.obj");
    
    // cube map
    Skybox skybox(Texture("assets/textures/skybox/", GL_TEXTURE_CUBE_MAP, false), Shader("shaders/skybox/SkyboxVertex.shader", "shaders/skybox/SkyboxFragment.shader"), Mesh(Cube()));

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float time = (float)glfwGetTime();

    double lastMouseX, lastMouseY;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

    // Uniform buffer (currently only one, so no linking is needed)
    gm::mat4 projection = gm::perspective(gm::radians(60.0f), (float)width / height, 0.1f, 100.0f);
    UniformBuffer& matrices = UniformBuffer(0, 2 * sizeof(gm::mat4)); // ADD COPY CONSTRUCTOR LATER
    matrices.Bind();
    matrices.SetMat4f(0, gm::transpose(projection));

    Mesh cube = Mesh(Cube());
    Texture gold_texture = Texture("assets/textures/block.png", GL_TEXTURE_2D, true);
    size_t cubes = 75;

    std::vector<gm::mat4> model_matrices(cubes * cubes * cubes);
    for (int i = 0; i < cubes; ++i)
        for (int j = 0; j < cubes; ++j)
            for (int k = 0; k < cubes; ++k) {
                gm::mat4 mat = gm::translate(gm::mat4(1.f),  { (float)i * 3, (float)j * 3, (float)k * 3 });
                model_matrices[i + j * cubes + k * cubes * cubes] = gm::transpose(mat);
            }

    Buffer batch_locations = Renderer::Get().SetUpInstancedTransformations(cube, model_matrices);
    

    // imgui params
    std::string drawType = "normal";
    gm::mat4 model(1.f);
    model = gm::scale(model, { 0.3f });
    model = gm::translate(model, { -6.f, 2.f, 1.f });

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        float deltaTime = CalculateDeltaTime(time);

        ProcessMouseInput(window, &camera, deltaTime, lastMouseX, lastMouseY);
        ProcessInput(window, &camera, deltaTime);

        gm::mat4 view = camera.GetViewMatrix();
        matrices.SetMat4f(64, gm::transpose(view));


        // DRAW
        if (drawType == "reflections")
        {
            // backpack
            reflectionShader.SetUniformMat4f("model", model);
            reflectionShader.SetUniform3f("cameraPos", camera.GetPosition());
            reflectionShader.SetUniform1i("skybox", 0);
            Renderer::Get().Draw(backpack, reflectionShader);

            // Draw skybox
            skybox.SetViewMatrix(view);
            skybox.Draw();
        }

        else if (drawType == "plainColor")
        {
            // backpack
            plainShader.SetUniformMat4f("model", model);
            plainShader.SetUniform3f("color", { 0.f, 1.f, 0.f });
            Renderer::Get().Draw(backpack, plainShader);

            // Draw skybox
            skybox.SetViewMatrix(view);
            skybox.Draw();
        }

        else if (drawType == "normal")
        {
            // backpack
            basicShader.SetUniformMat4f("model", model);
            Renderer::Get().Draw(backpack, basicShader);

            // Bash rendering
            gold_texture.Bind(0);
            Renderer::Get().DrawInstanced(cube, instanceShader, pow(75, 3));

            // Draw skybox
            skybox.SetViewMatrix(view);
            skybox.Draw();
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
