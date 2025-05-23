#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
  
out vec3 ourColor; // output a color to the fragment shader

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}       