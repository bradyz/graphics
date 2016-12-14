#version 330 core

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
  gl_Position = projection * view * model * gl_in[0].gl_Position;
  EmitVertex();
  gl_Position = projection * view * model * gl_in[1].gl_Position;
  EmitVertex();
  EndPrimitive();
}
