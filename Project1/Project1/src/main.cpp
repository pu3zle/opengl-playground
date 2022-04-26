//
//	Vasile FLUTUR (fluturv@yahoo.com)
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <iostream>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>
#include "vendor/sphere/Sphere.h"
#include "vendor/sphere/Cubesphere.h"

#define OFFSET_OF(TYPE, ATTRIB) (int)&((TYPE*)nullptr)->ATTRIB
#define SHADER_PATH "res/shaders/Basic.shader"
#define EARTH_TEXTURE "res/textures/2k_earth.jpg"
#define MOON_TEXTURE "res/textures/moon.jpg"
#define SUN_TEXTURE "res/textures/2k_sun.jpg"

//#define GL_SPHERE_SKELETON

// config
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float normalizedAngle(float angle)
{
    while (angle > 360)
        angle = angle - 360;
    return angle;
}

glm::mat4 GetPlanetModel(float rotationAngle, float scale, glm::vec3& translation, glm::vec3 rotationMatrix = glm::vec3(0.0f, 0.0f, 1.0f), glm::mat4 baseModel = glm::mat4(1.0f))
{
    glm::mat4 model = baseModel;

    model = glm::scale(model, glm::vec3(scale, scale, scale));

    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(rotationAngle), rotationMatrix);

    model = glm::translate(model, translation);

    return model;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Playground", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // syncronize with monitor refresh rate
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << '\n';
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Sphere sphere;

    // VAO
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    VertexArray va;
    VertexBuffer vb(sphere.getInterleavedVertices(), sphere.getInterleavedVertexSize());
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    // IBO
    IndexBuffer ib(sphere.getIndices(), sphere.getIndexCount());
    Shader shader(SHADER_PATH);
    Renderer renderer;

#ifndef GL_SPHERE_SKELETON
    Texture2D earth(EARTH_TEXTURE, false);
    Texture2D moon(MOON_TEXTURE, true);
    Texture2D sun(SUN_TEXTURE, false);

    earth.Bind(0);
    sun.Bind(1);
    moon.Bind(2);


#endif

    const float step = 1.0f;
    float rotationAngle = 0.0f;

    float earthScale = 0.4f;
    float moonScale = 0.2f;
    float sunScale = 0.6f;

    glm::vec3 earthTranslation = glm::vec3(5.0f, 0.0f, 0.0f);
    glm::vec3 moonTranslation = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 sunTranslation = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::mat4 mvp = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 projection = glm::perspective(glm::radians(-45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    while (!glfwWindowShouldClose(window))
    {
        renderer.Clear(); 
        shader.Bind();

        rotationAngle += step;

        // earth
        glm::mat4 earthModel = GetPlanetModel(normalizedAngle(rotationAngle * 0.25), earthScale, earthTranslation);
        mvp = projection * view * earthModel;
        
        shader.SetUniformMat4f("u_MVP", mvp);

#ifndef GL_SPHERE_SKELETON
        shader.SetUniform1i("u_Texture", 0);
#else
        shader.SetUniform4f("u_Color", 0.8f, 0.2f, 0.4f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
        renderer.Draw(va, ib, shader);

        // sun
        glm::mat4 sunModel = GetPlanetModel(normalizedAngle(rotationAngle * 0.15), sunScale, sunTranslation);
        mvp = projection * view * sunModel;

        shader.SetUniformMat4f("u_MVP", mvp);

#ifndef GL_SPHERE_SKELETON
        shader.SetUniform1i("u_Texture", 1);
#else
        shader.SetUniform4f("u_Color", 0.8f, 0.2f, 0.4f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
        renderer.Draw(va, ib, shader);
        
        // moon
        glm::mat4 moonModel = GetPlanetModel(normalizedAngle(rotationAngle * 1.5), moonScale, moonTranslation, glm::vec3(0.0f, 0.0f, 1.0f), earthModel);
        mvp = projection * view * moonModel;

        shader.SetUniformMat4f("u_MVP", mvp);

#ifndef GL_SPHERE_SKELETON
        shader.SetUniform1i("u_Texture", 2);
#else
        shader.SetUniform4f("u_Color", 0.8f, 0.2f, 0.4f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
        renderer.Draw(va, ib, shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

