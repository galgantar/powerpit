#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "SafeCall.h"
#include "Shader.h"
#include "Texture.h"

#include <iostream>

void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(800, 600, "PowerPit", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);


    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "Error [Main.cpp::main] GLEW_INIT_FAILED" << std::endl;
        glfwTerminate();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Shader* shader = new Shader("shaders/VertexShader.shader", "shaders/FragmentShader.shader");
    Texture* texture = new Texture("assets/textures/awesomeface.png", GL_RGBA);
    Texture* texture2 = new Texture("assets/textures/container.jpg", GL_RGB);

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // VAO has to bi bound before VBO and EBO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    unsigned int VBO;
    GLcall(glGenBuffers(1, &VBO));
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLcall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // stride (distance to next attribute of the same type), void * offset (distance from the begginnig to current attribute)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    shader->Bind();
    texture->Bind(0);
    texture2->Bind(1);
    shader->SetUniform1i("smiley", 0);
    shader->SetUniform1i("box", 1);

    glm::mat4 trans(1.f);
    trans = glm::scale(trans, glm::vec3(1.f, 2.f, 1.f));
    
    shader->SetUniformMat4f("transform", trans);

    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        trans = glm::rotate(trans, glm::radians(0.01f), glm::vec3(0.0, 0.0, 1.0));
        shader->SetUniformMat4f("transform", trans);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    
    GLcall(glDeleteVertexArrays(1, &VAO));
    GLcall(glDeleteBuffers(1, &VBO));
    GLcall(glDeleteBuffers(1, &EBO));

    // important for clean termination
    delete shader;
    delete texture;

    glfwTerminate();
    return 0;
}
