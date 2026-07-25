#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(std140, set = 0, binding = 0) uniform bindlessUniforms{
	vec4 offset;
	vec4 padding1;
	vec4 padding2;
	vec4 padding3;
} unis[];
layout(std430, set = 0, binding = 1) readonly buffer bindlessStorages{
	vec4 offset;
	vec4 padding1;
	vec4 padding2;
	vec4 padding3;
} stores[];

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(push_constant) uniform PushConstants {
	uint Index;
};

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = vec4(inPosition, 0.0, 1.0) + stores[Index].offset;
	fragColor = inColor;
}