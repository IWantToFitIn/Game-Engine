#include<iostream>
#include<log.hpp>
#include<window.hpp>
#include<device.hpp>
#include<renderContext.hpp>
#include<graphicsPipeline.hpp>
#include<commandPool.hpp>
#include<frameContext.hpp>
#include<buffer.hpp>
#include<sampler.hpp>
#include<pipelineBuilder.hpp>
//temporary
#include<defaultShaderVertex.hpp>
#include<defaultShaderFragment.hpp>
#include<fastgltf/core.hpp>
#include<fastgltf/tools.hpp>
#include<fastgltf/glm_element_traits.hpp>
#include<fastgltf/util.hpp>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

struct MeshData{
	std::vector<uint32_t> indices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec4> colors;
};

struct InstanceData{
	uint32_t meshIndex;
	glm::mat4 transform;
};

struct ModelData{
	std::vector<MeshData> meshes;
	std::vector<InstanceData> instances;
};

void transfer(Device& dev, ExecutionStream& str, FrameContext& frame, std::span<const std::byte> data, Buffer& dst){
	Buffer trans(dev, data.size(), BufferUsage::Transfer, BufferAccess::HostMutable);
	trans.copyMemory(data);
	auto transCmd = std::move(frame.getTransferBuffers(1)[0]);
	transCmd.begin();
	transCmd.copyBuffer(trans, dst, data.size(), 0);
	transCmd.end();
	auto waitToken = str.acquireNextToken();
	dev.submit(transCmd, {}, {&waitToken, 1});
	dev.waitOnToken(waitToken);
}

struct alignas(16) UBO{
	float offset[4];
	float padding1[4];
	float padding2[4];
	float padding3[4];
};

void uploadImage(Device& dev, ExecutionStream& str, FrameContext& frame, std::span<const std::byte> data, Image& dst){
	Buffer trans(dev, data.size(), BufferUsage::Transfer, BufferAccess::HostMutable);
	trans.copyMemory(data);
	auto transCmd = std::move(frame.getTransferBuffers(1)[0]);
	transCmd.begin();
	transCmd.transition(ImageLayout::transferDst, dst);
	transCmd.uploadImage(trans, dst);
	transCmd.transition(ImageLayout::sampling, dst);
	transCmd.end();
	auto waitToken = str.acquireNextToken();
	dev.submit(transCmd, {}, {&waitToken, 1});
	dev.waitOnToken(waitToken);
}

template<typename T>
bool loadGltfAttribute(fastgltf::Asset& asset, fastgltf::Primitive& primitive, std::vector<T>& out, uint32_t initialIndex, std::string_view attribute){
	auto attr = primitive.findAttribute(attribute);
	if(attr == primitive.attributes.end())
		return false;
	auto& accessor = asset.accessors[attr->accessorIndex];
	out.resize(out.size() + accessor.count);
	fastgltf::iterateAccessorWithIndex<T>(asset, accessor, [&](T a, size_t index){
		out[index + initialIndex] = a;
	});
	return true;
}

std::optional<ModelData> loadGltf(std::filesystem::path path){
	fastgltf::Parser parser;
	auto data = fastgltf::GltfDataBuffer::FromPath(path);
	if(data.error() != fastgltf::Error::None)
		return std::nullopt;
	auto asset = parser.loadGltf(data.get(), path.parent_path(), fastgltf::Options::None);
	if(asset.error() != fastgltf::Error::None)
		return std::nullopt;

	ModelData ret;
	for(auto& mesh : asset->meshes){
		MeshData meshData;
		for(auto& primitive : mesh.primitives){
			uint32_t initialVertex = meshData.positions.size();
			
			auto& indexAccessor = asset->accessors[primitive.indicesAccessor.value()];
			meshData.indices.reserve(meshData.indices.size() + indexAccessor.count);
			fastgltf::iterateAccessor<uint32_t>(asset.get(), indexAccessor, [&](uint32_t index){
				meshData.indices.push_back(index + initialVertex);
			});
		
			if(!loadGltfAttribute(asset.get(), primitive, meshData.positions, initialVertex, "POSITION"))
				LOG_WARN << "failed to load vertex positions";
			if(!loadGltfAttribute(asset.get(), primitive, meshData.normals, initialVertex, "NORMAL"))
				LOG_WARN << "failed to load vertex normals";
			if(!loadGltfAttribute(asset.get(), primitive, meshData.colors, initialVertex, "COLOR_0"))
				LOG_WARN << "failed to load vertex colors";
			if(!loadGltfAttribute(asset.get(), primitive, meshData.uvs, initialVertex, "TEXCOORD_0"))
				LOG_WARN << "failed to load texture coordinates";
		}
		ret.meshes.push_back(meshData);
	}
	fastgltf::iterateSceneNodes(asset.get(), 0, fastgltf::math::fmat4x4(), [&](fastgltf::Node& node, fastgltf::math::fmat4x4 matrix){
		if(node.meshIndex)
			ret.instances.emplace_back(InstanceData{
				.meshIndex = static_cast<uint32_t>(node.meshIndex.value()),
				.transform = glm::make_mat4(matrix.data())
			});
	});
	return ret;
}

struct Mesh{
	uint32_t indexCount;
	Buffer indices;
	Buffer positions;
	Buffer normals;
	// Buffer colors;
	Buffer uvs;
};

struct Instance{
	Buffer instancedData;
	uint32_t instanceCount;
	uint32_t meshIndex;
};

struct Model{
	std::vector<Mesh> meshes;
	std::vector<Instance> instances;
};

Model uploadModel(ModelData data, Device& dev, ExecutionStream& transferStream, FrameContext& frame){
	Model ret;
	for(auto& mesh : data.meshes){
		Buffer indices(dev, mesh.indices.size() * sizeof(uint32_t), BufferUsage::Index, BufferAccess::Immutable);
		transfer(dev, transferStream, frame, std::as_bytes(std::span{mesh.indices}), indices);

		Buffer positions(dev, mesh.positions.size() * sizeof(glm::vec3), BufferUsage::Vertex, BufferAccess::Immutable);
		transfer(dev, transferStream, frame, std::as_bytes(std::span{mesh.positions}), positions);
		
		Buffer normals(dev, mesh.normals.size() * sizeof(glm::vec3), BufferUsage::Vertex, BufferAccess::Immutable);
		transfer(dev, transferStream, frame, std::as_bytes(std::span{mesh.normals}), normals);
		
		// Buffer colors(dev, mesh.colors.size() * sizeof(glm::vec4), BufferUsage::Vertex, BufferAccess::Immutable);
		// transfer(dev, transferStream, frame, std::as_bytes(std::span{mesh.colors}), colors);
		
		Buffer uvs(dev, mesh.uvs.size() * sizeof(glm::vec2), BufferUsage::Vertex, BufferAccess::Immutable);
		transfer(dev, transferStream, frame, std::as_bytes(std::span{mesh.uvs}), uvs);
		
		ret.meshes.emplace_back(Mesh{static_cast<uint32_t>(mesh.indices.size()),
			std::move(indices), std::move(positions), std::move(normals), /*std::move(colors), */std::move(uvs)
		});
	}

	std::unordered_map<uint32_t, std::vector<glm::mat4>> instancedTransforms;
	for(auto& instance : data.instances)
		instancedTransforms[instance.meshIndex].emplace_back(instance.transform);
	for(auto& [meshIndex, transform] : instancedTransforms){
		Buffer instancedData(dev, transform.size() * sizeof(glm::mat4), BufferUsage::Vertex, BufferAccess::Immutable);
		transfer(dev, transferStream, frame, std::as_bytes(std::span{transform}), instancedData);
		ret.instances.emplace_back(Instance{
			.instancedData = std::move(instancedData),
			.instanceCount = static_cast<uint32_t>(transform.size()),
			.meshIndex = meshIndex
		});
	}
	
	return ret;
}

int main(){
	initLogger();
	setFilter(LogSeverity::debug);
	setFilter(LogSeverity::warning, "RHI");

	Window win("test", 1080, 720);
	win.show(true);

	uint32_t c;
	auto ex = win.getExtensions(c);
	std::vector<char const*> extensions(ex, ex + c);
	VkSurfaceKHR surf{};
	Device dev(extensions, [&win, &surf](VkInstance& instance) -> VkSurfaceKHR& {
		surf = createSurface(instance, win.getInternal());
		return surf;
	});
	RenderContext con(dev, std::move(surf), 1080, 720);
	std::vector<Shader> shaders{};
	shaders.emplace_back(dev, gDefaultshadervertex);
	shaders.emplace_back(dev, gDefaultshaderfragment);
	GraphicsPipelineBuilder builder;
	builder.setBindless(true)
		.addShader(shaders[0])
		.addShader(shaders[1])
		.setRenderContext(con)
		.addBinding(0, 1)
		.addBinding(1, 1)
		.addBinding(2, 1)
		.addBinding(3, 4, true);
	auto pipeline = builder.getResult(dev);
	FrameContext frame(dev);
	ExecutionStream transferStream(dev, {VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT }, 3);
	ExecutionStream graphicsStream(dev, {VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT}, 3);

	Model model;
	if(auto data = loadGltf("ABeautifulGame.glb"))
		model = uploadModel(data.value(), dev, transferStream, frame);

	auto beginRecord = [&](Image& image, CommandList& cmd){
		cmd.begin();
		cmd.transition(ImageLayout::attachment, image);
	};
	auto view = glm::lookAt(glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
	auto proj = glm::perspective(glm::radians(45.0f), 1080.0f / 720.0f, 0.1f, 100.0f);
	proj[1][1] *= -1;
	auto transform =  proj * view;

	auto draw = [&](Image& image, CommandList& cmd){
		cmd.beginRender(image);
		cmd.bindGraphicsPipeline(pipeline);
		cmd.setViewPort(1080, 720, 0, 0);
		cmd.setScissor(1080, 720, 0, 0);
		cmd.bindDescriptor(0, dev.getBindless().getSet());
		cmd.pushConstant("trans", transform);
		for(auto& instance : model.instances){
			auto& mesh = model.meshes[instance.meshIndex];
			cmd.bindVertexBuffer(mesh.positions, 0);
			cmd.bindVertexBuffer(mesh.normals, 1);
			cmd.bindVertexBuffer(mesh.uvs, 2);
			cmd.bindIndexBuffer(mesh.indices);
			cmd.bindVertexBuffer(instance.instancedData, 3);
			cmd.drawIndexed(mesh.indexCount, instance.instanceCount);
		}
		cmd.endRender();
	};

	auto endRecord = [&](Image& image, CommandList& cmd){
		cmd.transition(ImageLayout::present, image);
		cmd.end();
	};

	while(win.process()){
		frame.prepareFrame();
		
		auto imageToken = con.popNextImage();
		auto image = con.getImage();
		auto presentToken = con.getRenderFinishedToken();
		auto workFinishedToken = graphicsStream.acquireNextToken();

		//memory leak, probably somewhere here
		//frame Context should be removed
		auto cmd = std::move(frame.getGraphicsBuffers(1)[0]);

		beginRecord(image, cmd);
		draw(image, cmd);
		endRecord(image, cmd);
		std::array<SyncToken, 2> signalTokens = {presentToken, workFinishedToken};
		dev.submit(cmd, {&imageToken, 1}, signalTokens);
		con.present();
		
		frame.finishFrame(workFinishedToken);
		graphicsStream.finalizePass();
	}
	dev.waitTillIdle();
}