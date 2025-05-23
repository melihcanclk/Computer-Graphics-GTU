# Project Description
This project is a simple OpenGL project that user can move in a 3D space without any collision and can see a 3D model in the space.\

# Project Structure
- test.cpp: the main file of the project
- shaders folder: contains the shaders used in the project
- models folder: contains the 3D model used in the project
- assets folder: contains the textures used in the project
- vs folder: contains the vertex shaders
- fs folder: contains the fragment shaders
- makefile: the makefile of the project
- glm folder: the glm library that is used in the project for matrix and vector operations (already included)
- glad.h and glad.c: the glad files
- stb_image.h: the stb_image file that is used to load textures
- camera.h: the camera class
- mesh.h: the mesh class
- model.h: the model class

# how to build the project
```bash
make
```
# to run the project
```bash
./main
```
# to clean the project
```bash
make clean
```

# - install GLFW3
### for Linux/Ubuntu:
```bash
sudo apt-get install libglfw3-dev
```
### for Mac:
```bash
brew install glfw3
```

### for Windows:
[https://learnopengl.com/Getting-started/Creating-a-window]\
You can follow this tutorial to install GLFW3 on Windows with Visual Studio.

# - install glad
There is a website to generate glad files: [https://glad.dav1d.de/]\
Make sure to select the following options:
- Language: C/C++
- Specification: OpenGL
- gl: Version 3.0
- Profile: Core
- Options: Generate a loader


# - (glm already included) but install it if you want;
To install glm, you can follow this tutorial: [https://glm.g-truc.net/0.9.9/index.html]\
You can download the source code from the website, and then follow the instructions in the README file.

### If you install glad, make sure to change include paths in the source code.
# - for the mesh viewer, install assimp from source;
    for build assimp for Linux/Ubuntu:
### Get the source
Make sure you have a working git-installation. Open a command prompt and clone the Asset-Importer-Lib via:
```bash
git clone https://github.com/assimp/assimp.git
```
### Build from source:
```bash
cd assimp
cmake CMakeLists.txt 
cmake --build .
```

### Install
```bash
sudo make install
```

It should be installed in /usr/local/lib and /usr/local/include/assimp
in the makefile, -lassimp is already included, so you don't need to change anything.
