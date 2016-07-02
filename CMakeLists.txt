cmake_minimum_required( VERSION 2.6 )
project( MandelbotRenderer )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" )


find_package( GLEW REQUIRED )
find_package( GLFW3 REQUIRED )
find_package( OpenGL REQUIRED )

set( SRC mandelbrot.cpp )
set( LIBRARIES
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARIES}
    ${GLFW3_LIBRARY}
)

add_executable( Mandelbrot ${SRC} )
target_link_libraries( Mandelbrot ${LIBRARIES} )
