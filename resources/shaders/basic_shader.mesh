#version 450
#extension GL_NV_mesh_shader : require
#extension GL_ARB_separate_shader_objects : require

#define MAX_VER  96
#define MAX_PRIM 32

// Number of threads per workgroup
// - always 1D
// - max is warpsize = 32
layout(local_size_x=32) in;
// Primitive Type
// - points, lines or triangles
layout(triangles) out;
// max allocation size for each meshlet
layout(max_vertices=MAX_VER) out;
layout(max_primitives=MAX_PRIM) out;

// The amount of primitives the workgroup outputs
// - can be less than max_primitives ( <= max_primitives)
//out uint gl_PrimitiveCountNV;
// An index buffer, using list type indices
// - [max_primitives x 3 (triangles) / 2 (lines) / 1 (points)]
//out uint gl_PrimitiveIndicesNV[MAX_PRIM*3]; 

// Need to declare gl_PerVertex because of ARB_separate_shader_objects
out gl_PerVertex
{
  vec4 gl_Position;
  //float gl_PointSize;
  //float gl_ClipDistance[];
  //float gl_CullDistance[];
} gl_stupid;

out gl_MeshPerVertexNV {
  vec4 gl_Position;  
  //float gl_PointSize;
  //float gl_ClipDistance[];
  //float gl_CullDistance[];
} gl_MeshVerticesNV[MAX_VER];

out perVertexData {
  vec3 color;
} customData[MAX_VER];

uniform mat4 mvp;

// special purpose per-primitive outputs
//perprimitiveNV out gl_MeshPerPrimitiveNV {
//  int gl_PrimitiveID;
//  int gl_Layer;
//  int gl_ViewportIndex;
//  int gl_ViewportMask[];          // [1]
//} gl_MeshPrimitivesNV[];

layout(binding = 0, std430) readonly buffer vertex_ssbo
{
	vec4 vertices[];
};

layout(binding = 1, std430) readonly buffer normal_ssbo
{
	vec4 normals[];
};

layout(binding = 2, std430) readonly buffer index_ssbo
{
	uint indices[];
};


void main()
{
  uint lid = gl_LocalInvocationID.x;
  uint gid = gl_GlobalInvocationID.x;

  uint base_index = gid * 3;

  uvec3 face_indices = uvec3(indices[base_index], indices[base_index + 1], indices[base_index + 2]);

  uint vertex_index = lid * 3;

  gl_MeshVerticesNV[vertex_index].gl_Position = mvp * vertices[face_indices.x];
  gl_MeshVerticesNV[vertex_index + 1].gl_Position = mvp * vertices[face_indices.y];
  gl_MeshVerticesNV[vertex_index + 2].gl_Position = mvp * vertices[face_indices.z];

  customData[vertex_index].color = 0.5 * normals[face_indices.x].xyz + 0.5;
  customData[vertex_index + 1].color = 0.5 * normals[face_indices.y].xyz + 0.5;
  customData[vertex_index + 2].color = 0.5 * normals[face_indices.z].xyz + 0.5;

  gl_PrimitiveIndicesNV[vertex_index] = vertex_index;
  gl_PrimitiveIndicesNV[vertex_index + 1] = vertex_index + 1;
  gl_PrimitiveIndicesNV[vertex_index + 2] = vertex_index + 2;

  //writePackedPrimitiveIndices4x8NV
  
  if (gl_LocalInvocationID.x == 0) {
    gl_PrimitiveCountNV = 32;
  }
}