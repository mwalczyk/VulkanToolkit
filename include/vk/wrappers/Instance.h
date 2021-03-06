/*
*
* MIT License
*
* Copyright(c) 2017 Michael Walczyk
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Platform.h"

namespace plume
{

	namespace graphics
	{

		//! There is no global state in Vulkan and all per-application state is stored in an instance object.
		//! Creating an instance initializes the Vulkan library and allows the application to pass information
		//! about itself to the implementation.
		class Instance
		{
		public:

			class Options
			{
			public:

				Options();

				//! Specify the names of all instance layers that should be enabled. By default,
				//! only the VK_LAYER_LUNARG_standard_validation layer is enabled.
				Options& required_layers(const std::vector<const char*>& required_layers) { m_required_layers = required_layers; return *this; }

				//! Add a single name to the list of instance layers that should be enabled.
				Options& append_required_layer(const char* layer) { m_required_layers.push_back(layer); return *this; }

				//! Specify the names of all instance extensions that should be enabled.
				Options& required_extensions(const std::vector<const char*>& required_extensions) { m_required_extensions = required_extensions; return *this; }

				//! Add a single name to the list of instance extensions that should be enabled. By default,
				//! only the VK_EXT_debug_report instance extension is enabled.
				Options& append_required_extensions(const char* extension) { m_required_extensions.push_back(extension); return *this; }

				//! Specify a complete VkApplicationInfo structure that will be used to create this instance.
				Options& application_info(const vk::ApplicationInfo& application_info) { m_application_info = application_info; return *this; }

				//! Specify the logging level that will be observed by the validation layers. By default, 
				//! validation layers will only be enabled when building in debug mode, and they will observe
				//! the VK_DEBUG_REPORT_ERROR_BIT_EXT and VK_DEBUG_REPORT_WARNING_BIT_EXT flags.
				Options& set_logging_flags(VkDebugReportFlagsEXT debug_report_flags) { m_debug_report_flags = debug_report_flags; return *this; }

			private:

				std::vector<const char*> m_required_layers;
				std::vector<const char*> m_required_extensions;
				vk::ApplicationInfo m_application_info;
				VkDebugReportFlagsEXT m_debug_report_flags;

				friend class Instance;
			};

			Instance(const Options& options = Options());

			~Instance();

			vk::Instance get_handle() const { return m_instance_handle.get(); }

			//! Returns a vector of structs specifying the extension properties of the instance, such as the name and
			//! version of a particular extension.
			const std::vector<vk::ExtensionProperties>& get_instance_extension_properties() const { return m_instance_extension_properties; }

			//! Returns a vector of structs specifying the layer properties of the instance, such as the implementation 
			//! version of a particular layer.
			const std::vector<vk::LayerProperties>& get_instance_layer_properties() const { return m_instance_layer_properties; }

			//! Returns a vector of handles to all available physical devices.
			const std::vector<vk::PhysicalDevice>& get_physical_devices() const { return m_physical_devices; }

			//! Returns a handle to the first physical device that meets the criteria specified by `func`.
			vk::PhysicalDevice pick_physical_device(const std::function<bool(vk::PhysicalDevice)>& func);

		private:

			//! Ensures that each requested layer is supported by this instance.
			bool check_instance_layer_support();

			//! A helper function for creating the debug report callback object used by the instance.
			void setup_debug_report_callback(VkDebugReportFlagsEXT debug_report_flags);

			vk::UniqueInstance m_instance_handle;
			VkDebugReportCallbackEXT m_debug_report_callback;

			std::vector<vk::ExtensionProperties> m_instance_extension_properties;
			std::vector<vk::LayerProperties> m_instance_layer_properties;
			std::vector<vk::PhysicalDevice> m_physical_devices;
			std::vector<const char*> m_required_layers;
			std::vector<const char*> m_required_extensions;

			static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags,
																 VkDebugReportObjectTypeEXT object_type,
																 uint64_t object,
																 size_t location,
																 int32_t code,
																 const char* layer_prefix,
																 const char* message,
																 void* data)
			{
				std::cerr << "VALIDATION LAYER [";
				if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
				{
					std::cerr << "DEBUG]:" << message << "\n";
				}
				else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
				{
					std::cerr << "ERROR]:" << message << "\n";
				}
				else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
				{
					std::cerr << "INFORMATION]:" << message << "\n";
				}
				else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
				{
					std::cerr << "PERFORMANCE WARNING]:" << message << "\n";
				}
				else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
				{
					std::cerr << "WARNING]:" << message << "\n";
				}
				return VK_FALSE;
			}
		};

	} // namespace graphics

} // namespace plume