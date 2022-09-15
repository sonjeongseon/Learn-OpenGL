#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // vertex shader compile
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // shader ����, (������ shader ����)
    // glShaderSource : 
    // �Ķ���� 1 : shader ��ü
    // �Ķ���� 2 : �ҽ��ڵ尡 ��� ���ڿ��� �Ǿ��ִ���
    // �Ķ���� 3 : vertex shader ���� �ҽ��ڵ�
    // �Ķ���� 4 : 
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // shader �ڵ带 ��ü�� ÷�� �� compile
    glCompileShader(vertexShader);
    // compile error Ȯ��
    int success; // ������ ��Ÿ���� ������ ����
    char infoLog[512]; // error message ���� �����̳�
    // glGetShaderiv : compile ���� Ȯ�� �Լ�
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // compile ����
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;// error message ���
    }

    // fragment shader compile

    // fragment shader : �ȼ� ��� �÷� �� ���
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // ���������� compile error Ȯ��
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    // shaderProgram : �������� shader ����(link)
    // �� shader���� ��°� -> ���� shader �Է°����� ����

    // glCreateProgram : program ����, ������ program ��ü ID return
    // ������ compile�� shader�� program ��ü�� ÷�� -> glLinkProgram �Լ��� ����
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // linking errors Ȯ��
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // shader�� program ��ü�� ���� �Ŀ��� shader ��ü �ݵ�� ����
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // OpenGL = 3D Library -> ��ǥ�� ��� 3D ����
    // 3���� �� (x, y, z) ���� ���� ��� -1.0 ~ 1.0 ���̿� �־�߸� �� 
    // normalized device coordinates ���� �ܺ� ��ǥ�� ������ ����
    // ndc �ȿ� float �迭�� ����
    float vertices[] = {
        // 2Dó�� ���̵��� ���̸� ��� ����
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };

    //NDC ��ǥ -> vieport transform ���ؼ� -> screen-space coordinates ��ǥ�� ��ȯ
    // ssc ��ǥ�� fragment�� ��ȯ�Ǿ� fragment shader �Է°����� -> ���� ������ ���� �Ϸ�
    // ���� �����͸� vertex shader���� ���� -> GPU�� �޸� �Ҵ�
    // �޸� �ؼ� ��� ����, �׷��� ī�忡 ������ ��� ��� -> �۾� �Ϸ�
    // vertex shader�� �޸𸮿��� ó��

    // VBO(vertex buffer objects) : �޸� ����, ���� GPU �޸� �� ����, OpenGL ��ü
    // VAO (vertex array object) : VBO�� ���� ���ε� ����, �� �� vertex �Ӽ� ȣ���� VAO ���� ����
    // VBO�� buffer ������ GL_ARRAY_BUFFER
    // glGenBuffers : buffer ID ����
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // VAO ���ε�
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    // glBindBuffer : ���Ӱ� ������ ���۸� GL_ARRAY_BUFFER�� ���ε�
    // OpenGL�� ����ϱ� ���� vertex list ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData : �̸� ���ǵ� �����͸� ���� ���ε� �� buffer �޸𸮿� ����
    // �Ķ���� 1 : �����͸� �����Ͽ� ������� buffer�� ����
    // �Ķ���� 2 : buffer�� ������ ������ ũ��, sizeof ��� ����
    // �Ķ���� 3 : ���� ������
    // �Ķ���� 4 : �׷��� ī�尡 �־��� �����͸� �����ϴ� ��� (static, dinamic, stream)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);// ���� ������ ���� (static)

    // glVertexAttribPointer : OpenGL���� vertex �Ӽ��� ���� data�� ��� �ؼ��ؾ��ϴ��� �˷���
    // �Ķ���� 1 : vertex �Ӽ�
    // �Ķ���� 2 : vertex �Ӽ��� ũ��
    // �Ķ���� 3 : data type
    // �Ķ���� 4 : �����͸� ����ȭ ����
    // �Ķ���� 5 : stride, ������ vertex �Ӽ� ��Ʈ�� ���� ������ �˷���
    // �Ķ���� 6 : buffer���� �����Ͱ� �����ϴ� ��ġ�� offset, å �޺κп� �ڼ��� ������ ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // render loop (drawing code)
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // object �׸���
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); 
        // glDrawArrays : ���� Ȱ��ȭ�� shader, ������ ���ǵ� vertex �Ӽ� ����, VBO vertex data ����� primitive �׸�
        // �Ķ���� 1 : OpenGL primitive ���� ����
        // �Ķ���� 2 : vertex �迭 ���� index ����
        // �Ķ���� 3 : ��� vertex�� �׸���
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}