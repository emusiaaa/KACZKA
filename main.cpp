#define _USE_MATH_DEFINES
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

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
#include "WaterDisturber.h"

#include <vector>

#include "3rd/stb_image/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void generateTexture(unsigned int& texture, const char* path);
void loadCubemap(unsigned int& texture, std::vector<std::string> faces);

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
		WaterDisturber waterDisturber = WaterDisturber();

		unsigned int duckTexture, waterNormalsTexture, waterTEX, cubeMap;
		generateTexture(duckTexture, "model/ducktex.jpg");
		generateTexture(waterNormalsTexture, "model/normal2.png");
		glGenTextures(1, &waterTEX);
		glBindTexture(GL_TEXTURE_2D, waterTEX);

		/*std::vector<std::string> faces
		{
			"model/skybox2/right.png",
			"model/skybox2/left.png",
			"model/skybox2/top.png",
			"model/skybox2/bottom.png",
			"model/skybox2/front.png",
			"model/skybox2/back.png"
		};*/
		std::vector<std::string> faces
		{
			"model/skybox/right.jpg",
			"model/skybox/left.jpg",
			"model/skybox/top.jpg",
			"model/skybox/bottom.jpg",
			"model/skybox/front.jpg",
			"model/skybox/back.jpg"
		};
		loadCubemap(cubeMap, faces);


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
		glm::vec3 water_color = glm::vec3(0.f, .41f, .58f);

		// Light
		Light sceneLight;
		sceneLight.position			= glm::vec3(-75.5f, 35.0f, 75.0f);
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
		ShaderProgram WaterShader("water.vert", "water.frag");
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

			t += deltaTime / 3.f;
			duck.translation = pathMaker.calculateCurrentPosition(t);
			glm::vec3 tangent = pathMaker.bezierTangent(t);
			glm::mat4 rotationMatrix = pathMaker.alignModelToVector(tangent);

			int ii = static_cast<int>(((duck.translation.x + 250.f) / 500.0f) * 255.0f);
			int jj = static_cast<int>(((-duck.translation.z + 250.f) / 500.0f) * 255.0f);
			waterDisturber.disturb(jj, ii, waterTEX);

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

			PhongShader.setVec3("objectColor", roomColor);
			glEnable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);
			for (int i = 0; i < 6; i++)
			{
				PhongShader.setMat4("model", roomPlaneModels[i] * Scale(250));
				roomPlanes[i].Draw(cubeMap);
			}
			glDepthMask(GL_TRUE);

			glDisable(GL_CULL_FACE);
			//pathMaker.draw(view, projection);

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
			
			duck.Draw(duckTexture);

			WaterShader.use();

			WaterShader.setVec3("cameraPos", camera.position);
			WaterShader.setMat4("view", view);
			WaterShader.setMat4("proj", projection);

			WaterShader.setVec3("ambientColor", ambientColor);
			WaterShader.setVec3("light.position", sceneLight.position);
			WaterShader.setVec3("light.diffuseColor", sceneLight.diffuseColor);
			WaterShader.setVec3("light.specularColor", sceneLight.specularColor);

			WaterShader.setVec3("material.ka", defaultMaterial.ka);
			WaterShader.setVec3("material.kd", defaultMaterial.kd);
			WaterShader.setVec3("material.ks", defaultMaterial.ks);
			WaterShader.setFloat("material.shininess", defaultMaterial.shininess);

			WaterShader.setVec3("objectColor", water_color);
			WaterShader.setMat4("model", water_mtx * Scale(250));
			//KaczkaShader.use();
			//KaczkaShader.setVec3("objectColor", water_color);
			//KaczkaShader.setMat4("model", water_mtx);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			water.Draw(waterTEX, GL_TEXTURE_2D);

			

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

void generateTexture(unsigned int& texture, const char* path) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
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
	glBindTexture(GL_TEXTURE_2D, 0);
}

void loadCubemap(unsigned int& texture, std::vector<std::string> faces)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}