//Using Modern OpenGL in C++ by The Cherno --------- where im learning and open.gl for docs 
#include <iostream> 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
//compile ========== g++ *.cpp -o main -lGLEW -lglfw -lGL

//for the buffer indexing i can ask me why 
enum class ShaderType{
    NONE = -1,VERTEX = 0,FRAGMENT = 1 
};

//Plain old data just so i can keep data together and pass it
struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};

//this will read the shaderfiles
static ShaderProgramSource ParseShader(const std::string& filepath){
    std::ifstream stream(filepath);
    
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    std::string line;
    while(getline(stream,line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos){
                type = ShaderType::VERTEX;
            }
            else if(line.find("fragment") != std::string::npos){
                type = ShaderType::FRAGMENT;
            }
        }
        else{
            ss[(int)type] << line << "\n";
        }
    }
    return {ss[0].str(),ss[1].str()}; 
}

//this will compile shaders into a executable 
static unsigned int CompileShader(unsigned int type ,const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    
    glShaderSource(id,1,&src,nullptr); 
    
    glCompileShader(id);

    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
        
        char* message = (char*)alloca(length * sizeof(char)); 
        glGetShaderInfoLog(id,length,&length,message);
        std::cout << "Faild to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

//this will run the compiled shaders
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    unsigned int program = glCreateProgram(); 
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glValidateProgram(program); 
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;    
}


int main()
{
    //check library 
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n"; 
        return -1;
    }
    
    //this will make fullscreen based on screen size 
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width,mode->height,"triangle",monitor,nullptr); 
    
    //checked if window opened 
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //check library must be here dont move 
    if(!glewInit()){
        std::cout << "glew failed\n" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) <<std::endl;
    
    //positions for vertex on the screen
    float positions[] = {
        -0.5f,-0.5f,
        0.5f,-0.5f,
        0.5f,0.5f,
        -0.5f,0.5f
    };
    
    //index the positions to not repeat data
    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

    //make a buffer
    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,6 * 2 * sizeof(float),positions,GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    //also a buffer 
    unsigned int ibo;
    glGenBuffers(1,&ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(unsigned int),indices,GL_STATIC_DRAW);

    //use fuctions above and load shaders 
    ShaderProgramSource  source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
    glUseProgram(shader);

    //game loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    //delete all program data
    glDeleteProgram(shader);
    
    //exit screen
    glfwTerminate();
    return 0;
}
