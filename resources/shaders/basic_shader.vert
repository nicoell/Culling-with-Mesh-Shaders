#version 450
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

layout(location = 0) out vec3 color;

uniform mat4 mvp;
uniform mat4 model_matrix_transpose_inverse;

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

void main() 
{
  color = 0.5 * (model_matrix_transpose_inverse * normal).xyz + 0.5; 
  gl_Position = mvp * position;
}
