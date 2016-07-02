#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

static void error_callback(int error, const char *description) {
    cerr << "GLFW Error: " << description << endl;
}

double cx = 0.0, cy = 0.0, zoom = 1.0;
int itr = 100;
int fps = 0;

GLFWwindow *window = nullptr;

int w = 640;
int h = 480;

GLuint program;
GLuint shader;

double last_time = 0, current_time = 0;
unsigned int ticks = 0; 

bool keys[1024] = { 0 };

static void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    double xr = 2.0 * (xpos / (double)w - 0.5);
    double yr = 2.0 * (ypos / (double)h - 0.5);

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cx += (xr - cx) / zoom / 2.0;
        cy -= (yr - cy) / zoom / 2.0;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    zoom += yoffset * 0.1 * zoom;
    if(zoom < 0.1) {
        zoom = 0.1;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const double d = 0.1 / zoom;

    if(action == GLFW_PRESS) {
        keys[key] = true;
    } else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }

    if(keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, 1);
    } else if(keys[GLFW_KEY_A]) {
        cx -= d;
    } else if(keys[GLFW_KEY_D]) {
        cx += d;
    } else if(keys[GLFW_KEY_W]) {
        cy += d;
    } else if(keys[GLFW_KEY_S]) {
        cy -= d;
    } else if(keys[GLFW_KEY_MINUS]) {
        itr += 10;
    } else if(keys[GLFW_KEY_EQUAL]) {
        itr -= 10;
        if(itr <= 0) {
            itr = 0;
        }
    }

}

const char* vertex_shader =
"#version 410\n"
"in vec3 vp;"
"void main () {"
"  gl_Position = vec4 (vp, 1.0);"
"}";

static void update_window_title()
{
    std::ostringstream ss;
    ss << "Mandelbrot Renderer";
    ss << ", FPS: " << fps;
    ss << ", Iterations: " << itr;
    ss << ", Zoom: " << zoom;
    ss << ", At: (" << setprecision(8) << cx << " + " << cy << "i)";
    glfwSetWindowTitle(window, ss.str().c_str());
}

int main(int argc, char *argv[])
{
    if(!glfwInit()) {
        cerr << "Failed to init GLFW" << endl;
        return 1;
    }

    atexit(glfwTerminate);

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(w, h, "Mandelbrot", NULL, NULL);
    if(!window) {
        cerr << "Failed to create window" << endl;
        return 1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR_NORMAL, GLFW_STICKY_KEYS);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    std::ifstream t("shader.glsl");
    if(!t.is_open()) {
        cerr << "Cannot open shader.glsl!" << endl;
        return 1;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    const char *src  = str.c_str();

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &src, NULL);
    glCompileShader (fs);

    int success;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success) {
        int s;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetShaderInfoLog(fs, s, &s, buf);

        cerr << buf << endl;
        delete [] buf;
        return 1;
    }

    GLuint shader_programme = glCreateProgram ();
    glAttachShader (shader_programme, fs);
    glAttachShader (shader_programme, vs);
    glLinkProgram (shader_programme);

    float points[] = {
       -1.0f,  1.0f,  0.0f,
       -1.0f,  -1.0f,  0.0f,
       1.0f,  -1.0f,  0.0f,

       -1.0f,  1.0f,  0.0f,
       1.0f,  -1.0f,  0.0f,
       1.0f,  1.0f,  0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 2 * 9 * sizeof (float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUseProgram (shader_programme);

    last_time = glfwGetTime();

    while(!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &w, &h);
        glUniform2d(glGetUniformLocation(shader_programme, "screen_size"), (double)w, (double)h);
        glUniform1d(glGetUniformLocation(shader_programme, "screen_ratio"), (double)w / (double)h);
        glUniform2d(glGetUniformLocation(shader_programme, "center"), cx, cy);
        glUniform1d(glGetUniformLocation(shader_programme, "zoom"), zoom);
        glUniform1i(glGetUniformLocation(shader_programme, "itr"), itr);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray (vao);
        glDrawArrays (GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwWaitEvents();

        ticks++; 
        current_time = glfwGetTime();
        if(current_time - last_time > 1.0) {
            fps = ticks;
            update_window_title();
            last_time = glfwGetTime();
            ticks = 0;
        }
    }

    glfwDestroyWindow(window);
}
