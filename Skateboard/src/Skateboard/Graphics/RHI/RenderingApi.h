#pragma once
#include "Skateboard/Mathematics.h"
#include "Skateboard/Scene/SceneBuilder.h"
#include "Skateboard/Graphics/RHI/GraphicsContext.h"

namespace Skateboard
{
	// Forward declarations
	class RasterizationPipeline;
	class ComputePipeline;
	class RaytracingPipeline;
	class Scene;
	class SkeletalMesh;

	enum SKTBD_DSVClearMode
	{
		DEPTH = 1,
		STENCIL = 2,
	};
	ENUM_FLAG_OPERATORS(SKTBD_DSVClearMode)

	struct DispatchRaysDesc
	{
		ShaderTable* RaygenRecord;
		ShaderTable* MissTable;
		ShaderTable* HitGroupTable;
		ShaderTable* CallableTable;

		uint32_t Width;
		uint32_t Height;
		uint32_t Depth;
	};

	class RenderingApi
	{
	public:
		RenderingApi() {}
		virtual ~RenderingApi(){}

		virtual void SetViewport(Viewport* Viewport, uint32_t count, CommandBuffer* cb) = 0;
		virtual void SetScissor(Rect* n_scissor, uint32_t count, CommandBuffer* cb) = 0;

		// RENDERER UTILITIES //////////////////////////////////////////////////////////////

		void WaitUntilIdle();

		void Flush();

		// DRAW COMMANDS ///////////////////////////////////////////////////////////////////

		virtual void BeginCommandBuffer(CommandBuffer* cb) = 0;
		virtual void EndCommandBuffer(CommandBuffer* cb) = 0;

		virtual void SubmitCommandBuffers(CommandBuffer**, uint32_t count) = 0;

		virtual void Barrier(BarrierGroup* barriers, uint32_t group_count, CommandBuffer* cb) = 0;

		virtual void SetInline32bitDataGraphics(uint32_t InputSlot, void* Data, uint32_t Size, CommandBuffer* cb) = 0;
		virtual void SetInline32bitDataCompute(uint32_t InputSlot, void* Data, uint32_t Size, CommandBuffer* cb) = 0;

		virtual void SetInlineResourceViewGraphics(uint32_t InputSlot, Buffer* Buffer,  BufferViewDesc desc, ViewAccessType_ Type,  CommandBuffer* cb) = 0;
		virtual void SetInlineResourceViewCompute(uint32_t InputSlot,  Buffer* Buffer,	BufferViewDesc desc, ViewAccessType_ Type,  CommandBuffer* cb) = 0;

		virtual void SetDescriptorTableGraphics(uint32_t InputSlot, const DescriptorTable& table, CommandBuffer* cb) = 0;
		virtual void SetDescriptorTableCompute(uint32_t InputSlot, const DescriptorTable& table, CommandBuffer* cb) = 0;


		virtual void SetInputLayoutCompute(ShaderInputLayout* inputLayout, CommandBuffer* cb) = 0;
		virtual void SetInputLayoutGraphics(ShaderInputLayout* inputLayout, CommandBuffer* cb) = 0;


		virtual void SetPipelineState(Pipeline* pipeline, CommandBuffer* cb) = 0;

		virtual void SetVertexBuffer(const VertexBufferView* vbviews, uint8_t numViews, uint8_t startSlot, CommandBuffer* cb) = 0;
		virtual void SetPrimitiveTopology(SKTBD_PRIMITIVE_TOPOLOGY topology, CommandBuffer* cb) = 0;

		virtual void SetIndexBuffer(const IndexBufferView* ibview, CommandBuffer* cb) = 0;

		virtual void SetRenderTargets(RenderTargetView* views, uint32_t numViews, DepthStencilView* DepthRenderTarget, CommandBuffer* cb) = 0;
		virtual void ClearRenderTargets(RenderTargetView* views, uint32_t numViews, const float4& colour, Rect* rects, uint32_t rect_count, CommandBuffer* cb) = 0;
		virtual void ClearDepthStencil(DepthStencilView* view, ClearValue clearV, SKTBD_DSVClearMode mode, Rect* rects, uint32_t numRects, CommandBuffer* cb) = 0;

		virtual void WaitForCommandBufer(CommandBuffer* cb) = 0;

		//		void ResizeBackBuffers(uint32_t width, uint32_t height) final override;
		virtual void DispatchRays(const DispatchRaysDesc& desc, CommandBuffer* cb) = 0;

		virtual void Dispatch(uint32_t X_groups, uint32_t Y_groups, uint32_t Z_groups, CommandBuffer* cb) = 0;

		virtual void Draw(uint32_t StartingVertex, uint32_t VertexCount, CommandBuffer* cb) = 0;
		virtual void DrawInstanced(uint32_t StartingVertex, uint32_t VertexCount, uint32_t InstanceCount, uint32_t StartingInstance,  CommandBuffer* cb) = 0;
		virtual void DrawIndexed(uint32_t StartingVertex, uint32_t StartingIndex, uint32_t IndexCount, CommandBuffer* cb) = 0;
		virtual void DrawIndexedInstanced(uint32_t StartingVertex, uint32_t StartingIndex, uint32_t StartingInstance, uint32_t IndexCount, uint32_t InstanceCount, CommandBuffer* cb) = 0;

		virtual BottomLevelAccelerationStructure BuildBottomLevelAS(const BottomLevelAccelerationStructureDesc& Desc, const SKTBD_ACCELERATION_STRUCT_BUILD_FLAGS& Flags, const BufferRef& StorageBuffer, const Buffer* ScratchBuffer, const  uint64_t& Scratch_Offset, const CommandBuffer* cb) =0;
		virtual TopLevelAccelerationStructure BuildTopLevelAS(const TopLevelAccelerationStructureDesc& Desc, const SKTBD_ACCELERATION_STRUCT_BUILD_FLAGS& Flags, const BufferRef& StorageBuffer, const Buffer* ScratchBuffer, const  uint64_t& Scratch_Offset, const CommandBuffer* cb) =0;
	};

}
