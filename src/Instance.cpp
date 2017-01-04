#include "Instance.h"

namespace vk
{

	Instance::Options::Options()
	{
		mRequiredLayers = { "VK_LAYER_LUNARG_standard_validation" };
		mRequiredExtensions = { "VK_EXT_debug_report" };
		
		mApplicationInfo.apiVersion = VK_API_VERSION_1_0;
		mApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		mApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		mApplicationInfo.pApplicationName = "Application Name";
		mApplicationInfo.pEngineName = "Engine Name";
		mApplicationInfo.pNext = nullptr;
		mApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	}

	//! Proxy function for creating a debug callback object
	VkResult createDebugReportCallbackEXT(VkInstance tInstance, const VkDebugReportCallbackCreateInfoEXT* tCreateInfo, const VkAllocationCallbacks* tAllocator, VkDebugReportCallbackEXT* tCallback)
	{
		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(tInstance, "vkCreateDebugReportCallbackEXT");

		if (func != nullptr)
		{
			return func(tInstance, tCreateInfo, tAllocator, tCallback);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	//! Proxy function for destroying a debug callback object
	void destroyDebugReportCallbackEXT(VkInstance tInstance, VkDebugReportCallbackEXT tCallback, const VkAllocationCallbacks* tAllocator)
	{
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(tInstance, "vkDestroyDebugReportCallbackEXT");

		if (func != nullptr)
		{
			func(tInstance, tCallback, tAllocator);
		}
	}

	Instance::Instance(const Options &tOptions) :
		mInstanceHandle(VK_NULL_HANDLE),
		mRequiredLayers(tOptions.mRequiredLayers),
		mRequiredExtensions(tOptions.mRequiredExtensions),
		mApplicationInfo(tOptions.mApplicationInfo)
	{
		if (!checkInstanceLayerSupport())
		{
			throw std::runtime_error("One or more of the requested validation layers are not supported on this platform");
		}

		// Append the instance extensions required by the windowing system
#if defined(SPECTRA_MSW)
		mRequiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(SPECTRA_LINUX)
		mRequiredExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
		mRequiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		
		VkInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.enabledExtensionCount = mRequiredExtensions.size();
		instanceCreateInfo.enabledLayerCount = mRequiredLayers.size();
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &mApplicationInfo;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.ppEnabledExtensionNames = mRequiredExtensions.data();
		instanceCreateInfo.ppEnabledLayerNames = mRequiredLayers.data();
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstanceHandle);
		assert(result == VK_SUCCESS);

		setupDebugReportCallback();

		std::cout << "Successfully created instance\n";
	}

	Instance::~Instance()
	{
		destroyDebugReportCallbackEXT(mInstanceHandle, mDebugReportCallback, nullptr);

		vkDestroyInstance(mInstanceHandle, nullptr);
	}

	std::vector<VkExtensionProperties> Instance::getInstanceExtensionProperties() const
	{
		uint32_t instanceExtensionPropertiesCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionPropertiesCount, nullptr);

		std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionPropertiesCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionPropertiesCount, instanceExtensionProperties.data());

		return instanceExtensionProperties;
	}

	std::vector<VkLayerProperties> Instance::getInstanceLayerProperties() const
	{
		uint32_t instanceLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

		std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());

		return instanceLayerProperties;
	}

	std::vector<VkPhysicalDevice> Instance::getPhysicalDevices() const
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(mInstanceHandle, &physicalDeviceCount, nullptr);

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(mInstanceHandle, &physicalDeviceCount, physicalDevices.data());

		return physicalDevices;
	}

	bool Instance::checkInstanceLayerSupport()
	{
		auto instanceLayerProperties = getInstanceLayerProperties();

		for (const auto& requiredLayerName: mRequiredLayers)
		{
			auto predicate = [&](const VkLayerProperties &layerProperty) { return strcmp(requiredLayerName, layerProperty.layerName) == 0; };
			if (std::find_if(instanceLayerProperties.begin(), instanceLayerProperties.end(), predicate) == instanceLayerProperties.end())
			{
				std::cout << "Required layer " << requiredLayerName << " is not supported\n";
				return false;
			}
		}

		return true;
	}

	void Instance::setupDebugReportCallback()
	{
		VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = {};
		debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		debugReportCallbackCreateInfo.pfnCallback = debugCallback;
		debugReportCallbackCreateInfo.pNext = nullptr;
		debugReportCallbackCreateInfo.pUserData = nullptr;
		debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		
		auto result = createDebugReportCallbackEXT(mInstanceHandle, &debugReportCallbackCreateInfo, nullptr, &mDebugReportCallback);
		assert(result == VK_SUCCESS);
	}

} // namespace vk