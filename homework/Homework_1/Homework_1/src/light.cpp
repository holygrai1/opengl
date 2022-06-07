
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <string>
#include "Sphere.h"
#include <sstream> // stringstream

using namespace std;

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			// �� type ��Ϊ string stream �� index��������
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) // �ڶ�����������Ϊ GLenum �� unsigned int ��
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // ���ص���һ��ָ�� string �ڲ���ָ�룬������ string ������ڲ���Ч
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); // iv: integer, vector
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// alloca ��C���Ը����һ���������������ڶ�ջ�϶�̬�ط��䣬��������ж���ʹ��
		// ��Ϊ���ǲ����ڶ��Ϸ��䣬���Ե������������
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

// ���ļ��л�ȡ��Ȼ����롢���ӣ�����buffer id������֮��İ�
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// �����ĵ�����֪������֤��״̬�ᱻ�洢Ϊ�������״̬��һ��������Ե��ã������� glGetProgram ��ѯʵ�ʽ����ʲô֮���
	glValidateProgram(program);

	// ��Ϊ�Ѿ������ӵ�һ���������ˣ����������Ը�����ɾ���м䲿��
	// ��C++�б��붫����ʱ���õ����� .obj �������м��ļ���shader Ҳ�����
	// ����һЩ�����ĺ��������� glDetachShader ֮�������ɾ��Դ����
	// ����cherno��ϲ������Щ������
	// ����������ЩҲ���Ǳ�Ҫ�ģ���Ϊ��ռ�õ��ڴ���٣����Ǳ�����ɫ����Դ������ڵ���֮����Ǻ���Ҫ��
	// ��˺ܶ������������ȥ���� glDetachShader ��Щ
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
//static unsigned int* get_indexArr( ) {
//	unsigned int indexArr[6];
//	for (int i = 0; i < 6; i++) {
//		for (int j = 0; j < 3; j++) {
//			indexArr[i] = i + j * 2;
//		}
//	}
//	return  indexArr;
//};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);
	ShaderProgramSource source1 = ParseShader("res/shaders/light.shader");
	ShaderProgramSource source2 = ParseShader("res/shaders/lightcube.shader");
	unsigned int lightingShader = CreateShader(source1.VertexSource, source1.FragmentSource);
	unsigned int lightCubeShader = CreateShader(source2.VertexSource, source2.FragmentSource);


	float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

	GLuint VBO;
	glGenBuffers(1, &VBO);//�������㻺�����
	GLuint VAO;
	    glGenVertexArrays(1, &VAO);//���������������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);//�����㻺��󶨵����㻺�����
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//�����������ζ�������
		glBindVertexArray(VAO);//����������󶨵����������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		GLuint lightCubeVAO;
		GLuint lightCubeVBO;
		GLuint lightCubeEBO;
		Sphere moon(0.1f, 36, 18);
		int stride ;
		stride = moon.getInterleavedStride();
		glGenVertexArrays(1, &lightCubeVAO);
		glGenBuffers(1, &lightCubeVBO);
		glGenBuffers(1, &lightCubeEBO);
		glBindVertexArray(lightCubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightCubeEBO);
		glBufferData(GL_ARRAY_BUFFER, moon.getInterleavedVertexSize(), moon.getInterleavedVertices(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, moon.getIndexSize(), moon.getIndices(), GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::vec3 lightPos(-0.5f, 0.5f, 1.0f);
		glm::vec3 viewPos(0.0f, 0.0f, 3.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	    glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		//���õ��Դ
		glUseProgram(lightCubeShader);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(lightCubeVAO);
		glDrawElements(GL_TRIANGLES,moon.getIndexCount(),GL_UNSIGNED_INT,(void*)0);
	
		//����������
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		glUseProgram(lightingShader);
	    model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.2f, 0.7f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(lightingShader, "viewPos"), 1, glm::value_ptr(viewPos));
		glm::vec3 ma_ambient(0.8f, 0.5f, 0.31f);
		glm::vec3 ma_diffuse(0.8f, 0.5f, 0.31f);
		glm::vec3 ma_specular(0.8f, 0.8f, 0.8f);
		glUniform1f(glGetUniformLocation(lightingShader, "material.shininess"), 32.0f);
		glUniform3fv(glGetUniformLocation(lightingShader, "material.ambient"), 1, glm::value_ptr(ma_ambient));
		glUniform3fv(glGetUniformLocation(lightingShader, "material.diffuse"), 1, glm::value_ptr(ma_diffuse));
		glUniform3fv(glGetUniformLocation(lightingShader, "material.specular"), 1, glm::value_ptr(ma_specular));

		//���Դ
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.position"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.diffuse"), 1, glm::value_ptr(diffuseColor));
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.ambient"), 1, glm::value_ptr(ambientColor));
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.specular"), 1, glm::value_ptr(lightColor));
		glUniform1f(glGetUniformLocation(lightingShader, "pointLights.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader, "pointLights.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader, "pointLights.quadratic"), 0.032f);
	

		//ƽ�й�
		 glm::vec3 dr_direction(-0.5f, -0.5f, -0.5f);
		 glm::vec3 dr_ambient(0.05f, 0.05f, 0.05f);
		 glm::vec3 dr_diffuse(0.4f, 0.4f, 0.4f);
		 glm::vec3 dr_specular(0.5f, 0.5f, 0.5f);
		 glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.direction"), 1, glm::value_ptr(dr_direction));
		 glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.ambient"), 1, glm::value_ptr(dr_ambient));
		 glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.diffuse"), 1, glm::value_ptr(dr_diffuse));
		 glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.specular"), 1, glm::value_ptr(dr_specular));


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
