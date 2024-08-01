#include <vulkan/vulkan.hpp>
#include "CmdBuffer.h"

namespace vg
{
    namespace cmd
    {
        void BindPipeline::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).bindPipeline((vk::PipelineBindPoint) bindPoint, pipeline);
        }

        void BindVertexBuffer::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).bindVertexBuffers(0, 1, &buffers, &offset);
        }

        void BindIndexBuffer::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).bindIndexBuffer(buffer, offset, (vk::IndexType) type);
        }

        void BindDescriptorSets::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).bindDescriptorSets((vk::PipelineBindPoint) bindPoint, layout, firstSet, descriptorSets.size(), &descriptorSets[0], 0, nullptr);
        }

        void BeginRenderpass::operator()(CmdBuffer& commandBuffer) const
        {
            std::vector<vk::ClearValue> clearValues = {
                vk::ClearValue((vk::ClearColorValue) clearColor),
                vk::ClearValue{vk::ClearDepthStencilValue(depthClearColor,stencilClearColor)}
            };

            auto info = vk::RenderPassBeginInfo(renderpass, framebuffer, vk::Rect2D(offset, *(vk::Extent2D*) &extend), clearValues);
            CmdBufferHandle(commandBuffer).beginRenderPass(info, vk::SubpassContents::eInline);
        }

        void SetViewport::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).setViewport(first, viewports.size(), (vk::Viewport*) &viewports[0]);
        }

        void SetScissor::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).setScissor(first, scissors.size(), (vk::Rect2D*) &scissors[0]);
        }

        void Draw::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }

        void EndRenderpass::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).endRenderPass();
        }

        void DrawIndexed::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void CopyBuffer::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).copyBuffer(src, dst, *(std::vector<vk::BufferCopy>*) & regions);
        }

        void CopyBufferToImage::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).copyBufferToImage(src, dst, (vk::ImageLayout) dstImageLayout, *(std::vector<vk::BufferImageCopy>*) & regions);
        }

        void PipelineBarier::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).pipelineBarrier((vk::PipelineStageFlags) srcStageMask, (vk::PipelineStageFlags) dstStageMask, (vk::DependencyFlags) dependency, *(std::vector<vk::MemoryBarrier>*) & memoryBarriers, *(std::vector<vk::BufferMemoryBarrier>*) & bufferMemoryBarriers, *(std::vector<vk::ImageMemoryBarrier>*) & imageMemoryBarriers);
        }

        void ExecuteCommands::operator ()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).executeCommands(cmdBuffers);
        }

        void PushConstants::operator ()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).pushConstants(layout, (vk::ShaderStageFlags) stages, offset, data.size(), data.data());
        }

        void SetLineWidth::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setLineWidth(lineWidth);
        }

        void SetDepthBias::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setDepthBias(bias.constantFactor, bias.clamp, bias.slopeFactor);
        }

        void SetBlendConstants::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setBlendConstants(blendConstants);
        }

        void SetDepthBounds::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setDepthBounds(minDepthBounds, maxDepthBounds);
        }

        void SetStencilCompareMask::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setStencilCompareMask((vk::StencilFaceFlags) faceMask, compareMask);
        }

        void SetStencilWriteMask::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setStencilWriteMask((vk::StencilFaceFlags) faceMask, writeMask);
        }

        void SetStencilReference::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).setStencilReference((vk::StencilFaceFlags) faceMask, reference);
        }

        void DrawIndirect::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).drawIndirect(buffer, offset, drawCount, stride);
        }

        void DrawIndexedIndirect::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).drawIndexedIndirect(buffer, offset, drawCount, stride);
        }

        void Dispatch::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).dispatch(groupCountX, groupCountY, groupCountZ);
        }

        void DispatchIndirect::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).dispatchIndirect(buffer, offset);
        }

        void CopyImage::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).copyImage(srcImage, (vk::ImageLayout) srcImageLayout, dstImage, (vk::ImageLayout) dstImageLayout, *(std::vector<vk::ImageCopy>*) & regions);
        }

        void BlitImage::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).blitImage(srcImage, (vk::ImageLayout) srcImageLayout, dstImage, (vk::ImageLayout) dstImageLayout, *(std::vector<vk::ImageBlit>*) & regions, (vk::Filter) filter);
        }

        void CopyImageToBuffer::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).copyImageToBuffer(srcImage, (vk::ImageLayout) srcImageLayout, dstBuffer, *(std::vector<vk::BufferImageCopy>*) & regions);
        }

        void UpdateBuffer::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).updateBuffer<char>(dstBuffer, dstOffset, data);
        }

        void FillBuffer::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).fillBuffer(dstBuffer, dstOffset, size, data);
        }

        void ClearColorImage::operator ()(CmdBuffer& commandBuffer)const
        {
            CmdBufferHandle(commandBuffer).clearColorImage(image, (vk::ImageLayout) imageLayout, color, *(std::vector<vk::ImageSubresourceRange>*) & ranges);
        }

    }

    CmdBuffer::CmdBuffer(const Queue& queue, bool isShortLived, CmdBufferLevel cmdLevel) : m_commandPool(queue.GetCmdPool(isShortLived)), m_queue(queue)
    {
        m_handle = ((DeviceHandle) currentDevice).allocateCommandBuffers({ m_commandPool, (vk::CommandBufferLevel) cmdLevel, 1 })[0];
    }

    CmdBuffer::CmdBuffer(const CmdPool& pool, CmdBufferLevel cmdLevel) : m_commandPool(pool), m_queue(pool.GetQueue())
    {
        m_handle = ((DeviceHandle) currentDevice).allocateCommandBuffers({ m_commandPool, (vk::CommandBufferLevel) cmdLevel, 1 })[0];
    }

    std::vector<CmdBuffer> CmdBuffer::CreateArray(const Queue& queue, bool areShortLived, CmdBufferLevel cmdLevel, uint32_t count)
    {
        auto handles = ((DeviceHandle) currentDevice).allocateCommandBuffers({ queue.GetCmdPool(areShortLived), (vk::CommandBufferLevel) cmdLevel, count });
        std::vector<CmdBuffer> buffers(count);
        for (int i = 0; i < count; i++)
        {
            buffers[i].m_handle = handles[i];
            buffers[i].m_commandPool = queue.GetCmdPool(areShortLived);
            buffers[i].m_queue = queue;
        }

        return buffers;
    }

    std::vector<CmdBuffer> CmdBuffer::CreateArray(const CmdPool& pool, CmdBufferLevel cmdLevel, uint32_t count)
    {
        auto handles = ((DeviceHandle) currentDevice).allocateCommandBuffers({ pool, (vk::CommandBufferLevel) cmdLevel, count });
        std::vector<CmdBuffer> buffers(count);
        for (int i = 0; i < count; i++)
        {
            buffers[i].m_handle = handles[i];
            buffers[i].m_commandPool = pool;
            buffers[i].m_queue = pool.GetQueue();
        }

        return buffers;
    }

    CmdBuffer::CmdBuffer() :m_handle(nullptr), m_commandPool(nullptr) {}

    CmdBuffer::CmdBuffer(CmdBuffer&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_queue, other.m_queue);
    }

    CmdBuffer::~CmdBuffer()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) currentDevice).freeCommandBuffers(m_commandPool, 1, (vk::CommandBuffer*) &m_handle);
    }

    CmdBuffer& CmdBuffer::operator=(CmdBuffer&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_queue, other.m_queue);

        return *this;
    }

    CmdBuffer::operator const CmdBufferHandle& () const
    {
        return m_handle;
    }

    CmdBuffer& CmdBuffer::Clear()
    {
        m_handle.reset();

        return *this;
    }

    CmdBuffer& CmdBuffer::Begin(Flags<CmdBufferUsage> usage)
    {
        m_handle.begin(vk::CommandBufferBeginInfo((vk::CommandBufferUsageFlags) usage));

        return *this;
    }

    CmdBuffer& CmdBuffer::Begin(Flags<CmdBufferUsage> usage, RenderPassHandle renderPass, uint32_t subpassIndex, FramebufferHandle framebuffer, bool occlusionQueryEnable, Flags<QueryControl> queryFlags, Flags<PipelineStatistic> pipelineStatistics)
    {
        vk::CommandBufferInheritanceInfo inheritance(renderPass, subpassIndex, framebuffer, occlusionQueryEnable, (vk::QueryControlFlags) queryFlags, (vk::QueryPipelineStatisticFlags) pipelineStatistics);
        m_handle.begin(
            vk::CommandBufferBeginInfo(
                (vk::CommandBufferUsageFlags) usage,
                &inheritance
            ));

        return *this;
    }

    CmdBuffer& CmdBuffer::End()
    {
        m_handle.end();

        return *this;
    }

    CmdBuffer& CmdBuffer::Submit(const std::vector<std::tuple<Flags<PipelineStage>, SemaphoreHandle>>& waitStages, const std::vector<SemaphoreHandle>& signalSemaphores, const Fence& fence)
    {
        std::vector<vk::Semaphore> semaphores(waitStages.size());
        std::vector<vk::PipelineStageFlags> stages(waitStages.size());
        for (int i = 0; i < waitStages.size(); i++)
        {
            semaphores[i] = std::get<1>(waitStages[i]);
            stages[i] = (vk::PipelineStageFlags) std::get<0>(waitStages[i]);
        }
        std::vector<vk::CommandBuffer> buffers;
        buffers.push_back(m_handle);
        m_queue.submit({ vk::SubmitInfo(semaphores,stages,buffers, *(std::vector<vk::Semaphore>*) & signalSemaphores) }, (FenceHandle) fence);

        return *this;
    }
}