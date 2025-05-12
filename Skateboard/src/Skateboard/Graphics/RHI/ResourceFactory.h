#pragma once
#include "Skateboard/Graphics/Resources/CommonResources.h"

namespace Skateboard
{
	class ResourceFactory
	{
	public:
		virtual ~ResourceFactory() = default;
		static BufferRef CreateBuffer(const BufferDesc& desc) { return m_Factory->CreateBuffer_(desc); };
		static TextureBufferRef CreateTextureBuffer(const TextureDesc& desc) { return m_Factory->CreateTextureBuffer_(desc); };

		static  BufferSRVRef CreateBufferShaderResourceView(const BufferViewDesc& desc, BufferRef parent = BufferRef()) { return m_Factory->CreateBufferShaderResourceView_(desc, parent); };
		static  BufferUAVRef CreateBufferUnorderedAccessView(const BufferViewDesc& desc, BufferRef parent = BufferRef(), BufferRef counterResource = BufferRef()) { return m_Factory->CreateBufferUnorderedAccessView_(desc, parent, counterResource); };;
		static  BufferCBVRef CreateBufferConstantView(const BufferViewDesc& desc, BufferRef parent = BufferRef()) { return m_Factory->CreateBufferConstantView_(desc, parent); };

		static  TextureSRVRef CreateTextureShaderResourceView(const TextureViewDesc& desc, TextureBufferRef parent = TextureBufferRef()) { return m_Factory->CreateTextureShaderResourceView_(desc, parent); };
		static  TextureUAVRef CreateTextureUnorderedAccessView(const TextureViewDesc& desc, TextureBufferRef parent = TextureBufferRef()) { return m_Factory->CreateTextureUnorderedAccessView_(desc, parent); };

		static  DepthStencilViewRef CreateDepthStencilView(const DepthStencilDesc* desc, TextureBufferRef parent = TextureBufferRef()) { return m_Factory->CreateDepthStencilView_(desc, parent); };
		static  RenderTargetViewRef CreateRenderTargetView(const RenderTargetDesc* desc, TextureBufferRef parent = TextureBufferRef()) { return m_Factory->CreateRenderTargetView_(desc, parent); };

		static  CommandBufferRef CreateCommandBuffer(const CommandBufferType_& type, const CommandBufferPriority_& priority) { return m_Factory->CreateCommandBuffer_(type, priority); };

		static  ShaderInputLayoutRef CreateShaderInputLayout(const ShaderInputLayoutDesc& desc) { return m_Factory->CreateShaderInputLayout_(desc); };
		static  PipelineRef CreatePipelineState(const PipelineDesc& desc) { return m_Factory->CreatePipeline_(desc); };

		static ASViewRef CreateAccelerationStructureView(const AccelerationStructure& StructureHandle) { return m_Factory->CreateAccelerationStructureView_(StructureHandle); }

	protected:
		//resource creation
		virtual BufferRef CreateBuffer_(const BufferDesc& desc) = 0;
		virtual TextureBufferRef CreateTextureBuffer_(const TextureDesc& desc) = 0;

		virtual BufferSRVRef CreateBufferShaderResourceView_(const BufferViewDesc& desc, BufferRef parent = BufferRef()) = 0;
		virtual BufferUAVRef CreateBufferUnorderedAccessView_(const BufferViewDesc& desc, BufferRef parent = BufferRef(), BufferRef counterResource = BufferRef()) = 0;
		virtual BufferCBVRef CreateBufferConstantView_(const BufferViewDesc& desc, BufferRef parent = BufferRef()) = 0;

		virtual TextureSRVRef CreateTextureShaderResourceView_(const TextureViewDesc& desc, TextureBufferRef parent = TextureBufferRef()) = 0;
		virtual TextureUAVRef CreateTextureUnorderedAccessView_(const TextureViewDesc& desc, TextureBufferRef parent = TextureBufferRef()) = 0;

		virtual DepthStencilViewRef CreateDepthStencilView_(const DepthStencilDesc* desc, TextureBufferRef parent = TextureBufferRef()) = 0;
		virtual RenderTargetViewRef CreateRenderTargetView_(const RenderTargetDesc* desc, TextureBufferRef parent = TextureBufferRef()) = 0;

		virtual CommandBufferRef CreateCommandBuffer_(const CommandBufferType_& type, const CommandBufferPriority_& priority) = 0;

		virtual ShaderInputLayoutRef CreateShaderInputLayout_(const ShaderInputLayoutDesc&) = 0;
		virtual PipelineRef CreatePipeline_(const PipelineDesc& desc) = 0;

		virtual ASViewRef CreateAccelerationStructureView_(const AccelerationStructure& StructureHandle) = 0;

	protected:
		static ResourceFactory* m_Factory;
	};
};