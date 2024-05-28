#define _USE_MATH_DEFINES
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <string>
#include <math.h>

#include  "ShaderProgram.h"
#include  "Camera.h"
#include "Plane.h"
#include "Kaczka.h"
#include "PathMaker.h"

#include <vector>

#include "3rd/stb_image/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

Camera camera = Camera();
int SCR_WIDTH = 800;
int SCR_HEIGHT = 800;
bool firstMouse = true;
bool firstMouse2 = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool mouse_mode = false;
bool moving_cursor = false;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float ROOM_WIDTH = 500.0f;
float ROOM_HEIGHT = 500.0f;

float MIRROR_ANGLE = glm::half_pi<float>() / 3.0f;
float MIRROR_WIDTH = 1.2f;
float CIRCLE_RADIUS = 0.4f;

float t = 0.0f;

typedef struct Material
{
	glm::vec3 ka, kd, ks;
	float shininess;
};

typedef struct Light
{
	glm::vec3 position;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "kaczuszka", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL Initial configuration
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	// MAIN CODE BLOCK
	// 
	// the code block is obligatory there because it ensures that that all objects 
	// will be destroed before OpenGL termination, so all OpenGL's resources such as 
	// buffers, programs and vertex arrays will be freed before termination.
	{

		// Create scene objects
		// *=*=*=*=*=*=*=*=*=*=*

		// Robot
		KACZKA::Kaczka duck("model/duck.txt");
		glm::vec3 robotColor(0.8f);

		PathMaker pathMaker = PathMaker();

		int width, height, nrChannels;
		unsigned char* data = stbi_load("model/ducktex.jpg", &width, &height, &nrChannels, 0);
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		// Room Planes
		PUMA::Plane roomPlanes[6] = {
			PUMA::Plane(ROOM_WIDTH, ROOM_HEIGHT), // floor 
			PUMA::Plane(ROOM_WIDTH, ROOM_HEIGHT), // ceiling 
			PUMA::Plane(ROOM_WIDTH, ROOM_HEIGHT), // back
			PUMA::Plane(ROOM_WIDTH, ROOM_HEIGHT), // front
			PUMA::Plane(ROOM_HEIGHT), // left 
			PUMA::Plane(ROOM_HEIGHT)  // right
		};
		glm::vec3 roomColor(0.702f, 0.702f, 0.396f);

		glm::mat4 roomPlaneModels[6] = { 
			// floor 
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, - ROOM_HEIGHT / 2.0f, 0.0f)),
			// ceiling
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,  ROOM_HEIGHT/ 2.f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)),
			// back
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -ROOM_HEIGHT / 2.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)),
			// front 
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, ROOM_HEIGHT / 2.0f))*
			glm::rotate(glm::mat4(1.0f), -glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)),
			// left 
			glm::translate(glm::mat4(1.0f), glm::vec3(-ROOM_WIDTH / 2.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), -glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)),
			// right
			glm::translate(glm::mat4(1.0f), glm::vec3(ROOM_WIDTH / 2.0f, 0.0f, 0.0f))*
			glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))
		};

		PUMA::Plane water(ROOM_WIDTH, ROOM_HEIGHT);
		glm::mat4 water_mtx = glm::mat4(1.0f);
		glm::vec3 water_color = glm::vec3(0.f, 1.0f, 1.0f);

		// Light
		Light sceneLight;
		sceneLight.position			= glm::vec3(-75.5f, 175.0f, 75.0f);
		sceneLight.diffuseColor		= glm::vec3(1.0f, 1.0f, 1.0f);
		sceneLight.specularColor	= glm::vec3(1.0f, 1.0f, 1.0f);

		glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);

		// Materials
		Material defaultMaterial;
		defaultMaterial.ka = glm::vec3(0.3f);
		defaultMaterial.kd = glm::vec3(1.0f);
		defaultMaterial.ks = glm::vec3(0.5f);
		defaultMaterial.shininess = 128.0f;

		// Create Shaders
		// *=*=*=*=*=*=*=*=*=*=*
		ShaderProgram KaczkaShader("kaczka.vert", "kaczka.frag");
		ShaderProgram PhongShader("phong.vert", "phong.frag");

		// *=*=*=*=*
		// MAIN LOOP
		// *=*=*=*=*
		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			processInput(window);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 view = camera.viewMatrix();
			glm::mat4 projection = camera.projectionMatrix(SCR_HEIGHT, SCR_WIDTH);

			t += deltaTime / 5.f;
			//t = std::fmod((t + deltaTime / 10.0f), 1.0f);
			duck.translation = pathMaker.calculateCurrentPosition(t);
			glm::vec3 tangent = pathMaker.bezierTangent(t);

			// Compute the rotation matrix to align the model
			glm::mat4 rotationMatrix = pathMaker.alignModelToVector(tangent);
			pathMaker.draw(view, projection);

			KaczkaShader.use();

			KaczkaShader.setVec3("cameraPos", camera.position);
			KaczkaShader.setMat4("view", view);
			KaczkaShader.setMat4("proj", projection);
			
			KaczkaShader.setVec3("ambientColor", ambientColor);
			KaczkaShader.setVec3("light.position", sceneLight.position);
			KaczkaShader.setVec3("light.diffuseColor", sceneLight.diffuseColor);
			KaczkaShader.setVec3("light.specularColor", sceneLight.specularColor);

			KaczkaShader.setVec3("material.ka", defaultMaterial.ka);
			KaczkaShader.setVec3("material.kd", defaultMaterial.kd);
			KaczkaShader.setVec3("material.ks", defaultMaterial.ks);
			KaczkaShader.setFloat("material.shininess", defaultMaterial.shininess);

			KaczkaShader.setVec3("objectColor", robotColor);
			KaczkaShader.setMat4("model", duck.ModelMtx() * rotationMatrix);
			
			duck.Draw(texture);

			PhongShader.use();

			PhongShader.setVec3("cameraPos", camera.position);
			PhongShader.setMat4("view", view);
			PhongShader.setMat4("proj", projection);

			PhongShader.setVec3("ambientColor", ambientColor);
			PhongShader.setVec3("light.position", sceneLight.position);
			PhongShader.setVec3("light.diffuseColor", sceneLight.diffuseColor);
			PhongShader.setVec3("light.specularColor", sceneLight.specularColor);

			PhongShader.setVec3("material.ka", defaultMaterial.ka);
			PhongShader.setVec3("material.kd", defaultMaterial.kd);
			PhongShader.setVec3("material.ks", defaultMaterial.ks);
			PhongShader.setFloat("material.shininess", defaultMaterial.shininess);

			PhongShader.setVec3("objectColor", water_color);
			PhongShader.setMat4("model", water_mtx);
			water.Draw();

			PhongShader.setVec3("objectColor", roomColor);
			for (int i = 0; i < 6; i++)
			{
				PhongShader.setMat4("model", roomPlaneModels[i]);
				roomPlanes[i].Draw();
			}

			glfwSwapBuffers(window);
			glfwPollEvents();

		} // MAIN LOOP


	} // MAIN CODE BLOCK

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.updatePosition(Forward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.updatePosition(Backward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.updatePosition(Left, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.updatePosition(Right, deltaTime);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mouse_mode = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouse_mode = false;
		firstMouse = true;
	}
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (mouse_mode) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;


		camera.processMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.radius -= yoffset * 5;
	if (camera.radius < 0.1) camera.radius = 0.1;
	camera.updateCameraVectors();
}