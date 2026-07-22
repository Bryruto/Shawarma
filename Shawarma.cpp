#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

//Open GL libs 
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

//++++++++++++++++++++++++++++++++++++++++++++++Errors and test 
//Asserting for open gl big help 
#define ASSERT(x) if(!(x)) __builtin_trap();
#define TEST(x) GLClearError();\
       x;\
    ASSERT(GLLog(#x,__FILE__,__LINE__))

//clear Errors to isolate problems
static void GLClearError(){
    while(glGetError());
}

//log Errors black screen broke my heart 
static bool GLLog(const char* function,const char* file , int line){
    while (GLenum error = glGetError()){
        std::cout << "[openGL] error (" << error << ")\n" << "Function name: " << 
            function << "\nfile: " << file << "\nline: " << line << std::endl;
        return false;
    }
    return true;
}

//++++++++++++++++++++++++++++++++++++++++shaders vertex and fragment
//to keep my shaders together
struct ShaderSource{
    std::string VertexSource;
    std::string FragmentSource;
};

//making what im doing make more sense before it goes crazy
enum class ShaderType{
    NONE = -1,VERTEX = 0, FRAGMENT = 1
};

//build the shaders from file
static ShaderSource Parseshader(const std::string& filepath){
    std::ifstream stream(filepath);//read the path 

    std::stringstream ss[2];//make a string stream
    ShaderType type = ShaderType::NONE;//defualt value = NONE = -1

    //look at file line by line add to 1st stringstream in vertex and 2 for fragment
    std::string line;
    while(getline(stream,line)){
        if(line.find("#shader") != std::string::npos){
            
            if(line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            
            else if(line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else ss[(int)type] << line << "\n";
    }
    return {ss[0].str(),ss[1].str()};
}

//compile my shaders 
static unsigned int CompileShader(unsigned int type,const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id,1,&src,nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);

        char* message = (char*)alloca(length * sizeof(char));//point to stack memory of size length*char
        glGetShaderInfoLog(id,length,&length,message);
        std::cout << "Failed to compile "<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

//create new vertexShader and fragmentShader to use 
static unsigned int CreateShader(const std::string& vertexShader,const std::string& fragmentShader){
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

    //attach and link 
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //free the data its linked 
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(){
    //configure instance on glfw version 4.6 for me i can show you how to do this 
    if(!glfwInit()){
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1; 
    }
    TEST(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4));
    TEST(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6));
    TEST(glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE));
    //core only give me what i ask for and use 
    
    //getting the size of my monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    //building a window width,height,name,fullscreen = will do this later , share dont know what that means yet  
    GLFWwindow* window = glfwCreateWindow(mode->width/2,mode->height,"Shawarma",nullptr,nullptr);
    
    //check if the window is working  
    if(!window){
        std::cout << "failed to create window" << std::endl;
        glfwTerminate();//free all data for fail
    }
    
    //show to screen
    TEST(glfwMakeContextCurrent(window));
    
    //refresh rate for your monitor 
    TEST(glfwSwapInterval(1));

    //load up glad 
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "failed to initialize glad" << std::endl;
        glfwTerminate();
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    //vertices
    float Points[] = {
        -0.5f,-0.5f,0.0f,//0
        0.5f,-0.5f,0.0f,//1
        0.0f,0.5f,0.0f//2
    };

    //vertex buffer 
    int index[] = {
        1,2,3
    };

    
    //TODO
    unsigned int vao;
    TEST(glGenVertexArrays(1,&vao));
    TEST(glBindVertexArray(vao));

    
    //this what we use to put all points on the vram for fast draw
    unsigned int buffer;
    TEST(glGenBuffers(1,&buffer));
    TEST(glBindBuffer(GL_ARRAY_BUFFER,buffer));
    TEST(glBufferData(GL_ARRAY_BUFFER,sizeof(Points),Points,GL_STATIC_DRAW));

    
    //TODO
    unsigned int ibo;
    TEST(glGenBuffers(1,&ibo));
    TEST(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo));
    TEST(glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW));

    
    //TODO 
    TEST(glEnableVertexAttribArray(0));
    TEST(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));//this links buffer with vao  


    //make shader from file path
    ShaderSource  source = Parseshader("shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
    TEST(glUseProgram(shader));
 
    
    //this will let me actively change the colors
    int location = glGetUniformLocation(shader,"u_Color");
    ASSERT(location != -1);
    glUniform4f(location,0.7f,0.3f,0.3f,1.0f);

    
    float red{},green{},blue{},opacity{};
    //rendering loop i will abstract all of these :)  
    while(!glfwWindowShouldClose(window)){
        //colors
        glClearColor(0.0f,0.0f,0.0f,1.0f);//background
        glClear(GL_COLOR_BUFFER_BIT);

        //render shader 
        TEST(glUseProgram(shader));

        //update shader colors
        glUniform4f(location,red,green,blue,opacity);

        //draw to screen 
        TEST(glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr));

        //bufferswap
        TEST(glfwSwapBuffers(window));

        //Events
        TEST(glfwPollEvents());
    }
    glfwTerminate();//end game
}
