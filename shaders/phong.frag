#version 330 core

uniform vec4 obj_color;
uniform vec4 eye;

in vec4 world_pos;
in vec4 normal;
in vec4 light;

out vec4 fragment_color;

void main() {
  vec4 to_eye = eye - world_pos;
  float dot_nl = dot(normalize(light), normalize(normal));

  float specular = clamp(dot(normalize(normal), normalize(to_eye)), 0.0f, 1.0f);
  specular = pow(specular, 100);

  vec3 diffuse = vec3(obj_color.xyz) * clamp(dot_nl, 0.0f, 1.0f);

  vec3 color = diffuse + specular * diffuse; 

  fragment_color = clamp(vec4(color, obj_color.w), 0.0f, 1.0f);
}
