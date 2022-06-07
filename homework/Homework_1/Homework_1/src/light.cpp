
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
			// 把 type 作为 string stream 的 index，更清晰
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) // 第二个参数是因为 GLenum 是 unsigned int 的
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // 返回的是一个指向 string 内部的指针，因此这个 string 必须存在才有效
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); // iv: integer, vector
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// alloca 是C语言给你的一个函数，它让你在堆栈上动态地分配，根据你的判断来使用
		// 因为我们不想在堆上分配，所以调用了这个函数
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

// 从文件中获取，然后编译、链接，生成buffer id，用于之后的绑定
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// 查阅文档我们知道，验证的状态会被存储为程序对象状态的一部分你可以调用，比如用 glGetProgram 查询实际结果是什么之类的
	glValidateProgram(program);

	// 因为已经被链接到一个程序中了，所以如果你愿意可以删除中间部分
	// 在C++中编译东西的时候会得到诸如 .obj 这样的中间文件，shader 也是如此
	// 还有一些其他的函数，比如 glDetachShader 之类的它会删除源代码
	// 但是cherno不喜欢碰那些函数。
	// 首先清理这些也不是必要的，因为它占用的内存很少，但是保留着色器的源代码对于调试之类的是很重要的
	// 因此很多引擎根本不会去调用 glDetachShader 这些
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
	glGenBuffers(1, &VBO);//创建顶点缓冲对象
	GLuint VAO;
	    glGenVertexArrays(1, &VAO);//创建顶点数组对象
		glBindBuffer(GL_ARRAY_BUFFER, VBO);//将顶点缓冲绑定到顶点缓冲对象
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//缓冲区三角形顶点数据
		glBindVertexArray(VAO);//将顶点数组绑定到顶点数组对
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
		//设置点光源
		glUseProgram(lightCubeShader);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(lightCubeVAO);
		glDrawElements(GL_TRIANGLES,moon.getIndexCount(),GL_UNSIGNED_INT,(void*)0);
	
		//绘制正方形
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

		//点光源
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.position"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.diffuse"), 1, glm::value_ptr(diffuseColor));
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.ambient"), 1, glm::value_ptr(ambientColor));
		glUniform3fv(glGetUniformLocation(lightingShader, "pointLights.specular"), 1, glm::value_ptr(lightColor));
		glUniform1f(glGetUniformLocation(lightingShader, "pointLights.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader, "pointLights.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader, "pointLights.quadratic"), 0.032f);
	

		//平行光
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
