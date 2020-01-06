cmake_minimum_required(VERSION 3.5)

project("glfw_shader")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC video_light.cpp input.cpp render.cpp shader.cpp display.cpp main.cpp)
  
add_executable(glfw_shader ${SRC} )

#target_include_directories(playfield BEFORE PUBLIC /usr/include)
#target_include_directories(glfw_shader BEFORE PUBLIC glfw/include)
target_include_directories(glfw_shader BEFORE PUBLIC stb)
target_link_libraries(glfw_shader avcodecmm avformatmm avutilmm swscalemm swresamplemm X11 GL GLEW /usr/lib/x86_64-linux-gnu/libglfw.so.3.2)






