#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <math.h>
#include "points.h"
#include "shader.h"
#include "point.h"
#include <chrono>
#include <random>
#include <unistd.h>  // for usleep
#include "shapes.h"
#include "tracker.h"
#include <CL/cl2.hpp>
#include <GL/glx.h>
//to compile - g++ test.cpp glad.c -o test -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl

#define ROW_SIZE 768
#define COLUMN_SIZE 1024
#define POINT_SIZE_CONST 36
#define POINT_SIZE sizeof(point)
#define NEXT 1
#define PREVIOUS 0

tracker observer = tracker();

/**
 * GLOBAL VARS
 */

cl::Device devices;
cl::Program program;
cl::Context context;
cl::Kernel kernel;

/**
 * MEDICAL CELL GENERATION
 */

/**
 * @brief 
 * 
 * @param current_buffer 
 * @param injection_x 
 * @param injection_y 
 * @param radius_sqr 
 * @param queue 
 * @param bufferList 
 * @param proc 
 */
void generate_medical_cells(
    int curr_buff_id,
    float injection_x, 
    float injection_y, 
    float radius_sqr, 
    cl::CommandQueue& queue, 
    cl::Buffer& buffer 
    )
{   
    std::cout << "Injection x: " << injection_x << ", Injection y: " << injection_y << std::endl;
    kernel.setArg(0, buffer);
    kernel.setArg(1, curr_buff_id);
    kernel.setArg(2, injection_x);
    kernel.setArg(3, injection_y);
    kernel.setArg(4, radius_sqr);

    cl::NDRange global_size(COLUMN_SIZE,ROW_SIZE);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, cl::NullRange);
    queue.finish();
}

/**
 * WINDOW INPUT PROCESSSING
 */

/**
 * @brief The following processes the inputs to the window.
 * 
 * @param window window context 
 * @param p pointer to grid
 */
void processInput(
    GLFWwindow *window, 
    points* p)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {
        double xpos, ypos;  

        glfwGetCursorPos(window, &xpos, &ypos);

        std::cout << "X-POS: " << xpos << std::endl;    
        std::cout << "Y-POS: " << ypos << std::endl;    

        // Since the glfw indexing begins in the the top left corner and the 
        // openGL buffer starts drawing from the bottom left.

        ypos = (ROW_SIZE-1)-ypos;

        //normalised coordinates

        double x_norm, y_norm;

        x_norm = xpos/COLUMN_SIZE*2.0f-1.0f;
        y_norm = ypos/ROW_SIZE*2.0f-1.0f;

        float number_of_cells = 100000;    

        std::cout << "Enter the number of cells \n" << std::endl;   

        // std::cin >> number_of_cells;    

        float radius_squared = number_of_cells/(ROW_SIZE*COLUMN_SIZE);     

        // Set up the buffer
        cl::Buffer buff(context, CL_MEM_READ_WRITE, COLUMN_SIZE*ROW_SIZE*POINT_SIZE);
        point* curr_buff;
        if(p->current_buffer == PREVIOUS)
        {   
            curr_buff = p->previous_data_pointer;
        }
        else
        {   
            curr_buff = p->next_data_pointer;
        }
        std::vector<point> tmp_buffer = std::vector<point>(curr_buff,curr_buff+COLUMN_SIZE*ROW_SIZE);
        
        //Call the kernel.
        cl::CommandQueue queue(context, devices);
        queue.enqueueWriteBuffer(buff,CL_TRUE,0,COLUMN_SIZE*ROW_SIZE*POINT_SIZE, tmp_buffer.data());

         generate_medical_cells(
             p->get_buffer(),
             x_norm, 
             y_norm, 
             radius_squared,
             queue,
             buff
            );
        //p->generate_medical_cells(xpos, ypos,number_of_cells); 
        
        queue.enqueueReadBuffer(buff, CL_TRUE, 0, COLUMN_SIZE*ROW_SIZE*POINT_SIZE, curr_buff);
    }
}

/**r
 * GLFW WINDOW FUNCTIONS
 */

/**
*   @brief Sets the size of the viewport, can also be used to scale w.r.t aspect ratio.
*
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    
    // (left, bottom, left, top) size of the window
    glViewport(0, 0, width, height);
}

/**
* @brief Confirms whether or not window has initialised, if not throws an exception. 
*   
* @param window A pointer to the current window context
*/
void confirm_window_init(GLFWwindow* window)
{
    try
    {
        if(window == NULL)
        {
            throw std::runtime_error("Failed to intialize window.");
        }    
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        glfwTerminate();
    }
}

/** 
*   @brief Check wether or not glad has intialised. Always do this after setting the current glfw window context. 
*/
void confirm_glad_has_loaded()
{
    try
    {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initializa GLAD");
        }    
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

/**
 * GL INITIALISATION FUNCTIONS
 */

/**
*   @brief The following creates and intialises a shader program. 
*
*   @param argument1 Location of the vertex shader w.r.t current working directory
*   @param argument2 Location of the fragment shader w.r.t current workin directory
*/
void initialise_shader_program(char* argument1, char* argument2)
{
    Shader s1(argument1,argument2);

    s1.use();
}

/**
* @brief Draws object
*
* @param object_buffer_id  The Buffer ID of the object data generated by OpenGL
*
*/
void draw_object(unsigned int object_buffer_id)
{
    glBindVertexArray(object_buffer_id);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

/**
 * BUFFER INITIALISATION
 */

/**
 * @brief returns true or false randomly based on a bernoulli distribution
 * 
 * @return true 
 * @return false 
 */
bool randomTrueOrFalse() {
    static std::random_device rd;  

    static std::mt19937 gen(rd()); 

    static std::bernoulli_distribution dis(0.25); // Uniform distribution between 0 and 1

    return dis(gen); 
}

/**
 * @brief Assigns the coordinates and color of the point to be rendered.
 * 
 * @param grid The array of the point object. 
 * @param i index x
 * @param j index y
 * @param x_coord 
 * @param y_coord 
 */
void assign_coords_and_colors(point* &grid, int& i, int& j, float x_coord, float y_coord)
{   
    static int max_no_of_cancer_cells  = 196400;

    point* pixel = (grid + i*COLUMN_SIZE + j);

    //x_coord and y_coord are basically useless, #to_refactor
    pixel->x = x_coord;

    pixel->y = y_coord;

    if(randomTrueOrFalse() && max_no_of_cancer_cells > 0)
    {
        pixel->set_rgba(0.949f, 0.6f, 0.59f, 1.0f);

        pixel->type = 4;

        max_no_of_cancer_cells--;

        observer.increment_value(observer.cancer_cell);
    }
    else
    {   
        pixel->set_rgba(0.757f, 1.0f, 0.808f, 1.0f);

        pixel->type = 0;

        observer.increment_value(observer.normal_cell);
    }


}

/**
 * @brief The following function intialises a blank grid by asigning a point a position on the screen. The buffer starts drawing these points from bottom left.  
 * 
 * @param grid A 2D stl vector representing the 1024x768 grid of pixels
 */
void initialise_grid(point* grid)
{   
    int i = 0, j = 0;

    while(i < 768)
    {   
        float y_coord = i/768.0f*2.0f-1.0f;

        j = 0;

        while(j < 1024)
        {   
            float x_coord = j/1024.0f*2.0f-1.0f;

            assign_coords_and_colors(grid, i, j, x_coord, y_coord);

            j++;   
        }
        
        i++;
    }
}

/**
 * @brief Copies buffer from one location to another.
 * 
 * @param original_grid 
 * @param copy_grid 
 */

void copy_grid(point* original_grid, point* copy_grid)
{
    int i = 0, j = 0;

    while(i < 768)
    {   
        float y_coord = i/768.0f*2.0f-1.0f;

        j = 0;

        while(j < 1024)
        {   
            float x_coord = j/1024.0f*2.0f-1.0f;

            *(copy_grid+i*COLUMN_SIZE+j) = *(original_grid+i*COLUMN_SIZE+j);

            j++;   
        }
        
        i++;
    }
}

/**
 * OPENCL INITIALISATION
 */



cl::Device get_default_device()
{
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  
  if(platforms.empty())
  {
    std::cerr << "No platforms found!" << std::endl;
    exit(1);
  }

  auto platform = platforms.front();
  std::vector<cl::Device> devices_;
  platform.getDevices(CL_DEVICE_TYPE_ALL, &devices_);

  if(devices_.empty()){
    std::cerr << "No devices found!" << std::endl;
    exit(1);
  }
  
  cl::Device default_device = devices_.front();

    // Printing device information
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << std::endl;

    return default_device;
} 

void initialise_device()
{

  //Select device to run
  devices = get_default_device();

  std::ifstream kernel_file("gen_med_call_kernel.cl");
  std::string src(std::istreambuf_iterator<char>(kernel_file), (std::istreambuf_iterator<char>()));

  cl::Program::Sources sources(1, {src.c_str(), src.length() + 1});
  context = cl::Context(devices);
  program = cl::Program(context, sources);

  auto err = program.build();
  if(err != CL_SUCCESS || err != CL_BUILD_SUCCESS)
  {
        std::cerr << "Error!\nBuild Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices) 
        << "\nBuild Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices) << std::endl;
        exit(1);
  }
  kernel = cl::Kernel(program, "gen_med_call_kernel"); 
  kernel_file.close(); 
}



int main(int argc, char *argv[])
{   
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Cancer Cell Simulation",NULL,NULL);
    confirm_window_init(window);
    glfwMakeContextCurrent(window);
    confirm_glad_has_loaded();    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //variables
    int buffer_size =  ROW_SIZE*COLUMN_SIZE*POINT_SIZE*2;

    initialise_shader_program(argv[1], argv[2]);

    //Initialise a pointer to a buffer `point_grid' 
    point *point_grid;

    point_grid = new point[ROW_SIZE*COLUMN_SIZE*2+1];
    
    //Initialise the initial map onto the `point_grid` buffer
    initialise_grid(point_grid);

    //Create a duplicate buffer to allow for buffer swaps.
    copy_grid(point_grid, (point_grid+ROW_SIZE*COLUMN_SIZE));

    //Initial a grid object which encloses most of the logic of the program. 
    points p(&(point_grid)->x,buffer_size,sizeof(point)); 

    //Initialise OpenCL
    initialise_device(); 

    // --- Render Loop ---
    while(!glfwWindowShouldClose(window))
    {   
        auto start = std::chrono::high_resolution_clock::now();
        
        glClearColor(1.0f,1.0f,0.8f,1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_POINTS, 0, COLUMN_SIZE*ROW_SIZE);

        glfwPollEvents();

        p.generate_next_buffer();
        
        processInput(window, &p);

        glfwSwapBuffers(window);

        auto stop = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        //Display commands
        system("clear");

        std::cout << "Time taken by loop: " << duration.count() << " milliseconds" << std::endl;

        std::cout << "fps: " << (float) 1000/duration.count() << std::endl;

        std::cout << "The number of cells right now:" << std::endl;

        std::cout << "Normal Cells:\t" << observer.get_value(observer.normal_cell) << std::endl;

        std::cout << "Cancer Cells:\t" << observer.get_value(observer.cancer_cell) << std::endl;
        
        std::cout << "Medicine Cells:\t" << observer.get_value(observer.medicine_cell) << std::endl;
    }

    glfwTerminate();

    //TODO: garbage collection and freeing up memory. 

    return 0;
}   
