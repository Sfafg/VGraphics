#pragma once
#include "Queue.h"
#include "Structs.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include <tuple>
#include <iostream>

namespace vg
{
    namespace cmd
    {
        class Command {};
        struct BindPipeline : Command
        {
            GraphicsPipelineHandle pipeline;
            BindPipeline(GraphicsPipelineHandle pipeline) : pipeline(pipeline) {}
        };
        struct BeginRenderpass : Command
        {
            RenderPassHandle renderpass;
            FramebufferHandle framebuffer;
            int32_t offsetX;
            int32_t offsetY;
            uint32_t extendX;
            uint32_t extendY;
            float clearColorR;
            float clearColorG;
            float clearColorB;
            float clearColorA;
            BeginRenderpass(const RenderPass& renderpass, const Framebuffer& framebuffer, int32_t offsetX, int32_t offsetY, uint32_t extendX, uint32_t extendY, float clearColorR = 0, float clearColorG = 0, float clearColorB = 0, float clearColorA = 0)
                : renderpass(renderpass), framebuffer(framebuffer), offsetX(offsetX), offsetY(offsetY), extendX(extendX), extendY(extendY), clearColorR(clearColorR), clearColorG(clearColorG), clearColorB(clearColorB), clearColorA(clearColorA)
            {}
        };
        struct SetViewport : Command
        {
            Viewport viewport;
            SetViewport(const Viewport& viewport) : viewport(viewport) {}
        };
        struct SetScissor : Command
        {
            Scissor scissor;
            SetScissor(const Scissor& scissor) : scissor(scissor) {}
        };
        struct Draw : Command
        {
            int vertexCount;
            int instanceCount;
            int firstVertex;
            int firstInstance;
            Draw(int vertexCount, int instanceCount, int firstVertex = 0, int firstInstance = 0)
                :vertexCount(vertexCount), instanceCount(instanceCount), firstVertex(firstVertex), firstInstance(firstInstance)
            {}
        };
        struct EndRenderpass : Command
        {
            EndRenderpass() {}
        };
    }

    template<class T, class U>
    concept Derived = std::is_base_of<U, T>::value;
    class CommandBuffer
    {
    public:
        CommandBuffer(const Queue& queue);

        CommandBuffer();
        CommandBuffer(CommandBuffer&& other) noexcept;
        CommandBuffer(const CommandBuffer& other) = delete;
        ~CommandBuffer();

        CommandBuffer& operator=(CommandBuffer&& other) noexcept;
        CommandBuffer& operator=(const CommandBuffer& other) = delete;
        operator const CommandBufferHandle& () const;

        void Clear();
        void Begin();
        template<Derived<cmd::Command> T>
        void Append(T command);
        void End();

    private:
        CommandBufferHandle m_handle;
        const Queue* m_queue;
    };
}