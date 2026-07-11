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

	auto& graphics = dev.getQueue(CommandUse::draw)->get();
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
		cmd.draw(3);
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
		VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		graphics.submit(fence, {&imageSemaphore, 1}, {&renderSemaphore, 1}, waitStages, {&cmd.get(), 1});
		con.present();
		
		frame.finishFrame();
	}
	dev.waitTillIdle();
}