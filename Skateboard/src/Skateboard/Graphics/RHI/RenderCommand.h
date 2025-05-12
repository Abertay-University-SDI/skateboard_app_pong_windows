#pragma once
#include "Skateboard/Graphics/Resources/CommonResources.h"
#include "Skateboard/Graphics/RHI/RenderingApi.h"

namespace Skateboard
{
	// Forward declarations
	class GraphicsContext;
	class Mesh;

	class RenderCommand
	{
		friend class Application;
	public:
		static void SetViewport(Viewport* n_viewports,uint32_t count, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetViewport(n_viewports,count, cb);
		}

		static void SetScissor(Rect* n_rects, uint32_t count, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetScissor(n_rects,count,cb);
		}

		static void WaitUntilIdle()
		{
			Api->WaitUntilIdle();
		}

		static void Flush()
		{
			Api->Flush();
		}

		static void BeginCommandBuffer(CommandBuffer* cb)																																				
		{
			Api->BeginCommandBuffer(cb);
		}

		static void EndCommandBuffer(CommandBuffer* cb)																																					
		{
			Api->EndCommandBuffer(cb);
		}

		static void Barrier(BarrierGroup* barriers, uint32_t barrier_count, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->Barrier(barriers, barrier_count, cb);
		}

		static void SubmitCommandBuffers(CommandBuffer** cbs, uint32_t count)																																
		{
			Api->SubmitCommandBuffers(cbs, count);
		}

		static void SetInputLayoutGraphics(ShaderInputLayout* layout, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetInputLayoutGraphics(layout, cb);
		}

		static void SetInputLayoutCompute(ShaderInputLayout* layout, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetInputLayoutCompute(layout, cb);
		}

		static void SetInline32bitDataGraphics(uint32_t InputSlot, void* Data, uint32_t Size, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())											
		{
			Api->SetInline32bitDataGraphics(InputSlot, Data, Size, cb);
		}

		static void SetInline32bitDataCompute(uint32_t InputSlot, void* Data, uint32_t Size, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetInline32bitDataCompute(InputSlot, Data, Size, cb);
		}

		static void SetInlineResourceViewGraphics(uint32_t InputSlot, Buffer* Buffer, BufferViewDesc desc, ViewAccessType_ type , CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())												
		{
			Api->SetInlineResourceViewGraphics(InputSlot, Buffer, desc, type, cb);
		}

		static void SetInlineResourceViewCompute(uint32_t InputSlot, Buffer* Buffer, BufferViewDesc desc, ViewAccessType_ type, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetInlineResourceViewCompute(InputSlot,Buffer,desc, type, cb);
		}

		static void SetDescriptorTableCompute(uint32_t InputSlot, const DescriptorTable& table, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())										
		{
			Api->SetDescriptorTableCompute(InputSlot, table, cb);
		}

		static void SetDescriptorTableGraphics(uint32_t InputSlot, const DescriptorTable& table, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetDescriptorTableGraphics(InputSlot, table, cb);
		}

		static void SetPipelineState(Pipeline* pipeline, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())						
		{
			Api->SetPipelineState(pipeline, cb);
		}

		static void SetVertexBuffer(const VertexBufferView* vbviews, uint8_t numViews, uint8_t startSlot = 0, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())												
		{
			Api->SetVertexBuffer(vbviews, numViews,startSlot, cb);
		}

		static void SetPrimitiveTopology(SKTBD_PRIMITIVE_TOPOLOGY Topology, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->SetPrimitiveTopology(Topology, cb);
		}

		static void SetIndexBuffer(const IndexBufferView* ibview, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())	
		{
			Api->SetIndexBuffer(ibview, cb);
		}

		static void SetRenderTargets(RenderTargetView* views, uint32_t numViews, DepthStencilView* DepthRenderTarget, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())	
		{
			Api->SetRenderTargets(views, numViews, DepthRenderTarget, cb);
		}

		static void ClearRenderTargets(RenderTargetView* views, uint32_t numViews, const float4& colour, Rect* rects = nullptr, uint32_t numRects = 0, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->ClearRenderTargets(views, numViews, colour, rects, numRects, cb);
		}

		static void WaitForCommandBufer(CommandBuffer* cb)
		{
			Api->WaitForCommandBufer(cb);
		}

		static void Dispatch(uint32_t X_groups, uint32_t Y_groups, uint32_t Z_groups, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())											
		{
			Api->Dispatch(X_groups, Y_groups, Z_groups, cb);
		}

		static void DispatchRays(const DispatchRaysDesc& desc, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->DispatchRays(desc, cb);
		}

		static void Draw(uint32_t StartingVertex, uint32_t VertexCount, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->Draw(StartingVertex, VertexCount, cb);
		}

		static void DrawInstanced(uint32_t StartingVertex, uint32_t VertexCount, uint32_t InstanceCount, uint32_t StartingInstance, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->DrawInstanced(StartingVertex, VertexCount, InstanceCount, StartingInstance, cb);
		}

		static void DrawIndexed(uint32_t StratingVertex, uint32_t StartingIndex, uint32_t IndexCount, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->DrawIndexed(StratingVertex, StartingIndex, IndexCount, cb);
		}

		static void DrawIndexedInstanced(uint32_t StratingVertex, uint32_t StartingIndex, uint32_t StartingInstance, uint32_t IndexCount, uint32_t InstanceCount, CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			Api->DrawIndexedInstanced( StratingVertex,  StartingIndex,  StartingInstance,  IndexCount,  InstanceCount, cb);
		}

		static BottomLevelAccelerationStructure BuildBottomLevelAS(const BottomLevelAccelerationStructureDesc& Desc, const SKTBD_ACCELERATION_STRUCT_BUILD_FLAGS& Flags, const BufferRef& StorageBuffer, const Buffer* ScratchBuffer, const  uint64_t& Scratch_Offset = 0, const CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			return Api->BuildBottomLevelAS(Desc, Flags, StorageBuffer, ScratchBuffer, Scratch_Offset, cb);
		}

		static TopLevelAccelerationStructure BuildTopLevelAS(const TopLevelAccelerationStructureDesc& Desc, const SKTBD_ACCELERATION_STRUCT_BUILD_FLAGS& Flags, const BufferRef& StorageBuffer, const Buffer* ScratchBuffer, const  uint64_t& Scratch_Offset = 0, const CommandBuffer* cb = GraphicsContext::GetDefaultCommandBuffer())
		{
			return Api->BuildTopLevelAS(Desc, Flags, StorageBuffer, ScratchBuffer, Scratch_Offset, cb);
		}

	private:
		static void Init();
		static RenderingApi* Api;
	};

	
}
