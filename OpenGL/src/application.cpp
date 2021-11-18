#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// C++ Macro - these are compiler specific.
// __debugbreak is VS specific.
# define ASSERT(x) if (!(x)) __debugbreak();
# define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));    

static void GLClearError()
{
    GLenum err = glGetError();
    while (glGetError() != GL_NO_ERROR); 
};

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << error << 
            " " << function << " " << file << ":" << line << std::endl;
        return false;
    };
    return true;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragemntSource;
};

static ShaderProgramSource ParseShader(const std::string filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /* enables vsync */
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    /* Positions map */
    float positions[] = {
        -0.5f, -0.5,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
    };
    /* Index into the vertex buffer*/
    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
    };


    /*
    How does the vertex buffer link to the index buffer? I dont understand that. 
    Look at the index buffer video again.
    */

    /* Created vertex array, what stores state of vertex and index buffer*/
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    /* Bind call allows following setup to work */
    GLCall(glBindVertexArray(vao));

    /* Create a vertex buffer */
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    /* Bind the buffer, make it active */
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    /* Provide buffer data with reference of array. */
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    /* Enable and create vertex attributes */
    GLCall(glEnableVertexAttribArray(0));
    /* This attrib pointer tells openGL that the vertex is the size of 2 floats. */
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    /* Create an index buffer This uses the indicies variable to index into the positions array. */
    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    /* Bind the buffer, make it active */
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    /* Provide buffer data with reference of array. */
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW));



    /* Parse shaders */
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT\n" << std::endl;
    std::cout << source.FragemntSource << std::endl;

    // Create shaders from parsed source
    unsigned int shader = CreateShader(source.VertexSource, source.FragemntSource);
    GLCall(glUseProgram(shader));

    // Obtain uniform location from shader, this is for the colour properties.
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    // set shader values.
    //GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

    float r = 0.0f;
    float increment = 0.005f;

    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        /* Drwas bound buffer */
        GLClearError();


        // bind relevant vertex / index buffers for drawing
        GLCall(glUseProgram(shader));
        // updating shader value dynamically.
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        // bind vertex array
        GLCall(glBindVertexArray(vao));
        // video guy says you need to bind the index buffer after the VAO.
        // evidence points otherwise as the index array still functions without binding.
        // comments also say otherwise. 
        // Vertex Array in OpenGL.
        //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        // change increment of red channel on every draw cycle.
        if (r > 1.0f)
            increment = -0.005f;
        else if (r < 0.0f)
            increment = 0.005f;

        r += increment;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteProgram(shader));
    glfwTerminate();
    return 0;
}