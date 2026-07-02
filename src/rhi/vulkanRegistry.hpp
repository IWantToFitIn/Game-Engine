#pragma once
#include<vector>
#include<set>

class VulkanRegistry{
	static std::vector<const char*>& getInstanceRegistry(){
		static std::vector<const char*> instanceRegistry;
		return instanceRegistry;
	}
	static std::vector<const char*>& getDeviceRegistry(){
		static std::vector<const char*> deviceRegistry;
		return deviceRegistry;
	}
	static std::set<size_t>& getFeatureRegistry(){
		static std::set<size_t> featureRegistry;
		return featureRegistry;
	}

public:
	static const std::vector<const char*>& getDeviceExtensions(){
		return getDeviceRegistry();
	}
	static const std::vector<const char*>& getInstanceExtensions(){
		return getInstanceRegistry();
	}
	static const std::set<size_t> getFeatures(){
		return getFeatureRegistry();
	}

	template<bool device>
	struct Register{
		Register(std::initializer_list<const char*> list){
			if constexpr(device){
				auto& reg = getDeviceRegistry();
				for(auto& name : list)
					reg.push_back(name);
			} else{
				auto& reg = getInstanceRegistry();
				for(auto& name : list)
					reg.push_back(name);
			}
		}
		Register(std::initializer_list<size_t> list){
			auto& reg = getFeatureRegistry();
			for(auto& feat : list)
				reg.emplace(feat);
		}
		Register(size_t feat){
			auto& reg = getFeatureRegistry();
			reg.emplace(feat);
		}
	};
};

#define REG_CONCAT_INNER(a, b) a ## b
#define REG_CONCAT(a, b) REG_CONCAT_INNER(a, b)

#define REGISTER_DEVICE_EXTENSION(...) \
	namespace { \
		static const VulkanRegistry::Register<1> REG_CONCAT(gExtReg, __LINE__){__VA_ARGS__} ; \
	}
	
#define REGISTER_INSTANCE_EXTENSION(...) \
	namespace { \
		static const VulkanRegistry::Register<0> REG_CONCAT(gExtReg, __LINE__){__VA_ARGS__} ; \
	}

#define REGISTER_DEVICE_FEATURE(...) \
	namespace { \
		static const VulkanRegistry::Register<1> REG_CONCAT(gExtReg, __LINE__)(__VA_ARGS__); \
	} 
