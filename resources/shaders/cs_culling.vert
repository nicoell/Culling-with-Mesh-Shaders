#version 460

#extension GL_NV_mesh_shader : require
#extension GL_NV_shader_thread_group : require

// Structs and Config

#define GPU_WARP_SIZE 32
#define GPU_GROUP_SIZE GPU_WARP_SIZE


// Number of threads per workgroup
// - always 1D
// - max is warpsize = 32
layout(local_size_x=GPU_GROUP_SIZE) in;


#define GPU_PRIMITIVES_PER_MESHLET GPU_GROUP_SIZE

#define GPU_MESHLETS_PER_INVOCATION 3
#define GPU_MESHLETS_PER_WORKGROUP (GPU_GROUP_SIZE * GPU_MESHLETS_PER_INVOCATION)

#define GPU_INDICES_PER_PRIMITIVE 3

#define GPU_INDEX_BUFFER_BINDING 0
#define GPU_VERTEX_DESCRIPTOR_BINDING 1
#define GPU_MESHLET_DESCRIPTOR_BINDING 2

const float pi_half = 1.57079;

struct BoundingSphere
{
  vec3 center;
  float radius;
};

struct BoundingCone
{
  vec3 normal;
  float angle;
};

struct MeshletDescriptor
{
  BoundingSphere sphere;
  BoundingCone cone;
  uint primitive_count;
};

layout(binding = GPU_MESHLET_DESCRIPTOR_BINDING, std430) readonly buffer meshlet_descriptor_ssbo
{
	MeshletDescriptor meshlet_descriptors[];
};


uniform bool disable_frustum_culling;
uniform bool disable_backface_culling;

uniform vec4 frustum_planes[6]; //obj space
uniform vec3 view_origin; //obj space
//uniform vec3 view_up; //obj space
//uniform vec3 view_right; //obj space

uniform float debug_value;

taskNV out Task
{
  uint   primitive_base_id;
  uint   primitive_offsets[GPU_GROUP_SIZE];
  uint   meshlet_ids[GPU_GROUP_SIZE];
} OUT;

bool isFrontFaceVisible(
  vec3 sphere_center, float sphere_radius,
  vec3 cone_normal, float cone_angle)
{
  vec3 sphere_center_dir = normalize(sphere_center - view_origin);

  float sin_cone_angle = sin(min(cone_angle, pi_half)) + debug_value; //TODO: precompute offline
  return dot(cone_normal, sphere_center_dir) < sin_cone_angle;
}

bool isSphereOutsideFrustum(
  vec3 sphere_center, float sphere_radius)
{
  bool isInside = false;
  for(int i = 0; i < 6; i++)
  {
    isInside = isInside || (dot(frustum_planes[i].xyz, sphere_center) + frustum_planes[i].w < -sphere_radius);
  }
  return isInside;
}

// Exact version considering sphere radius 
//bool isFrontFaceVisibleExact(
//  vec3 sphere_center, float sphere_radius,
//  vec3 cone_normal, float cone_angle)
//{
//  vec3 sphere_center_dir = normalize(sphere_center - view_origin);
//
//  vec3 right_offset = sphere_radius * cross(sphere_center, view_up);
//  vec3 up_offset = sphere_radius * cross(sphere_center, view_right);
//
//  vec3 sphere_bound_dirs[4] = vec3[4](
//    normalize((sphere_center - right_offset) - view_origin),
//    normalize((sphere_center + up_offset)    - view_origin),
//    normalize((sphere_center + right_offset) - view_origin),
//    normalize((sphere_center - up_offset)    - view_origin)
//  );
//
//  float angular_span = 0.0;
//  for(int i = 0; i < 4; i++)
//  {
//    float temp = dot(sphere_center_dir, sphere_bound_dirs[i]);
//    temp = temp * temp;
//    temp = sqrt(1 - temp);// = sin(acos(dot(a,b)))
//    angular_span = max(angular_span, temp); 
//  }
//
//  float sin_cone_angle = sin(min(cone_angle, pi_half)) + debug_value; //TODO: precompute offline
//  return (dot(cone_normal, sphere_center_dir) - angular_span) < sin_cone_angle;
//}


void main()
{
  uint meshlet_id = gl_GlobalInvocationID.x;
  uint workgroup_meshlet_base_id = gl_WorkGroupID.x * GPU_GROUP_SIZE;
  uint workgroup_primitive_base_id = workgroup_meshlet_base_id * GPU_PRIMITIVES_PER_MESHLET;
  uint local_primitive_offset = gl_LocalInvocationID.x * GPU_PRIMITIVES_PER_MESHLET;

  MeshletDescriptor desc = meshlet_descriptors[meshlet_id];

  bool render = (disable_frustum_culling || !isSphereOutsideFrustum(desc.sphere.center, desc.sphere.radius)) &&
                (disable_backface_culling || isFrontFaceVisible(desc.sphere.center, desc.sphere.radius,
                                   desc.cone.normal, desc.cone.angle));

  uint warp_bitfield = ballotThreadNV(render);
  uint task_count = bitCount(warp_bitfield);

  if (gl_LocalInvocationID.x == 0)
  {
    OUT.primitive_base_id = workgroup_primitive_base_id;
    gl_TaskCountNV = task_count;
  }

  uint task_out_index = bitCount(warp_bitfield & gl_ThreadLtMaskNV);
  if (render)
  {
    OUT.primitive_offsets[task_out_index] = local_primitive_offset;
    OUT.meshlet_ids[task_out_index] = meshlet_id;
  }
}
