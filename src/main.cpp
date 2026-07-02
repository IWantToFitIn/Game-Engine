#include<iostream>
#include<log.hpp>
#include<window.hpp>
#include<device.hpp>
#include<context.hpp>
#include<graphicsPipeline.hpp>
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
	Context con(dev, std::move(surf), 1080, 720);
	std::vector<Shader> shaders{};
	shaders.emplace_back(dev, gDefaultshadervertex);
	shaders.emplace_back(dev, gDefaultshaderfragment);
	GraphicsPipeline(dev, shaders, con.getFormat());
	while(win.process()){

	}
}