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

struct Vertex {
	float pos[2];
	float color[3];
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
		.addBinding(0, 2);
	auto pipeline = builder.getResult(dev);
	FrameContext frame(dev);
	ExecutionStream transferStream(dev, {VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT }, 3);
	ExecutionStream graphicsStream(dev, {VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT}, 3);

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
	transfer(dev, transferStream, frame, std::as_bytes(std::span{vertices}), vbo);
	const std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};
	Buffer ibo(dev, indices.size() * sizeof(uint32_t), BufferUsage::Index, BufferAccess::Immutable);
	transfer(dev, transferStream, frame, std::as_bytes(std::span{indices}), ibo);

	std::vector<uint32_t> redSquareData(32 * 32, 0xaaff0000);
	auto redSquareImage = Image(dev, 32, 32, 1, VK_FORMAT_R8G8B8A8_SRGB);
	uploadImage(dev, transferStream, frame, std::as_bytes(std::span{redSquareData}), redSquareImage);
	auto redSquareSampler = Sampler(dev);
	auto texHandle = dev.getBindless().storeTexture(std::move(redSquareImage), std::move(redSquareSampler));

	UBO uboData = {0.5f, 0.0f};
	Buffer ubo(dev, sizeof(UBO), BufferUsage::Storage, BufferAccess::Immutable);
	transfer(dev, transferStream, frame, std::as_bytes(std::span{&uboData, 1}), ubo);
	auto uboHandle = dev.getBindless().storeBuffer(std::move(ubo));

	auto beginRecord = [&](Image& image, CommandList& cmd){
		cmd.begin();
		cmd.transition(ImageLayout::attachment, image);
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