// 공부 내용 추가 완료

#include <glad/glad.h> // GLAD용 헤더파일은 OpenGL 헤더파일 포함 -> 가장 먼저 포함시켜야 함
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// 창 크기 설정
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit(); // GLFW 초기화
    // glfwWindoWHint
    // 파라미터 1 : 옵션 알려주기,GLFW_ 접두어 붙은 옵션들
    // 파라미터 2 : 옵션 값 설정, 가능한 값들은 문서 참고
    // undefined reference 오류 발생시 라이브러리 잘못 연결됨 -> 오류 X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 윈도우 객체 생성
    // 함수의 리턴 값 : GLFWwindow 객체 -> 다른 GLFW 기능을 사용하기 위해 필요

    // glfwCreateWindow
    // 파라미터 1, 2 : 창의 너비와 높이
    // 파라미터 3 : 창의 이름
    // 파라미터 4, 5는 무시하면 됨
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SonGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // window의 context를 현재 thread의 main context로 지정하겠다고 알림
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD : OpenGL용 함수 포인터 관리, OS에 따라 올바른 함수를 정의하는 glfwGetProcAddress 제공
    // GLAD 초기화 필요
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // render loop 

    // 중지하라는 메시지 받기 전까지 계속 입력 처리해야 함
    // glfwWindowShouldClose : loop가 시작될 때마다 GLFw가 종료 명령을 받았는지 확인
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering 명령

        // glClearColor : 상태 설정(state-setting) 함수
        // glClear : 상태 사용(state-using) 함수

        // 어떤 색으로 지울지 설정
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 화면 buffer 지움(안하면 이전 loop 결과 남음)
        // 지우려 하는 buffer의 bit 전달, color, depth, stencil이 존재, 여기서는 color만 지움
        glClear(GL_COLOR_BUFFER_BIT);

        // glfwSwapBuffers : color buffer(image 그리고 화면 출력하는 기능) 교체 
        // glfwPollEvents : 이벤트 발생 확인 -> 윈도우 상태 업데이트 -> 정해진 함수 호출
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // rendering loop 종료 시 할당된 자원 정리,삭제
    glfwTerminate();
    return 0;
}

// GLFW 입력 관리
void processInput(GLFWwindow* window)
{
    // glfwGetKey : 해당 키가 눌려져있는지 여부 return
    // esc 눌렀는지 확인 -> 안눌렸다면 GLFW_RELEASE return
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // 눌렸다면 WindowClose 속성을 true로 setting -> GLFW 닫음
        glfwSetWindowShouldClose(window, true); 
}

// 렌더링 시작 전 렌더링 윈도우 사이즈를 알려주기
// 파라미터 1 : 위에서 언급한 GLFWwindow 객체
// 파라미터 2, 3 : 창의 새로운 크기
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // glViewport : 차원(dimensions) 설정
    // 파라미터 1, 2 : 윈도위 왼쪽 아래 모서리 위치 설정
    // 파라미터 3, 4 : 렌더링 윈도우 너비, 높이를 픽셀로 설정 (GLFW 자체에서 가져옴)
    glViewport(0, 0, width, height); 
}
