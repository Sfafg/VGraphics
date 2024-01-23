#include <vulkan/vulkan.hpp>
#include "CommandBuffer.h"

namespace vg
{
    CommandBuffer::CommandBuffer(const Queue& queue) : m_queue(&queue)
    {
        m_handle = m_queue->GetDevice().allocateCommandBuffers({ queue.GetCommandPool(), vk::CommandBufferLevel::ePrimary, 1 })[0];
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
        m_queue->GetDevice().freeCommandBuffers(m_queue->GetCommandPool(), 1, (vk::CommandBuffer*) &m_handle);
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
    void CommandBuffer::Begin()
    {
        m_handle.begin(vk::CommandBufferBeginInfo());
    }
    void CommandBuffer::End()
    {
        m_handle.end();
    }

    template<> void CommandBuffer::Append(cmd::BindPipeline command)
    {
        m_handle.bindPipeline(vk::PipelineBindPoint::eGraphics, command.pipeline);
    }
    template<> void CommandBuffer::Append(cmd::BeginRenderpass command)
    {
        auto clear = vk::ClearColorValue(command.clearColorR, command.clearColorG, command.clearColorB, command.clearColorA);
        auto clearValues = vk::ClearValue(clear);
        auto info = vk::RenderPassBeginInfo(command.renderpass, command.framebuffer, vk::Rect2D({ command.offsetX, command.offsetY }, { command.extendX,command.extendY }), 1, &clearValues);
        m_handle.beginRenderPass(info, vk::SubpassContents::eInline);
    }
    template<> void CommandBuffer::Append(cmd::SetViewport command)
    {
        m_handle.setViewport(0, 1, (vk::Viewport*) &command.viewport);
    }
    template<> void CommandBuffer::Append(cmd::SetScissor command)
    {
        m_handle.setScissor(0, 1, (vk::Rect2D*) &command.scissor);
    }
    template<> void CommandBuffer::Append(cmd::Draw command)
    {
        m_handle.draw(command.vertexCount, command.instanceCount, command.firstVertex, command.firstInstance);
    }
    template<> void CommandBuffer::Append(cmd::EndRenderpass command)
    {
        m_handle.endRenderPass();
    }
}