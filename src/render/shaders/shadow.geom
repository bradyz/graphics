#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 shadow;

void main() {
  for (int i = 0; i < gl_in.length(); i++) {
    vec4 pos = shadow * model * gl_in[i].gl_Position;
    pos /= pos.w;
    gl_Position = projection * view * pos;
    EmitVertex();
  }
  EndPrimitive();
}
