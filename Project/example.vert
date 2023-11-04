// Shader-ul de varfuri / Vertex shader
#version 330 core

layout (location = 0) in vec4 in_Position;

uniform mat4 myMatrix;
uniform vec4 myColor;
out vec4 ex_Color;

void main ()
{
   gl_Position = myMatrix * in_Position;
   ex_Color = myColor;
}