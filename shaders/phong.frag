#version 330 core

uniform vec4 obj_color;

in vec4 face_normal;
in vec4 light_direction;

out vec4 fragment_color;

void main() {
  float dot_nl = dot(normalize(light_direction), normalize(face_normal));
  vec3 color = vec3(obj_color.xyz) * clamp(dot_nl, 0.0f, 1.0f);
  fragment_color = vec4(color, obj_color.w);
}
