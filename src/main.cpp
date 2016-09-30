// GLEngine by Joshua Senouf - 2016
// Credits to Joey de Vries (LearnOpenGL) and Kevin Fung (Glitter)


#include "shader.h"
#include "camera.h"
#include "model.h"
#include "basicshape.h"
#include "textureobject.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cameraMove();

const GLuint WIDTH = 1600;
const GLuint HEIGHT = 900;

bool keys[1024];

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;
bool firstMouse = true;


int main(int argc, char* argv[])
{
    cout << argv[0] << endl;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLEngine", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	gladLoadGL();

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);     


    Model suitModel("/home/jsenouf/workspace/projects/GLEngine/GLEngine/Models/nanosuit/nanosuit.obj");

    Shader lightingShader("/home/jsenouf/workspace/projects/GLEngine/GLEngine/Shaders/lighting.vs", "/home/jsenouf/workspace/projects/GLEngine/GLEngine/Shaders/lighting.frag");
    Shader lampShader("/home/jsenouf/workspace/projects/GLEngine/GLEngine/Shaders/lamp.vs", "/home/jsenouf/workspace/projects/GLEngine/GLEngine/Shaders/lamp.frag");

    BasicShape shape1("cube", glm::vec3(2.0f, -0.5f, 1.0f));
    shape1.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
    BasicShape shape2("cube", glm::vec3(-1.7f, 0.9f, 1.0f));
    shape2.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
    BasicShape shape3("cube", glm::vec3(0.0f, -0.5f, -1.5f));
    shape3.setScale(glm::vec3(0.3f, 0.3f, 0.3f));

    BasicShape shape4("plane", glm::vec3(0.0f, -1.0f, -0.0f));
    shape4.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    shape4.setDiffuseTexture("/home/jsenouf/workspace/projects/GLEngine/GLEngine/Textures/container2_diffuse.png");
    shape4.setSpecularTexture("/home/jsenouf/workspace/projects/GLEngine/GLEngine/Textures/container2_specular.png");

    glm::vec3 pointLightPositions[] = {
        shape1.getPosition(),
        shape2.getPosition(),
        shape3.getPosition()
    };


    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        cameraMove();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

        // SUIT COMPUTE
        lightingShader.Use();

        glm::mat4 projection = glm::perspective(camera.cameraFOV, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera.cameraPosition.x, camera.cameraPosition.y, camera.cameraPosition.z);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);		
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);		
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.009);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.0032);		

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);		
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);		
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.009);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0032);	

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);		
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);		
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.009);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0032);

        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -1.25f, 0.0f));
        GLfloat angle = glfwGetTime()/5.0f * 5.0f;
  		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        suitModel.Draw(lightingShader);

        shape1.drawShape(lampShader, view, projection, camera);
        shape2.drawShape(lampShader, view, projection, camera);
        shape3.drawShape(lampShader, view, projection, camera);
        shape4.drawShape(lightingShader, view, projection, camera);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}


void cameraMove()
{
	if (keys[GLFW_KEY_W])
		camera.keyboardCall(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.keyboardCall(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.keyboardCall(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.keyboardCall(RIGHT, deltaTime);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.mouseCall(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.scrollCall(yoffset);
}