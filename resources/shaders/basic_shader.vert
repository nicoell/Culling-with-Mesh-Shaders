#version 410
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec3 color;

uniform mat4 mvp;

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

void main() 
{
  color = 0.5*normal + 0.5; 
  gl_Position = mvp * vec4(position, 1);
}
