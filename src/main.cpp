#include<iostream>
#include<log.hpp>
#include<window.hpp>
#include<device.hpp>
#include<renderContext.hpp>
#include<graphicsPipeline.hpp>
#include<commandPool.hpp>
#include<frameContext.hpp>
//temporary
#include<defaultShaderVertex.hpp>
#include<defaultShaderFragment.hpp>

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
	// auto cmds = frame.getGraphicsBuffers(gFramesInFlight);

	auto& graphics = dev.getQueue(CommandUse::draw)->get();
	auto& present = dev.getQueue(CommandUse::present)->get();
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
		auto frameData = con.popNextImage(imageSemaphore);
		auto& image = std::get<VkImage>(frameData);
		auto& view = std::get<VkImageView>(frameData);
		auto& renderSemaphore = std::get<VkSemaphore>(frameData);
		auto imageIndex = std::get<uint32_t>(frameData);

		auto& cmd = beginRecord(image);
		draw(view, cmd);
		endRecord(image, cmd);
		VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		graphics.submit(fence, {&imageSemaphore, 1}, {&renderSemaphore, 1}, waitStages, {&cmd.get(), 1});
		present.present({&renderSemaphore, 1}, imageIndex, con.getSwapchain());

		frame.finishFrame();
	}
}