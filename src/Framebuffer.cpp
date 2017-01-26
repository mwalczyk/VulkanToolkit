#include "Framebuffer.h"

namespace graphics
{

	Framebuffer::Options::Options()
	{
	}

	Framebuffer::Framebuffer(const DeviceRef &tDevice, const RenderPassRef &tRenderPass, const std::vector<vk::ImageView> &tImageViews, uint32_t tWidth, uint32_t tHeight, const Options &tOptions) :
		mDevice(tDevice),
		mRenderPass(tRenderPass),
		mImageViews(tImageViews),
		mWidth(tWidth),
		mHeight(tHeight)
	{
		vk::FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(mImageViews.size());
		framebufferCreateInfo.height = mHeight;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.pAttachments = mImageViews.data();
		framebufferCreateInfo.renderPass = mRenderPass->getHandle();		
		framebufferCreateInfo.width = mWidth;

		mFramebufferHandle = mDevice->getHandle().createFramebuffer(framebufferCreateInfo);
	}

	Framebuffer::~Framebuffer()
	{
		mDevice->getHandle().destroyFramebuffer(mFramebufferHandle);
	}

} // namespace graphics