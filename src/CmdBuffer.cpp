#include <vulkan/vulkan.hpp>
#include "CmdBuffer.h"

namespace vg
{
    namespace cmd
    {
        void BindPipeline::operator()(CmdBuffer& commandBuffer) const
        {
            CmdBufferHandle(commandBuffer).bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
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
            CmdBufferHandle(commandBuffer).bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, firstSet, descriptorSets.size(), &descriptorSets[0], 0, nullptr);
        }

        void BeginRenderpass::operator()(CmdBuffer& commandBuffer) const
        {
            std::vector<vk::ClearValue> clearValues = {
                vk::ClearValue({clearColorR, clearColorG, clearColorB, clearColorA}),
                vk::ClearValue{vk::ClearDepthStencilValue(depthClearColor,stencilClearColor)} };

            auto info = vk::RenderPassBeginInfo(renderpass, framebuffer, vk::Rect2D({ offsetX,offsetY }, { extendX,extendY }), clearValues);
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
            CmdBufferHandle(commandBuffer).pushConstants(layout, (vk::ShaderStageFlags) stages, offset, size, values);
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