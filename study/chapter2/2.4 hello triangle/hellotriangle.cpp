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
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // shader 생성, (생성할 shader 유형)
    // glShaderSource : 
    // 파라미터 1 : shader 객체
    // 파라미터 2 : 소스코드가 몇개의 문자열로 되어있는지
    // 파라미터 3 : vertex shader 실제 소스코드
    // 파라미터 4 : 
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // shader 코드를 객체에 첨부 후 compile
    glCompileShader(vertexShader);
    // compile error 확인
    int success; // 성공을 나타내는 정수형 변수
    char infoLog[512]; // error message 담을 컨테이너
    // glGetShaderiv : compile 성고 확인 함수
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // compile 실패
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;// error message 출력
    }

    // fragment shader compile

    // fragment shader : 픽셀 출력 컬러 값 계산
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 마찬가지로 compile error 확인
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    // shaderProgram : 여러가지 shader 연결(link)
    // 각 shader들의 출력값 -> 다음 shader 입력값으로 연결

    // glCreateProgram : program 생성, 생성된 program 객체 ID return
    // 이전에 compile한 shader를 program 객체에 첨부 -> glLinkProgram 함수로 연결
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // linking errors 확인
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // shader를 program 객체로 연결 후에는 shader 객체 반드시 삭제
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // OpenGL = 3D Library -> 좌표는 모두 3D 공간
    // 3개의 축 (x, y, z) 에서 값이 모두 -1.0 ~ 1.0 사이에 있어야만 함 
    // normalized device coordinates 범위 외부 좌표는 보이지 않음
    // ndc 안에 float 배열로 정의
    float vertices[] = {
        // 2D처럼 보이도록 깊이를 모두 같게
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };

    //NDC 좌표 -> vieport transform 통해서 -> screen-space coordinates 좌표로 변환
    // ssc 좌표는 fragment로 변환되어 fragment shader 입력값으로 -> 정점 데이터 정의 완료
    // 정점 데이터를 vertex shader에게 전달 -> GPU에 메모리 할당
    // 메모리 해석 방법 구성, 그래픽 카드에 전달할 방법 명시 -> 작업 완료
    // vertex shader가 메모리에서 처리

    // VBO(vertex buffer objects) : 메모리 관리, 정점 GPU 메모리 상에 저장, OpenGL 객체
    // VAO (vertex array object) : VBO와 같이 바인딩 가능, 그 후 vertex 속성 호출은 VAO 내에 저장
    // VBO의 buffer 유형은 GL_ARRAY_BUFFER
    // glGenBuffers : buffer ID 생성
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // VAO 바인딩
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    // glBindBuffer : 새롭게 생성된 버퍼를 GL_ARRAY_BUFFER로 바인딩
    // OpenGL이 사용하기 위해 vertex list 복사
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData : 미리 정의된 데이터를 현재 바인딩 된 buffer 메모리에 복사
    // 파라미터 1 : 데이터를 복사하여 집어넣을 buffer의 유형
    // 파라미터 2 : buffer에 저장할 데이터 크기, sizeof 사용 가능
    // 파라미터 3 : 실제 데이터
    // 파라미터 4 : 그래픽 카드가 주어진 데이터를 관리하는 방법 (static, dinamic, stream)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);// 거의 변하지 않음 (static)

    // glVertexAttribPointer : OpenGL에게 vertex 속성에 대한 data를 어떻게 해석해야하는지 알려줌
    // 파라미터 1 : vertex 속성
    // 파라미터 2 : vertex 속성의 크기
    // 파라미터 3 : data type
    // 파라미터 4 : 데이터를 정규화 할지
    // 파라미터 5 : stride, 연이은 vertex 속성 세트들 사이 공백을 알려줌
    // 파라미터 6 : buffer에서 데이터가 시작하는 위치의 offset, 책 뒷부분에 자세히 설명할 예정
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

        // object 그리기
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); 
        // glDrawArrays : 현재 활성화된 shader, 이전에 정의된 vertex 속성 구성, VBO vertex data 사용해 primitive 그림
        // 파라미터 1 : OpenGL primitive 유형 지정
        // 파라미터 2 : vertex 배열 시작 index 지정
        // 파라미터 3 : 몇개의 vertex를 그릴지
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
