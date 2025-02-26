![image](https://github.com/user-attachments/assets/6385a17a-7f40-49de-a20b-c380085c55b7)
[![OpenCL](https://img.shields.io/badge/OpenCL-Khronos-blue)](https://www.khronos.org/opencl/)
[![oneTBB](https://img.shields.io/badge/oneTBB-Intel-blue)](https://oneapi.intel.com/tbb)
[![CMake](https://img.shields.io/badge/CMake-Official-blue)](https://cmake.org/)

### Simple Cancer Cell Simulator

Simple simulator built to compare performance of software multi-threading, hardware multi-threading and GPU kernels. 
Cancer cells are represented by the red color and healthy cells are green, they steal nutrients from healty cells and form clusters. Medicinal cell are yellow, they consume cancer cells and exit through the corners. 

### Results

GPU kernels written in openCL provide the most performance boost if a performant GPU is present, otherwise CPU  hardware multi-threading on separate cores provides the best performance boost. 
With intel oneTBB, hardware multi-threading is explicitly specified otherwise the OS usually defaults to scheduling threads at a software level for concurrency and a generic scheduler is used at the hardware level. 

![image](https://github.com/user-attachments/assets/42251200-7a44-4389-a9fa-22702f508c9c)

### Code Structure
```
├── build/              # CMake build directory (generated files)
├── CMakeLists.txt      # Main CMake build configuration
├── demo.cpp            # Main entry point (contains main() function)
├── fragmentShader.glsl # GLSL fragment shader source
├── gen_med_call_kernel.cl # OpenCL kernel file for compute tasks
├── glad.c              # OpenGL loader library source (manages OpenGL function pointers)
├── point.cpp           # Implementation for point-related functionality
├── points.cpp          # manages points
├── scheduler.cpp       # Logic for scheduling tasks or coordinating events
├── scheduler.h         # Header for scheduler declarations
├── shader.cpp          # Implementation for shader loading/compilation
├── shader.h            # Declarations for shader utilities
├── shapes.cpp          # Implementation for shapes or geometry primitives
├── shapes.h            # Declarations for shapes (Rectangle, etc.)
├── test/               # Directory for tests (if any)
├── tracker.cpp         # Logic for tracking data/state
├── tracker.h           # Declarations for the tracker
├── value_tracker.cpp   # Similar to tracker.cpp but for specific value tracking
├── value_tracker.h     # Header for value-tracking declarations
└── vertexShader.glsl   # GLSL vertex shader source
```
