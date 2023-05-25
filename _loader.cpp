#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "importer/shader.h"
#include "importer/camera.h"
#include "importer/scene.h"
#include "importer/animator.h"

#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int render(int x_pos, float angle) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for macOS


    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "crow", NULL, NULL);
    if (window == NULL) {
        std::cout << "GLFW window error" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, x_pos, 0);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD error" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("model_anim/model_anim.vs", "model_anim/model_anim.fs");
    Scene crow("/Users/null_pointer/stepikcpp/crow/crow.glb");



    Animation anim("/Users/null_pointer/stepikcpp/crow/crow.glb", &crow);
    Animator animator(&anim);
    while (!glfwWindowShouldClose(window))
    {
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        animator.UpdateAnimation(deltaTime);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


        glm::mat4 view =  glm::lookAt(glm::vec3(23.23f, -26.8f, 67.85f), glm::vec3(23.23f, -26.8f, 67.85f) + glm::vec3(-0.425f, -0.472f, -0.77f ), glm::vec3(-0.228f, 0.881f, -0.413f)); // //
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        auto & transforms = animator.GetTransforms(); // there was a memory problem
        for (int i = 0; i < transforms.size(); ++i)
            shader.setMat4("transforms[" + std::to_string(i) + "]", transforms[i]);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        crow.Draw(shader);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glfwTerminate();
    return 0;
}



int main()
{   


    pid_t c_pid = fork();

    if (c_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (c_pid > 0)
    {
        render(0 ,0.0f);
    }
    else
    {
        render(600, 0.5f);
    }

    return 0;
}
