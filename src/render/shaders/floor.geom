#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 view;

in vec4 vs_light_direction[];

out vec4 light_direction;
out vec4 world_position;

void main() {
  for (int n = 0; n < gl_in.length(); n++) {
    light_direction = normalize(vs_light_direction[n]);
    gl_Position = projection * view * gl_in[n].gl_Position;
    world_position = gl_in[n].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
