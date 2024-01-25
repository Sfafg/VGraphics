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

    /**
     *@brief Array of commands
     * Used to send commands to the GPU in one big batch improving performance
     */
    class CommandBuffer
    {
    public:
        /**
         *@brief Construct a new Command Buffer object
         *
         * @param queue Queue object
         */
        CommandBuffer(const Queue& queue);

        CommandBuffer();
        CommandBuffer(CommandBuffer&& other) noexcept;
        CommandBuffer(const CommandBuffer& other) = delete;
        ~CommandBuffer();

        CommandBuffer& operator=(CommandBuffer&& other) noexcept;
        CommandBuffer& operator=(const CommandBuffer& other) = delete;
        operator const CommandBufferHandle& () const;

        /**
         *@brief Clear commands
         *
         */
        void Clear();
        /**
         *@brief Begin appending to the command buffer
         *  Needs to be called before all Appends
         * @param clear if true buffer gets cleared before beginning
         */
        void Begin(bool clear = true);
        /**
         *@brief Append command
         * Appends a new command to the buffer, has to be between Begin() and End()
         * @tparam T class derived from cmd::Command
         * @param command
         */
        template<Derived<cmd::Command> T>
        void Append(T command);
        /**
         *@brief End appending to command buffer
         * Has to be called before submiting
         */
        void End();

    private:
        CommandBufferHandle m_handle;
        const Queue* m_queue;
    };
}