//#include <iostream>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include "Sphere.h"
//#include <fstream>
//#include <string>
//#include <sstream> // stringstream
//
//using namespace std;
//
//struct ShaderProgramSource
//{
//	std::string VertexSource;
//	std::string FragmentSource;
//};
//static ShaderProgramSource ParseShader(const std::string& filepath)
//{
//	std::ifstream stream(filepath);
//
//	enum class ShaderType
//	{
//		NONE = -1, VERTEX = 0, FRAGMENT = 1
//	};
//
//	std::string line;
//	std::stringstream ss[2];
//	ShaderType type = ShaderType::NONE;
//	while (std::getline(stream, line))
//	{
//		if (line.find("#shader") != std::string::npos)
//		{
//			if (line.find("vertex") != std::string::npos)
//			{
//				type = ShaderType::VERTEX;
//			}
//			else if (line.find("fragment") != std::string::npos)
//			{
//				type = ShaderType::FRAGMENT;
//			}
//		}
//		else
//		{
//			// 把 type 作为 string stream 的 index，更清晰
//			ss[(int)type] << line << '\n';
//		}
//	}
//
//	return { ss[0].str(), ss[1].str() };
//}
//
//static unsigned int CompileShader(unsigned int type, const std::string& source) // 第二个参数是因为 GLenum 是 unsigned int 的
//{
//	unsigned int id = glCreateShader(type);
//	const char* src = source.c_str(); // 返回的是一个指向 string 内部的指针，因此这个 string 必须存在才有效
//	glShaderSource(id, 1, &src, nullptr);
//	glCompileShader(id);
//
//	// Error handling
//	int result;
//	glGetShaderiv(id, GL_COMPILE_STATUS, &result); // iv: integer, vector
//	if (result == GL_FALSE)
//	{
//		int length;
//		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
//
//		// alloca 是C语言给你的一个函数，它让你在堆栈上动态地分配，根据你的判断来使用
//		// 因为我们不想在堆上分配，所以调用了这个函数
//		char* message = (char*)alloca(length * sizeof(char));
//		glGetShaderInfoLog(id, length, &length, message);
//		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
//		std::cout << message << std::endl;
//		glDeleteShader(id);
//		return 0;
//	}
//
//	return id;
//}
//
//// 从文件中获取，然后编译、链接，生成buffer id，用于之后的绑定
//static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
//{
//	unsigned int program = glCreateProgram();
//	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
//	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
//
//	glAttachShader(program, vs);
//	glAttachShader(program, fs);
//	glLinkProgram(program);
//
//	// 查阅文档我们知道，验证的状态会被存储为程序对象状态的一部分你可以调用，比如用 glGetProgram 查询实际结果是什么之类的
//	glValidateProgram(program);
//
//	// 因为已经被链接到一个程序中了，所以如果你愿意可以删除中间部分
//	// 在C++中编译东西的时候会得到诸如 .obj 这样的中间文件，shader 也是如此
//	// 还有一些其他的函数，比如 glDetachShader 之类的它会删除源代码
//	// 但是cherno不喜欢碰那些函数。
//	// 首先清理这些也不是必要的，因为它占用的内存很少，但是保留着色器的源代码对于调试之类的是很重要的
//	// 因此很多引擎根本不会去调用 glDetachShader 这些
//	glDeleteShader(vs);
//	glDeleteShader(fs);
//
//	return program;
//}
////static unsigned int* get_indexArr( ) {
////	unsigned int indexArr[6];
////	for (int i = 0; i < 6; i++) {
////		for (int j = 0; j < 3; j++) {
////			indexArr[i] = i + j * 2;
////		}
////	}
////	return  indexArr;
////};
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//int main() {
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//
//	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
//	if (window == NULL) {
//		cout << "Failed to create GLFW window" << endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		cout << "Failed to initialize GLAD" << endl;
//		return -1;
//	}
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glEnable(GL_DEPTH_TEST);
//	ShaderProgramSource source = ParseShader("res/shaders/Sphere.shader");
//
//	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
//	//创建地球
//	Sphere earth(0.5f, 36, 18);
//	unsigned int VBO[2], VAO[2], EBO[2];
//	int stride = earth.getInterleavedStride();
//	glGenVertexArrays(2, VAO);
//	glGenBuffers(2, VBO);
//	glGenBuffers(2, EBO);
//	glBindVertexArray(VAO[0]);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
//	glBufferData(GL_ARRAY_BUFFER, earth.getInterleavedVertexSize(), earth.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, earth.getIndexSize(), earth.getIndices(), GL_STATIC_DRAW);
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
//	glEnableVertexAttribArray(1);
//
//	// load and create a texture 
//   // -------------------------
//	unsigned int texture1;
//	// texture 1
//	// ---------
//	glGenTextures(1, &texture1);
//	glBindTexture(GL_TEXTURE_2D, texture1);
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
//	unsigned char* data = stbi_load("res/pic/earth2048.bmp", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//	//创建月球
//	Sphere moon(0.2f, 36, 18);
//	 stride = moon.getInterleavedStride();
//	glBindVertexArray(VAO[1]);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
//	glBufferData(GL_ARRAY_BUFFER, moon.getInterleavedVertexSize(), moon.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, earth.getIndexSize(), moon.getIndices(), GL_STATIC_DRAW);
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
//	glEnableVertexAttribArray(1);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//	// load and create a texture 
//   // -------------------------
//	unsigned int texture2;
//	// texture 1
//	// ---------
//	glGenTextures(1, &texture2);
//	glBindTexture(GL_TEXTURE_2D, texture2);
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
//	data = stbi_load("res/pic/moon1024.bmp", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	while (!glfwWindowShouldClose(window)) {
//		processInput(window);
//		glm::mat4 view = glm::mat4(1.0f);
//		glm::mat4 projection = glm::mat4(1.0f);
//		glm::mat4 model = glm::mat4(1.0f);
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, texture1);
//		glUseProgram(shader);
//		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
//		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
//		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
//		glBindVertexArray(VAO[0]);
//		glUniform1i(glGetUniformLocation(shader, "ourTexture"), 0); // 手动设置
//		glDrawElements(GL_TRIANGLES,                   
//			earth.getIndexCount(),
//			GL_UNSIGNED_INT,                 
//			(void*)0);
//		glBindVertexArray(0);
//
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, texture2);
//		glUseProgram(shader);
//		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.5f));
//		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
//		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
//		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
//		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//
//		glBindVertexArray(VAO[1]);
//		glUniform1i(glGetUniformLocation(shader, "ourTexture"), 1); // 手动设置
//		glDrawElements(GL_TRIANGLES,
//			earth.getIndexCount(),
//			GL_UNSIGNED_INT,
//			(void*)0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindVertexArray(0);
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	} 
//		glDeleteBuffers(2, VBO);
//        glDeleteVertexArrays(2, VAO);
//	glfwTerminate();
//	return 0;
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//}
//
//void processInput(GLFWwindow* window) {
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//		glfwSetWindowShouldClose(window, true);
//	}
//}
