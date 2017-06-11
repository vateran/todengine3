#include "tod/graphics/openglrenderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include "tod/filesystem.h"

//Library link
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3dll.lib")

namespace tod::graphics
{
    GLuint gShaderProgram;
    
    // 정점 버퍼 오브젝트
    GLuint gVertexBufferObject;
    std::vector<glm::vec3> gVertices;
    
    bool CheckShader(GLuint shader);
    
    const std::string ReadStringFromFile(const std::string& filename) {
        std::ifstream f(filename);
        
        auto pwd = FileSystem::instance()->getCurrentWorkingDirectory();
        
        if (!f.is_open())
            return "";
        
        return std::string(std::istreambuf_iterator<char>(f),
                           std::istreambuf_iterator<char>());
    }
    
    GLuint CreateShader(GLenum shaderType, const std::string& source) {
        GLuint shader = glCreateShader(shaderType);
        if (shader == 0)
            return 0;
        
        // set shader source
        const char* raw_str = source.c_str();
        glShaderSource(shader, 1, &raw_str, NULL);
        
        // compile shader object
        glCompileShader(shader);
        
        // check compilation error
        if (!CheckShader(shader)){
            glDeleteShader(shader);
            return 0;
        }
        
        return shader;
    }
    
    bool CheckShader(GLuint shader) {
        GLint state;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
        if (GL_FALSE == state){
            int infologLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
            if (infologLength > 1){
                int charsWritten = 0;
                char *infoLog = new char[infologLength];
                glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
                std::cout << infoLog << std::endl;
                delete[] infoLog;
            }
            return false;
        }
        return true;
    }
    
    bool CheckProgram(GLuint program) {
        GLint state;
        glGetProgramiv(program, GL_LINK_STATUS, &state);
        if (GL_FALSE == state) {
            int infologLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
            if (infologLength > 1){
                int charsWritten = 0;
                char *infoLog = new char[infologLength];
                glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
                std::cout << infoLog << std::endl;
                delete[] infoLog;
            }
            return false;
        }
        return true;
    }

    
    bool InitApp() {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        
        glEnable(GL_DEPTH_TEST);
        
        glDisable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_LINE);
        
        
        // ---------- 셰이더 생성 및 컴파일 ----------
        // 셰이더 파일 읽기
        std::string vertShaderSource = ReadStringFromFile("../../../../BasicShader.glvs");
        std::string fragShaderSource = ReadStringFromFile("../../../../BasicShader.glfs");
        
        // 셰이더 오브젝트 생성
        GLuint vertShaderObj = CreateShader(GL_VERTEX_SHADER, vertShaderSource);
        GLuint fragShaderObj = CreateShader(GL_FRAGMENT_SHADER, fragShaderSource);
        
        // 셰이더 프로그램 오브젝트 생성
        gShaderProgram = glCreateProgram();
        
        // 셰이더 프로그램에 버텍스 및 프래그먼트 셰이더 등록
        glAttachShader(gShaderProgram, vertShaderObj);
        glAttachShader(gShaderProgram, fragShaderObj);
        
        // 셰이더 프로그램과 셰이더 링킹(일종의 컴파일) 그리고 확인
        glLinkProgram(gShaderProgram);
        if (!CheckProgram(gShaderProgram)) {
            glDeleteProgram(gShaderProgram);
            return false;
        }
        
        // 사용된 셰이더 떼어냄
        glDetachShader(gShaderProgram, vertShaderObj);
        glDetachShader(gShaderProgram, fragShaderObj);
        
        // 셰이더 삭제
        glDeleteShader(vertShaderObj);
        glDeleteShader(fragShaderObj);
        
        
        // ---------- 정점 어트리뷰트 버퍼 생성 ----------
        // 정점 정의
        gVertices = {
            glm::vec3(-1.0f, -1.0f, 0.f),
            glm::vec3(1.0f, -1.0f, 0.f),
            glm::vec3(0.0f, 1.0f, 0.f)
        };
        
        // OpenGL 4.5 이전의 버퍼 생성 방식.
        // 정점 버퍼 생성
        glGenBuffers(1, &gVertexBufferObject);
        
        // 정점 버퍼 바인딩 및 데이터 등록
        glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, gVertices.size()*sizeof(glm::vec3), &gVertices[0], GL_STATIC_DRAW);
        
        // OpenGL 4.5의 버퍼 생성 방식. (Direct State Access 기능으로 가능해짐)
        //glCreateBuffers(1, &gVertexBufferObject);
        //glNamedBufferData(gVertexBufferObject, gVertices.size()*sizeof(glm::vec3), &gVertices[0], GL_STATIC_DRAW);
        
        return true;
    }
    
    void ErrorCallback(int error, const char* description) {
        std::cout << description << std::endl;
    }
    
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    void WindowSizeChangeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
    
    void Render()
    {
        // 버퍼 지우기
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // gShaderProgram의 셰이더 프로그램을 사용할것임
        glUseProgram(gShaderProgram);
        
        // 버퍼를 바인딩
        glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
        // location 0 정점 속성을 활성화 (셰이더 코드 참조)
        glEnableVertexAttribArray(0);
        // location 0 정점 속성의 포인터
        glVertexAttribPointer(0, 9, GL_FLOAT, 0, 0, NULL);
        
        // 정점을 그림
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)gVertices.size());
        
        // 바인딩 해제
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 활성화된 정점 속성 비활성화
        glDisableVertexAttribArray(0);
        
        // 셰이더 프로그램 사용 중지
        glUseProgram(0);
    }
    
    
    
//-----------------------------------------------------------------------------
bool OpenGlRenderer::initialize(void* window_handle)
{
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
    // uncomment these lines if on Apple OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
    /*float points[] = {
        0.0f,  0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f
    };
    
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);
    
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    const char* vertex_shader =
    "#version 400\n"
    "in vec3 vp;"
    "void main() {"
    "  gl_Position = vec4(vp, 1.0);"
    "}";
    
    const char* fragment_shader =
    "#version 400\n"
    "out vec4 frag_colour;"
    "void main() {"
    "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
    "}";
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    
    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);*/
    
    
    
    
    while(!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glUseProgram(shader_programme);
        //glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }

    
    // close GL context and any other GLFW resources
    glfwTerminate();
    
    return true;
}

    
//-----------------------------------------------------------------------------
bool OpenGlRenderer::render(Camera* camera, Node* scene_root)
{
    return true;
}
    
}
