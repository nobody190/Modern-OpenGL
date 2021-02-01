#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

void KeyPressed(GLFWwindow *window, glm::vec3 &position, glm::vec3& rotation, glm::vec3& scale) {
	/*Movement Events*/
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position.z -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position.z += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position.x += 0.01f;
	}
	/*Rotation Events*/
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		rotation.y -= 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		rotation.y += 1.0f;
	}
	/*Scale Events*/
}

int main() {
	GLFWwindow* window;

	/*Initialize the library*/
	if (!glfwInit())
		return -1;

	/*Set OpenGL verstion to 3.3*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/*Set OpenGL to Core Profile*/
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*Create windows*/
	const int width = 960;
	const int height = 480;

	int frameBufferWidth = 0;
	int frameBufferHeight = 0;

	window = glfwCreateWindow(width, height, "Shaders", NULL, NULL);

	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	/*Make the window context current*/
	glfwMakeContextCurrent(window);

	/*Limite frame rate*/
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	/*Create Scope For Classes*/
	{
		glCullFace(GL_FRONT);
		float size = 1, x = 0, y = 0, z = 0;

		float vertices[] = {
			/*Positions*/						
			x - size, y + size, z + size,			
			x - size, y + size, z - size,
			x + size, y + size, z + size,
			x + size, y + size, z - size,
			x - size, y - size, z + size,
			x - size, y - size, z - size,
			x + size, y - size, z + size,
			x + size, y - size, z - size
		};

		float normals[]{
			 0, 1, 0,
			 0, 0,-1,
			 1, 0, 0,
			 0,-1, 0,
			-1, 0, 0,
			 0, 0, 1
		};

		unsigned int indices[] = {
			0,1,2,
			2,1,3,

			3,1,5,
			5,7,3,

			3,7,2,
			2,7,6,

			6,7,4,
			4,7,5,

			5,1,4,
			4,1,0,

			1,3,6,
			6,5,0
		};


		/*Bind Vertex Array Object*/
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		VertexArray va;

		/*Bind Vertex Buffer*/
		VertexBuffer vb(vertices, 8 * 3 * sizeof(float));
		VertexBufferLayout layout;
			/*Position Coordinates*/
			layout.Push<float>(3);
			/*Normal Coordinates*/
			//layout.Push<float>(3);
		va.AddBuffer(vb, layout);


		/*Bind Index Buffer Object*/
		IndexBuffer ib(indices, 36);

		/*Lights*/
		glm::vec3 lightPos0(-2.0f, 0.0f, -2.0f);


		/*Object Status*/
		glm::vec3 position(0.0f);
		glm::vec3 rotation(0.0f);
		glm::vec3 scale(1.0f);

		glm::vec3 positionB(0.0f);
		glm::vec3 rotationB(0.0f);
		glm::vec3 scaleB(1.0f);

		/*View Matrix*/
		glm::vec3 camPos(0.0f, 0.0f, 4.0f);
		glm::vec3 camFront(0.0f, 0.0f, -1.0f);
		glm::vec3 wUp(0.0f, 1.0f, 0.0f);
		glm::mat4 view(1.0f);
		view = glm::lookAt(camPos, camPos + camFront, wUp);

		/*Projection Matrix*/
		float fov = 90.0f;
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;
		glm::mat4 proj(1.0f);

		proj = glm::perspective(
			glm::radians(fov), 
			static_cast<float>(frameBufferWidth/frameBufferHeight),
			nearPlane,
			farPlane
		);

		/*Model Matrix*/
		glm::mat4 model(1.0f);

		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);

		/*Define shader*/
		Shader phong("shaders/phong.shader");
		phong.Bind();
		phong.SetUniform3fv("u_Light0", lightPos0);
		phong.SetUniform3fv("u_Camera", camPos);
		phong.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		/*Reset previous binds*/
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		phong.Unbind();

		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		/*Render Scene*/
		while (!glfwWindowShouldClose(window)) {
			/*Input*/
			KeyPressed(window, position, rotation, scale);

			/*Render Scene*/
			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();

			/*Translations, Rotations, Scales*/
			position.x = 2;

			model = glm::mat4(1.0f);

			model = glm::translate(model, position);
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, scale);

			/*Update Projection Matrix*/
			glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

			proj = glm::mat4(1.0f);
			proj = glm::perspective(
				glm::radians(fov),
				static_cast<float>(frameBufferWidth / frameBufferHeight),
				nearPlane,
				farPlane
			);

			view = glm::lookAt(camPos, camPos + camFront, wUp);

			/*Bind shader and Vertex Array*/
			phong.Bind();
			phong.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

			phong.SetUniformMat4f("ModelMatrix", model);
			phong.SetUniformMat4f("ViewMatrix", view);
			phong.SetUniformMat4f("ProjectionMatrix", proj);

			renderer.Draw(va, ib, phong);

			/*Translations, Rotations, Scales*/
			position.x = -2;

			model = glm::mat4(1.0f);

			model = glm::translate(model, position);
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, scale);
			
			/*Gourad Shading*/
			phong.SetUniformMat4f("ModelMatrix", model);

			renderer.Draw(va, ib, phong);

			{
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/*Swap front and back buffers*/
			glfwSwapBuffers(window);

			/*Poll for and process events*/
			glfwPollEvents();
		}
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}