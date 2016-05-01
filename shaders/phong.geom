#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

in vec4 vs_light_direction[];
in vec4 vs_vertex_normal[];

out vec4 face_normal;
out vec4 light_direction;

void main() {
  for (int i = 0; i < gl_in.length(); i++) {
    face_normal = model * vs_vertex_normal[i];
    light_direction = normalize(vs_light_direction[i]);
    gl_Position = projection * view * model * gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
