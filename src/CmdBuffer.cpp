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
    }

    CmdBuffer::CmdBuffer(const Queue& queue, bool isShortLived) : m_queue(&queue), m_isShortLived(isShortLived)
    {
        m_handle = ((DeviceHandle) currentDevice).allocateCommandBuffers({ queue.GetCommandPool(isShortLived), vk::CommandBufferLevel::ePrimary, 1 })[0];
    }

    CmdBuffer::CmdBuffer() :m_handle(nullptr), m_queue(nullptr) {}

    CmdBuffer::CmdBuffer(CmdBuffer&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_queue, other.m_queue);
        std::swap(m_isShortLived, other.m_isShortLived);
    }

    CmdBuffer::~CmdBuffer()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) currentDevice).freeCommandBuffers(m_queue->GetCommandPool(m_isShortLived), 1, (vk::CommandBuffer*) &m_handle);
    }

    CmdBuffer& CmdBuffer::operator=(CmdBuffer&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_queue, other.m_queue);
        std::swap(m_isShortLived, other.m_isShortLived);

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

    CmdBuffer& CmdBuffer::Begin(Flags<CmdBufferUsage> usage, bool clear)
    {
        if (clear) Clear();
        m_handle.begin(vk::CommandBufferBeginInfo((vk::CommandBufferUsageFlags) usage));

        return *this;
    }

    CmdBuffer& CmdBuffer::End()
    {
        m_handle.end();

        return *this;
    }

    CmdBuffer& CmdBuffer::Submit(const std::vector<SubmitInfo>& submits, const Fence& fence)
    {
        vk::SubmitInfo* submits_ = new vk::SubmitInfo[submits.size()];
        vk::Semaphore** waitSemaphores = new vk::Semaphore * [submits.size()];
        vk::PipelineStageFlags** stages = new vk::PipelineStageFlags * [submits.size()];
        for (unsigned int i = 0; i < (unsigned int) submits.size(); i++)
        {
            waitSemaphores[i] = new vk::Semaphore[submits[i].waitStages.size()];
            stages[i] = new vk::PipelineStageFlags[submits[i].waitStages.size()];
            for (unsigned int j = 0; j < (unsigned int) submits[i].waitStages.size(); j++)
            {
                waitSemaphores[i][j] = (vk::Semaphore) std::get<1>(submits[i].waitStages[j]);
                stages[i][j] = (vk::PipelineStageFlagBits) (Flags<PipelineStage>::TMask) std::get<0>(submits[i].waitStages[j]);
            }
            vk::SubmitInfo sub(submits[i].waitStages.size(), waitSemaphores[i], stages[i], 1, (vk::CommandBuffer*) &m_handle, submits[i].signalSemaphores.size(), (vk::Semaphore*) submits[i].signalSemaphores.data());
            submits_[i] = sub;
        }

        auto result = ((QueueHandle) *m_queue).submit(submits.size(), submits_, (FenceHandle) fence);
        delete[] submits_;
        delete[] waitSemaphores;
        delete[] stages;

        return *this;
    }
}