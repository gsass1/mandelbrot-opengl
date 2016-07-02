# Mandelbrot Renderer

Real-time Mandelbrot Fractal renderer using OpenGL 4.1 and double-precision GLSL.

## Compilation

You need to install cmake, a C++ compiler, GLEW, GLFW3 and OpenGL libraries.


```
mkdir -p build && cd build
cmake ..
make
```

Then you need to copy shader.glsl to the executable.

```
copy ../shader.glsl .
./Mandelbrot
```

## Keys
* WASD: control camera position
* -: increase iterations
* =: decrease iterations
* Mouse wheel: control zoom
