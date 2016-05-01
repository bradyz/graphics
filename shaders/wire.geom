#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
  for (int i = 0; i <= gl_in.length(); i++) {
    gl_Position = projection * view * model * gl_in[i % gl_in.length()].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
