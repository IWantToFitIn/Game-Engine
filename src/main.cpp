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
//temporary
#include<defaultShaderVertex.hpp>
#include<defaultShaderFragment.hpp>

struct Vertex {
	float pos[2];
	float color[3];
};

void transfer(Device& dev, FrameContext& frame, std::span<unsigned char> data, Buffer& dst){
	Buffer trans(dev, data.size(), BufferUsage::Transfer, BufferAccess::HostMutable);
	trans.copyMemory(data);
	auto transCmd = std::move(frame.getTransferBuffers(1)[0]);
	transCmd.begin();
	transCmd.copyBuffer(trans, dst, data.size(), 0);
	transCmd.end();
	VkFence fence = [&](){
		VkFence fence;
		VkFenceCreateInfo fenceInfo ={
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
		};
		vkCreateFence(dev.getDevice(), &fenceInfo, nullptr, &fence);
		return fence;
	}();
	dev.submit(transCmd, fence, {}, {}, 0);
	vkWaitForFences(dev.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(dev.getDevice(), fence, nullptr);
}

struct alignas(16) UBO{
	float offset[4];
	float padding1[4];
	float padding2[4];
	float padding3[4];
};

void uploadImage(Device& dev, FrameContext& frame, std::span<unsigned char> data, Image& dst){
	Buffer trans(dev, data.size(), BufferUsage::Transfer, BufferAccess::HostMutable);
	trans.copyMemory(data);
	auto transCmd = std::move(frame.getTransferBuffers(1)[0]);
	transCmd.begin();
	transCmd.transition(ImageLayout::transferDst, dst);
	transCmd.uploadImage(trans, dst);
	transCmd.transition(ImageLayout::sampling, dst);
	transCmd.end();
	VkFence fence = [&](){
		VkFence fence;
		VkFenceCreateInfo fenceInfo ={
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
		};
		vkCreateFence(dev.getDevice(), &fenceInfo, nullptr, &fence);
		return fence;
	}();
	dev.submit(transCmd, fence, {}, {}, 0);
	vkWaitForFences(dev.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(dev.getDevice(), fence, nullptr);
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
	GraphicsPipeline pipeline(dev, shaders, con.getFormat());
	FrameContext frame(dev);
	std::vector<CommandList> cmds;// = frame.getGraphicsBuffers(gFramesInFlight);
	for(int i = 0; i < gFramesInFlight; i++){
		auto newCmds = frame.getGraphicsBuffers(1);
		cmds.emplace_back(std::move(newCmds[0]));
		frame.finishFrame();
	}
	frame.finishFrame();

	// const std::vector<Vertex> vertices = {
	// 	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	// 	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	// };
	
	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	Buffer vbo(dev, vertices.size() * sizeof(Vertex), BufferUsage::Vertex, BufferAccess::Immutable);
	transfer(dev, frame, {(unsigned char*)vertices.data(), vertices.size() * sizeof(Vertex)}, vbo);
	const std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};
	Buffer ibo(dev, indices.size() * sizeof(uint32_t), BufferUsage::Index, BufferAccess::Immutable);
	transfer(dev, frame, {(unsigned char*)indices.data(), indices.size() * sizeof(uint32_t)}, ibo);

	std::vector<uint32_t> redSquareData(32 * 32, 0xaaff0000);
	auto redSquareImage = Image(dev, 32, 32, 1, VK_FORMAT_R8G8B8A8_SRGB);
	uploadImage(dev, frame, {(unsigned char*)redSquareData.data(), redSquareData.size() * sizeof(uint32_t)}, redSquareImage);
	auto redSquareSampler = Sampler(dev);
	auto texHandle = dev.getBindless().storeTexture(std::move(redSquareImage), std::move(redSquareSampler));

	UBO uboData = {0.5f, 0.0f};
	Buffer ubo(dev, sizeof(UBO), BufferUsage::Storage, BufferAccess::Immutable);
	transfer(dev, frame, {(unsigned char*)&uboData, sizeof(UBO)}, ubo);
	auto uboHandle = dev.getBindless().storeBuffer(std::move(ubo));

	auto beginRecord = [&](Image& image) -> CommandList&{
		static size_t frameIndex{0};
		frameIndex = (frameIndex + 1) % gFramesInFlight;
		auto& cmd = cmds[frameIndex];

		cmd.begin();
		cmd.transition(ImageLayout::attachment, image);

		return cmd;
	};

	auto draw = [&](Image& image, CommandList& cmd){
		cmd.beginRender(image);
		cmd.bindGraphicsPipeline(pipeline);
		cmd.setViewPort(1080, 720, 0, 0);
		cmd.setScissor(1080, 720, 0, 0);
		cmd.bindDescriptor(0, dev.getBindless().getSet());
		cmd.pushConstant("Index", uboHandle);
		cmd.pushConstant("TextureIndex", texHandle);
		cmd.bindVertexBuffer(vbo);
		cmd.bindIndexBuffer(ibo);
		cmd.drawIndexed(indices.size());
		cmd.endRender();
	};

	auto endRecord = [&](Image& image, CommandList& cmd){
		cmd.transition(ImageLayout::present, image);
		cmd.end();
	};

	while(win.process()){
		frame.prepareFrame();

		auto fence = frame.getFence();
		auto imageSemaphore = frame.getSemaphore();
		con.popNextImage(imageSemaphore);
		auto image = con.getImage();
		auto& renderSemaphore = con.getSemaphore();

		auto& cmd = beginRecord(image);
		draw(image, cmd);
		endRecord(image, cmd);
		dev.submit(cmd, fence, { &imageSemaphore, 1 }, { &renderSemaphore, 1 }, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		con.present();
		
		frame.finishFrame();
	}
	dev.waitTillIdle();
}