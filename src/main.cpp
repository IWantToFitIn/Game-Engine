#include<iostream>
#include<log.hpp>
#include<window.hpp>
#include<device.hpp>
#include<renderContext.hpp>
#include<graphicsPipeline.hpp>
#include<commandPool.hpp>
#include<frameContext.hpp>
#include<buffer.hpp>
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

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	Buffer vbo(dev, vertices.size() * sizeof(Vertex), BufferUsage::Vertex, BufferAccess::Immutable);
	transfer(dev, frame, {(unsigned char*)vertices.data(), vertices.size() * sizeof(Vertex)}, vbo);
	const std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};
	Buffer ibo(dev, indices.size() * sizeof(uint32_t), BufferUsage::Index, BufferAccess::Immutable);
	transfer(dev, frame, {(unsigned char*)indices.data(), indices.size() * sizeof(uint32_t)}, ibo);

	auto beginRecord = [&](VkImage& image) -> CommandList&{
		static size_t frameIndex{0};
		frameIndex = (frameIndex + 1) % gFramesInFlight;
		auto& cmd = cmds[frameIndex];

		cmd.begin();
		cmd.transition(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, image);

		return cmd;
	};

	auto draw = [&](VkImageView& view, CommandList& cmd){
		cmd.beginRender(view);
		cmd.bindGraphicsPipeline(pipeline);
		cmd.setViewPort(1080, 720, 0, 0);
		cmd.setScissor(1080, 720, 0, 0);
		cmd.bindVertexBuffer(vbo);
		cmd.bindIndexBuffer(ibo);
		float offset[] = { 0.5f, 0.0f };
		cmd.pushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0, {reinterpret_cast<std::byte*>(offset), sizeof(offset)});
		cmd.drawIndexed(indices.size());
		cmd.endRender();
	};

	auto endRecord = [&](VkImage& image, CommandList& cmd){
		cmd.transition(VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, image);
		cmd.end();
	};

	while(win.process()){
		frame.prepareFrame();

		auto fence = frame.getFence();
		auto imageSemaphore = frame.getSemaphore();
		con.popNextImage(imageSemaphore);
		auto& image = con.getImage();
		auto& view = con.getView();
		auto& renderSemaphore = con.getSemaphore();

		auto& cmd = beginRecord(image);
		draw(view, cmd);
		endRecord(image, cmd);
		dev.submit(cmd, fence, { &imageSemaphore, 1 }, { &renderSemaphore, 1 }, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		con.present();
		
		frame.finishFrame();
	}
	dev.waitTillIdle();
}