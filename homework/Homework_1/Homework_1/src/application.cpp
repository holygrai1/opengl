
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

	//������ɫ������
	GLfloat vertices[] = {
	-1.0f,0.0f,0.0f,//��һ������
	-0.9f,0.0f,0.0f,
	-0.8f,0.0f,0.0f,
	-0.7f,0.0f,0.0f,
	-0.6f,0.0f,0.0f,
	-0.5f,0.0f,0.0f,
	-0.4f,0.0f,0.0f,
	-0.3f,0.0f,0.0f,
	-0.2f,0.0f,0.0f,
	-0.1f,0.0f,0.0f,



	-0.9f,0.1f,0.0f,//�ڶ�������
	-0.8f,0.1f,0.0f,
	-0.7f,0.1f,0.0f,
	-0.6f,0.1f,0.0f,
	-0.5f,0.1f,0.0f,
	-0.4f,0.1f,0.0f,
	-0.3f,0.1f,0.0f,
	-0.2f,0.1f,0.0f,
	-0.1f,0.1f,0.0f,
	-0.0f,0.1f,0.0f,



	-0.9f,-0.1f,0.0f,//����������
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
	glGenBuffers(1, &VBO);//�������㻺�����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//�����㻺��󶨵����㻺�����
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//�����������ζ�������
	
	GLuint VAO[10],EBO[10];
	glGenVertexArrays(10, VAO);//���������������
	glGenBuffers(10, EBO);//�������㻺�����
	for (int i = 0; i < 10; i++) {
		unsigned int indices[3];
		for (int j = 0; j < 3; j++) {
			indices[j] = i + 10 * j;
		}
		glBindVertexArray(VAO[i]);//����������󶨵������������
		//���ö�������ָ��
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
