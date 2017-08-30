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

#include <vector>
#include <fstream>
#include <string>

struct FileResource
{
	std::vector<uint8_t> contents;
};

struct ImageResource
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
	std::vector<uint8_t> contents;
};

struct ImageResourceHDR
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
	std::vector<float> contents;
};

class ResourceManager
{
public:

	static ResourceManager& resource_manager()
	{
		static ResourceManager manager;
		return manager;
	}

	static FileResource load_file(const std::string& file_name);
	static ImageResource load_image(const std::string& file_name, bool force_channels = true);
	static ImageResourceHDR load_image_hdr(const std::string& file_name, bool force_channels = true);

	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;

private:

	ResourceManager() = default;
};