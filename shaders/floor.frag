#version 330 core

in vec4 face_normal;
in vec4 light_direction;
in vec4 world_position;

out vec4 fragment_color;

void main() {
  vec4 n = vec4(0.0f, 1.0f, 0.0f, 0.0f);
  vec4 pos = world_position;
  float check_width = 0.25;
  float i = floor(pos.x / check_width);
  float j  = floor(pos.z / check_width);
  vec3 color = mod(i + j, 2) * vec3(1.0, 1.0, 1.0);
  float dot_nl = dot(normalize(light_direction), normalize(n));
  dot_nl = clamp(dot_nl, 0.0, 1.0);
  color = clamp(dot_nl * color, 0.0, 1.0);
  fragment_color = vec4(color, 1.0);
}
