// Shader-ul de varfuri / Vertex shader
#version 330 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec4 in_Color;

uniform mat4 myMatrix;
out vec4 ex_Color;

void main ()
{
   gl_Position = myMatrix * in_Position;
   ex_Color = in_Color;
}