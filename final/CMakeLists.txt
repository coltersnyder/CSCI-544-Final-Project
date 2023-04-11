set(target final)

set(SOURCES
	scenebasic.cpp
   main.cpp)

add_executable( ${target} ${SOURCES} )

target_compile_definitions(${target}
      PRIVATE
      ${DEFAULT_COMPILE_DEFINITIONS}
      GLFW_INCLUDE_NONE
      )

target_link_libraries( ${target}
        PRIVATE
        ingredients
      glfw
        ${OPENGL_gl_LIBRARY}
      )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
