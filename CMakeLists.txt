cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

set (glm_DIR "./src/GLM/lib/cmake/glm")
set (glfw3_DIR "./src/GLFW/lib/cmake/glfw3")

project (finalproject)

if(MSVC)
  add_definitions(/DNOMINMAX)
endif()

if(UNIX)
    message(STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message(STATUS "Setting GLVND option.")
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package( glm CONFIG REQUIRED )
find_package( glfw3 CONFIG REQUIRED )
find_package( OpenGL REQUIRED )

include_directories( ./src/ingredients )

add_subdirectory( ./src/ingredients )

set(target finalproject)
set( finalproject_SOURCES
	./src/main.cpp
	./src/glad.o
	)
add_executable( ${target} ${finalproject_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		GLFW_INCLUDE_NONE
		)

target_link_libraries( ${target}
		PRIVATE
		glfw
		${OPENGL_gl_LIBRARY}
		)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})