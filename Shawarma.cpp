#include <iostream>
#include <iostream>
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

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

int main(){
    
    //configure instance on glfw version 4.6 for me i can show you how to do this 
    TEST(glfwInit());
    TEST(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4));
    TEST(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6));
    TEST(glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE));
    //core only give me what i ask for and use 
    
    //building a window width,height,name,fullscreen = will do this later , share dont know what that means yet  
    GLFWwindow* window = glfwCreateWindow(800,600,"Shawarma",nullptr,nullptr);
    
    //check if the window is working  
    if(!window){
        std::cout << "failed to create window" << std::endl;
        glfwTerminate();//free all data for fail
    }
    TEST(glfwMakeContextCurrent(window));//show to screen
    

    //load up glad 
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "failed to initialize glad" << std::endl;
        glfwTerminate();
    }

    //colors
    struct Colors{
        float red;
        float green;
        float blue;
        float opacity;
        
        Colors(float red = 0.0f,float green = 0.0f,float blue = 0.0f,float opacity = 1.0f):
            red(red),green(green),blue(blue),opacity(opacity){}
    };
    
    //may change the name i dont know yet 
    Colors mainScreen;

    //rendering loop i will abstract all of these :)  
    while(!glfwWindowShouldClose(window)){
        //input 
        
        //colors
        glClearColor(mainScreen.red,mainScreen.green,mainScreen.blue,mainScreen.opacity);
        glClear(GL_COLOR_BUFFER_BIT);

        //bufferswap
        glfwSwapBuffers(window);

        //Events
        glfwPollEvents();
    }
    glfwTerminate();//end game
}
