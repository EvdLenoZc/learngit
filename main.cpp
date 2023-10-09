#include "Angel.h"

#include <vector>
#include <fstream>
#include <string>

using namespace std;

int window;
// 三角面片中的顶点序列
typedef struct vIndex {
	unsigned int a, b, c;
	vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

std::vector<glm::vec3> vertices;
std::vector<vec3i> faces;

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

std::vector<glm::vec3> points;   // 传入着色器的绘制点
std::vector<glm::vec3> colors;   // 传入着色器的颜色

const int NUM_VERTICES = 8;

const glm::vec3 vertex_colors[NUM_VERTICES] = {
	glm::vec3(1.0, 1.0, 1.0),  // White
	glm::vec3(1.0, 1.0, 0.0),  // Yellow
	glm::vec3(0.0, 1.0, 0.0),  // Green
	glm::vec3(0.0, 1.0, 1.0),  // Cyan
	glm::vec3(1.0, 0.0, 1.0),  // Magenta
	glm::vec3(1.0, 0.0, 0.0),  // Red
	glm::vec3(0.0, 0.0, 0.0),  // Black
	glm::vec3(0.0, 0.0, 1.0)   // Blue
};

void read_off(const std::string filename)
{
	// 打开OFF文件并读取模型信息
	if (filename.empty()) {
		return;
	}
	std::ifstream fin;
	fin.open(filename);
	if (!fin) {
		printf("文件有误\n");
		return;
	}
	else {
		printf("文件打开成功\n");
		vertices.clear();
		faces.clear();

		// 读取OFF字符串
		string str;
		fin >> str;
		// 读取文件中顶点数、面片数、边数
		fin >> nVertices >> nFaces >> nEdges;

		// 读取顶点坐标并存储到vertices容器
		for (int i = 0; i < nVertices; ++i) {
			glm::vec3 vertex;
			fin >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		}

		// 读取面片信息并存储到faces容器
		for (int i = 0; i < nFaces; ++i) {
			int numVerticesPerFace;
			fin >> numVerticesPerFace;
			if (numVerticesPerFace == 3) {
				int a, b, c;
				fin >> a >> b >> c;
				faces.push_back(vec3i(a, b, c));
			}
			// 处理其他多边形
		}
	}
	fin.close();
}





void storeFacesPoints()
{
	points.clear();
	colors.clear();

	// 遍历每个三角面片
	for (const vec3i& face : faces)
	{
		// 添加面片的顶点坐标到points容器
		points.push_back(vertices[face.a - 1]); // 注意索引从1开始，需要减1
		points.push_back(vertices[face.b - 1]);
		points.push_back(vertices[face.c - 1]);

		// 添加面片的颜色到colors容器
		colors.push_back(vertex_colors[face.a - 1]);
		colors.push_back(vertex_colors[face.b - 1]);
		colors.push_back(vertex_colors[face.c - 1]);
	}
}


void init()
{
	// 读取off模型文件
	//read_off("./assets/cube.off");
	read_off("C://Users//light//Desktop//大三下学期//计算机图形学//2//实验2.2//assets//cube.off");
	storeFacesPoints();

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);  	// 分配1个顶点数组对象
	glBindVertexArray(vao[0]);  	// 绑定顶点数组对象

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3) + colors.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

	// @TODO: Task1:修改完成后再打开下面注释，否则程序会报错
	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);

	// 读取着色器并使用
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// 从顶点着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

	// 黑色背景
	glClearColor(0.0, 0.0, 0.0, 1.0);
}


void display(void)
{
	// @TODO: Task2:清理窗口，包括颜色缓存和深度缓存

	// 清理窗口颜色缓存和深度缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制你的模型或者其他OpenGL渲染操作
	glDrawArrays(GL_TRIANGLES, 0, points.size());
}





// 窗口键盘回调函数。
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		cout << "read cube.off" << endl;
		//read_off("./assets/cube.off");
		read_off("C://Users//light//Desktop//大三下学期//计算机图形学//2//实验2.2//assets//cube.off");
		storeFacesPoints();
		// @TODO: Task1:修改完成后再打开下面注释，否则程序会报错
		glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), &points[0]);
		glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);
	}
	else if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		cout << "read cube2.off" << endl;
		read_off("./assets/cube2.off");
		//read_off("C://Users//light//Desktop//大三下学期//计算机图形学//2//实验2.2//assets//cube2.off");
		storeFacesPoints();
		// @TODO: Task1:修改完成后再打开下面注释，否则程序会报错
		glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), &points[0]);
		glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);
	}
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		cout << "reset" << endl;
		// 关闭深度测试
		glDisable(GL_DEPTH_TEST);
		// 关闭面剔除
		glDisable(GL_CULL_FACE);
		// 使用填充绘制模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	}




	// 在key_callback函数中处理深度测试的启用和禁用
	// Task-2: 启用深度测试
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS && mode == 0x0000) // 0x0000表示组合键为空 
	{
		cout << "depth test: enable" << endl;

		// 启用深度测试
		glEnable(GL_DEPTH_TEST);

		// 清理窗口，包括颜色缓存和深度缓存
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	}
	// Task-2: 禁用深度测试
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		cout << "depth test: disable" << endl;
		glDisable(GL_DEPTH_TEST);
	}





	// Task-3: 启用背面面片剔除
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS && mode == 0x0000)
	{
		cout << "cull back: enable" << endl;
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	// Task-3: 禁用背面面片剔除
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		cout << "cull back: disable" << endl;
		glDisable(GL_CULL_FACE);
	}












	// Task-4: 启用正面面片剔除
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS && mode == 0x0000)
	{
		cout << "cull front: enable" << endl;
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	// Task-4: 禁用正面面片剔除
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		cout << "cull front: disable" << endl;
		glDisable(GL_CULL_FACE);
	}






	// Task-5: 启用线绘制模式
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS && mode == 0x0000)
	{
		cout << "line mode: enable" << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	// Task-5: 禁用线绘制模式
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		cout << "line mode: disable" << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    int a = 1;
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(600, 600, "3D OFF Model", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	init();
	while (!glfwWindowShouldClose(window))
	{
		display();

		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}