#include <vulkan/vulkan.hpp>
#include "CommandBuffer.h"

namespace vg
{
    namespace cmd
    {
        void BindPipeline::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        }

        void BindVertexBuffer::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).bindVertexBuffers(0, 1, &buffers, &offset);
        }

        void BindIndexBuffer::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).bindIndexBuffer(buffer, offset, (vk::IndexType) type);
        }

        void BindDescriptorSets::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, firstSet, descriptorSets.size(), &descriptorSets[0], 0, nullptr);
        }

        void BeginRenderpass::operator()(CommandBuffer& commandBuffer) const
        {
            auto clear = vk::ClearColorValue(clearColorR, clearColorG, clearColorB, clearColorA);
            auto clearValues = vk::ClearValue(clear);

            auto info = vk::RenderPassBeginInfo(renderpass, framebuffer, vk::Rect2D({ offsetX,offsetY }, { extendX,extendY }), 1, &clearValues);
            CommandBufferHandle(commandBuffer).beginRenderPass(info, vk::SubpassContents::eInline);
        }

        void SetViewport::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).setViewport(0, 1, (vk::Viewport*) &viewport);
        }

        void SetScissor::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).setScissor(0, 1, (vk::Rect2D*) &scissor);
        }

        void Draw::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }

        void EndRenderpass::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).endRenderPass();
        }

        void DrawIndexed::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void CopyBuffer::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).copyBuffer(src, dst, *(std::vector<vk::BufferCopy>*) & regions);
        }

        void CopyBufferToImage::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).copyBufferToImage(src, dst, (vk::ImageLayout) dstImageLayout, *(std::vector<vk::BufferImageCopy>*) & regions);
        }

        void PipelineBarier::operator()(CommandBuffer& commandBuffer) const
        {
            CommandBufferHandle(commandBuffer).pipelineBarrier((vk::PipelineStageFlags) srcStageMask, (vk::PipelineStageFlags) dstStageMask, (vk::DependencyFlags) dependency, *(std::vector<vk::MemoryBarrier>*) & memoryBarriers, *(std::vector<vk::BufferMemoryBarrier>*) & bufferMemoryBarriers, *(std::vector<vk::ImageMemoryBarrier>*) & imageMemoryBarriers);
        }
    }

    CommandBuffer::CommandBuffer(const Queue& queue) : m_queue(&queue)
    {
        m_handle = ((DeviceHandle) currentDevice).allocateCommandBuffers({ queue.GetCommandPool(), vk::CommandBufferLevel::ePrimary, 1 })[0];

    }

    CommandBuffer::CommandBuffer() :m_handle(nullptr), m_queue(nullptr) {}

    CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_queue, other.m_queue);
    }

    CommandBuffer::~CommandBuffer()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) currentDevice).freeCommandBuffers(m_queue->GetCommandPool(), 1, (vk::CommandBuffer*) &m_handle);
    }

    CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_queue, other.m_queue);

        return *this;
    }

    CommandBuffer::operator const CommandBufferHandle& () const
    {
        return m_handle;
    }

    void CommandBuffer::Clear()
    {
        m_handle.reset();
    }

    void CommandBuffer::Begin(bool clear)
    {
        if (clear) Clear();
        m_handle.begin(vk::CommandBufferBeginInfo());
    }

    void CommandBuffer::End()
    {
        m_handle.end();
    }

    void CommandBuffer::Submit(const std::vector<SubmitInfo>& submits, const Fence& fence)
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
    }
}