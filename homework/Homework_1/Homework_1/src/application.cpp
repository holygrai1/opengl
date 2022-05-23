
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

	//创建着色器程序
	GLfloat vertices[] = {
	-1.0f,0.0f,0.0f,//第一个顶点
	-0.9f,0.0f,0.0f,
	-0.8f,0.0f,0.0f,
	-0.7f,0.0f,0.0f,
	-0.6f,0.0f,0.0f,
	-0.5f,0.0f,0.0f,
	-0.4f,0.0f,0.0f,
	-0.3f,0.0f,0.0f,
	-0.2f,0.0f,0.0f,
	-0.1f,0.0f,0.0f,



	-0.9f,0.1f,0.0f,//第二个顶点
	-0.8f,0.1f,0.0f,
	-0.7f,0.1f,0.0f,
	-0.6f,0.1f,0.0f,
	-0.5f,0.1f,0.0f,
	-0.4f,0.1f,0.0f,
	-0.3f,0.1f,0.0f,
	-0.2f,0.1f,0.0f,
	-0.1f,0.1f,0.0f,
	-0.0f,0.1f,0.0f,



	-0.9f,-0.1f,0.0f,//第三个顶点
	-0.8f,-0.1f,0.0f,
	-0.7f,-0.1f,0.0f,
	-0.6f,-0.1f,0.0f,
	-0.5f,-0.1f,0.0f,
	-0.4f,-0.1f,0.0f,
	-0.3f,-0.1f,0.0f,
	-0.2f,-0.1f,0.0f,
	-0.1f,-0.1f,0.0f,
	-0.0f,-0.1f,0.0f,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);//创建顶点缓冲对象
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//将顶点缓冲绑定到顶点缓冲对象
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//缓冲区三角形顶点数据
	
	GLuint VAO[10],EBO[10];
	glGenVertexArrays(10, VAO);//创建顶点数组对象
	glGenBuffers(10, EBO);//创建顶点缓冲对象
	for (int i = 0; i < 10; i++) {
		unsigned int indices[3];
		for (int j = 0; j < 3; j++) {
			indices[j] = i + 10 * j;
		}
		glBindVertexArray(VAO[i]);//将顶点数组绑定到顶点数组对象
		//设置顶点属性指针
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);





	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		int flagLoc = glGetUniformLocation(shader, "flag");
		for (int i = 0; i < 10; i++) {
			float r = (i / 10.0f)+0.1f;
			glBindVertexArray(VAO[i]);
			glUniform4f(flagLoc, 0.1f, r, 0.0f, 1.0f);
			/*	glDrawElements(GL_TRIANGLES, 0, 3)
				glUniform1i(flagLoc, 1);*/
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)0);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(10, VAO);
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
