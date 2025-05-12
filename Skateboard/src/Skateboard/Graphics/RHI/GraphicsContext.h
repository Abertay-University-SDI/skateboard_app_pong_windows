#pragma once
#include "Skateboard/Mathematics.h"
#include "Skateboard/Graphics/Resources/CommonResources.h"
#include "Skateboard/Graphics/RHI/GraphicsSettingsDefines.h"
#include "sktbdpch.h"

namespace Skateboard
{
	using CopyResult = void*;
	class RenderingApi;
	class ResourceFactory;

	class GraphicsContext
	{
		friend class RenderCommand;
		friend class ResourceFactory;

	public:
		GraphicsContext(int32_t clientWidth, int32_t clientHeight) :
			m_ClientWidth(clientWidth),
			m_ClientHeight(clientHeight),
			m_CurrentFrameResourceIndex(0u)
		{
		}

		GraphicsContext() :
			m_ClientWidth(0),
			m_ClientHeight(0),
			m_CurrentFrameResourceIndex(0u)
		{
		}

		virtual ~GraphicsContext() {}

		/// <summary>
		/// Static Interface Of Skateboard Engine Graphics Context
		/// </summary> 

		static void SetRenderTargetToBackBuffer() { return Context->SetRenderTargetToBackBuffer_(); }
		static void Resize(int clientWidth, int clientHeight) { Context->Resize_(clientWidth, clientHeight);}
		static void OnResized() { Context->OnResized_(); }

		static float GetClientAspectRatio()	{ return static_cast<float>(Context->m_ClientWidth) / Context->m_ClientHeight; }
		static void Reset() { Context->Reset_(); }
		static void Flush() { Context->Flush_(); }
		static void WaitUntilIdle() { Context->WaitUntilIdle_(); }
		static void BeginFrame() { Context->BeginFrame_(); }
		static void EndFrame() { Context->EndFrame_(); }
		static void Present() { return Context->Present_(); }
		static bool IsRaytracingSupported() { return Context->IsRaytracingSupported_(); }
		static bool AreWorkGraphsSupported() { return Context->AreWorkGraphsSupported_(); }

		static CopyResult CopyDataToBuffer(Buffer* dest, off_t offset, size_t size, void* src) { return Context->CopyDataToBuffer_(dest, offset, size, src); };
		static CopyResult CopyDataToBuffer(Buffer* dest, off_t offset, size_t size, std::function<void(void*)> WriterFunct) { return Context->CopyDataToBuffer_(dest, offset, size, WriterFunct); }

		//Raytracing
		static CopyResult WriteTopLevelASInstanceDataToBuffer(Buffer* dest, off_t offset, size_t num, TLASInstanceData* data) { return Context->WriteTopLevelASInstanceDataToBuffer_(dest, offset, num, data); }
		static RaytracingASSizeInfo QueryAccelerationStructureSizeReq(const AccelerationStructureDesc& AS_Desc, SKTBD_ACCELERATION_STRUCT_BUILD_FLAGS Flags) { return Context->QueryAccelerationStructureSizeReq_(AS_Desc, Flags); };
		static ShaderTable BuildShaderTable(const BufferRef& Target, off_t offset, Pipeline* pso, const RaytracingPipelineDesc& m_desc, ShaderTableGroup Group) { return Context->BuildShaderTable_(Target, offset, pso, m_desc, Group); };

		//Access to Variables

		static const int32_t GetClientWidth() { return Context->m_ClientWidth; }
		static const int32_t GetClientHeight()  { return Context->m_ClientHeight; }
		static void NextFrame() { Context->m_CurrentFrameResourceIndex = (Context->m_CurrentFrameResourceIndex + 1) % GRAPHICS_SETTINGS_NUMFRAMERESOURCES; ++Context->m_DefaultGraphicsCB; ++Context->m_SwapChainRTVs; }
		static uint64_t GetCurrentFrameResourceIndex() { return Context->m_CurrentFrameResourceIndex; }
		static CommandBuffer* GetDefaultCommandBuffer() { return Context->m_DefaultGraphicsCB.Get().get(); }

		static RenderTargetView* GetBackBuffer() { return Context->m_SwapChainRTVs.Get().get(); }
		static DepthStencilView* GetDefaultDepthBuffer() { return Context->m_DefaultDSV.get(); }

		static void SetBackBufferClearColour(float4 nClearColour) { Context->m_ClearColour = nClearColour; }
		static void SetClearBackBuffer(bool ClearBackBuffer) { Context->m_bClearBackBuffer = ClearBackBuffer; }

		/// <summary>
		/// Virtual Interface of Skateboard Engine Graphics Context
		/// </summary>

	protected:

		virtual void SetRenderTargetToBackBuffer_() = 0;

		virtual void Resize_(int clientWidth, int clientHeight) {}
		virtual void OnResized_() {}
		
		virtual void Reset_() {}
		virtual void Flush_() {}
		virtual void WaitUntilIdle_() {}

		virtual void BeginFrame_() {}
		virtual void EndFrame_() {}
		virtual void Present_() {}

		virtual bool IsRaytracingSupported_() = 0;
		virtual bool AreWorkGraphsSupported_() = 0;

		//Data Shuffling
		virtual CopyResult CopyDataToBuffer_(Buffer* dest, off_t offset, size_t size, void* src) = 0;
		virtual CopyResult CopyDataToBuffer_(Buffer* dest, off_t offset, size_t size, std::function<void(void*)> WriterFunct) = 0;

		//Raytracing Data
		virtual CopyResult WriteTopLevelASInstanceDataToBuffer_(Buffer* dest, off_t offset, size_t num, TLASInstanceData* data) = 0;

		//virtual ShaderID GetShaderIDFromPSO(Pipeline* RT_pipeline, const wchar_t* ShaderName);
		//virtual void WriteShaderRecordToShaderTable (ShaderTable* dest, ShaderRecord record);

		virtual ShaderTable BuildShaderTable_(const BufferRef& Target, off_t offset, Pipeline* pso, const RaytracingPipelineDesc& m_desc, ShaderTableGroup Group) = 0;
		virtual RaytracingASSizeInfo QueryAccelerationStructureSizeReq_(const AccelerationStructureDesc& AS_Desc, SKTBD_ACCELERATION_STRUCT_BUILD_FLAGS Flags) = 0;
		

	protected:
		virtual RenderingApi* GetAPI() = 0;
		virtual ResourceFactory* GetResourceFactory() = 0;

	public:
		static GraphicsContext* Context;
	protected:

		int32_t m_ClientWidth, m_ClientHeight;		// Width and height of the client area (does not include the top bar and menus, this is the drawable surface)
		uint64_t m_CurrentFrameResourceIndex;

		float4 m_ClearColour = GRAPHICS_BACKBUFFER_DEFAULT_CLEAR_COLOUR;
		bool m_bClearBackBuffer = true;

		MultiResource<CommandBufferRef> m_DefaultGraphicsCB;

		//Default Render Targets
		MultiResource<RenderTargetViewRef, GRAPHICS_SETTINGS_NUMFRAMERESOURCES>	m_SwapChainRTVs;			// only views can be accessed

		//Default DSV
		TextureBufferRef										m_DepthStencilBuffer;						// The depth/stencil buffer
		DepthStencilViewRef										m_DefaultDSV;
	};
}
