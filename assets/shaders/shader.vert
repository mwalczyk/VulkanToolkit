#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

layout(std430, push_constant) uniform PushConstants
{
	float time;
	vec2 mouse;
	vec3 color;
} constants;

// Vertex shader inputs
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

// Vertex shader outputs
out gl_PerVertex
{
	vec4 gl_Position;
};

layout (location = 0) out vec3 vsColor;

void main()
{
	vsColor = inColor;
  gl_Position = ubo.projection * ubo.model * vec4(inPosition, 1.0);
}
