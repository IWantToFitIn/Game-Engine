#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 2) uniform sampler2D bindlessTextures[];

layout(location = 0) in vec3 fragColor;

layout(push_constant) uniform PushConstants {
	layout(offset = 4) uint TextureIndex;
};

layout(location = 0) out vec4 outColor;

void main(){
	//outColor = vec4(fragColor, 1.0);
	outColor = texture(bindlessTextures[TextureIndex], fragColor.xy);
}