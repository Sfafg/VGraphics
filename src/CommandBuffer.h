#pragma once
#include "Queue.h"
#include "Structs.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "Synchronization.h"
#include "Buffer.h"
#include "Image.h"

namespace vg
{
    class CommandBuffer;
    namespace cmd
    {
        struct Command {};
        struct BindPipeline : Command
        {
            BindPipeline(GraphicsPipelineHandle pipeline) : pipeline(pipeline) {}

        private:
            GraphicsPipelineHandle pipeline;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct BindVertexBuffer : Command
        {
            BindVertexBuffer(BufferHandle buffers, uint64_t offset) :buffers(buffers), offset(offset) {}

        private:
            BufferHandle buffers;
            uint64_t offset;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct BindIndexBuffer : Command
        {
            BindIndexBuffer(BufferHandle buffer, uint64_t offset, uint32_t type) :buffer(buffer), offset(offset), type(type) {}

        private:
            BufferHandle buffer;
            uint64_t offset;
            uint32_t type;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct BindDescriptorSets : Command
        {
            BindDescriptorSets(PipelineLayoutHandle layout, uint32_t firstSet, std::vector<DescriptorSetHandle> descriptorSets) :layout(layout), firstSet(firstSet), descriptorSets(descriptorSets) {}

        private:
            PipelineLayoutHandle layout;
            uint32_t firstSet;
            std::vector<DescriptorSetHandle> descriptorSets;

            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct BeginRenderpass : Command
        {
            BeginRenderpass(const RenderPass& renderpass, const Framebuffer& framebuffer, int32_t offsetX, int32_t offsetY, uint32_t extendX, uint32_t extendY, float clearColorR = 0, float clearColorG = 0, float clearColorB = 0, float clearColorA = 0)
                : renderpass(renderpass), framebuffer(framebuffer), offsetX(offsetX), offsetY(offsetY), extendX(extendX), extendY(extendY), clearColorR(clearColorR), clearColorG(clearColorG), clearColorB(clearColorB), clearColorA(clearColorA)
            {}

        private:
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
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct SetViewport : Command
        {
            SetViewport(const Viewport& viewport) : viewport(viewport) {}

        private:
            Viewport viewport;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct SetScissor : Command
        {
            SetScissor(const Scissor& scissor) : scissor(scissor) {}

        private:
            Scissor scissor;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct Draw : Command
        {
            Draw(int vertexCount, int instanceCount = 1, int firstVertex = 0, int firstInstance = 0)
                :vertexCount(vertexCount), instanceCount(instanceCount), firstVertex(firstVertex), firstInstance(firstInstance)

            {}

        private:
            int vertexCount;
            int instanceCount;
            int firstVertex;
            int firstInstance;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct EndRenderpass : Command
        {
            EndRenderpass() {}

        private:
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct DrawIndexed : Command
        {
            DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0)
                : indexCount(indexCount), instanceCount(instanceCount), firstIndex(firstIndex), vertexOffset(vertexOffset), firstInstance(firstInstance)
            {}

        private:
            uint32_t indexCount;
            uint32_t instanceCount;
            uint32_t firstIndex;
            uint32_t vertexOffset;
            uint32_t firstInstance;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
        struct CopyBuffer : Command
        {
            CopyBuffer(Buffer& src, Buffer& dst, const std::vector<BufferCopyRegion>& regions) : src(src), dst(dst), regions(regions) {}

        private:
            BufferHandle src;
            BufferHandle dst;
            std::vector<BufferCopyRegion> regions;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };

        struct CopyBufferToImage : Command
        {
            CopyBufferToImage(const Buffer& src, const Image& dst, ImageLayout dstImageLayout, std::vector<BufferImageCopyRegion> regions)
                : src(src), dst(dst), dstImageLayout(dstImageLayout), regions(regions)
            {}

        private:
            BufferHandle src;
            ImageHandle dst;
            ImageLayout dstImageLayout;
            std::vector<BufferImageCopyRegion> regions;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };

        struct PipelineBarier : Command
        {
            PipelineBarier(Flags<PipelineStage> srcStageMask, Flags<PipelineStage> dstStageMask, Flags<Dependency> dependency, std::vector<MemoryBarrier> memoryBarriers, std::vector<BufferMemoryBarrier> bufferMemoryBarriers = {}, std::vector<ImageMemoryBarrier> imageMemoryBarriers = {})
                :srcStageMask(srcStageMask), dstStageMask(dstStageMask), dependency(dependency), memoryBarriers(memoryBarriers), bufferMemoryBarriers(bufferMemoryBarriers), imageMemoryBarriers(imageMemoryBarriers)
            {}

            PipelineBarier(Flags<PipelineStage> srcStageMask, Flags<PipelineStage> dstStageMask, Flags<Dependency> dependency, std::vector<BufferMemoryBarrier> bufferMemoryBarriers, std::vector<ImageMemoryBarrier> imageMemoryBarriers = {})
                :srcStageMask(srcStageMask), dstStageMask(dstStageMask), dependency(dependency), bufferMemoryBarriers(bufferMemoryBarriers), imageMemoryBarriers(imageMemoryBarriers)
            {}

            PipelineBarier(Flags<PipelineStage> srcStageMask, Flags<PipelineStage> dstStageMask, Flags<Dependency> dependency, std::vector<ImageMemoryBarrier> imageMemoryBarriers)
                :srcStageMask(srcStageMask), dstStageMask(dstStageMask), dependency(dependency), imageMemoryBarriers(imageMemoryBarriers)
            {}

        private:
            Flags<PipelineStage> srcStageMask;
            Flags<PipelineStage> dstStageMask;
            Flags<Dependency> dependency;
            std::vector<MemoryBarrier> memoryBarriers;
            std::vector<BufferMemoryBarrier> bufferMemoryBarriers;
            std::vector<ImageMemoryBarrier> imageMemoryBarriers;
            void operator ()(CommandBuffer& commandBuffer) const;
            friend CommandBuffer;
        };
    }

    /**
     *@brief Holds information about submiting command buffer
     *
     */
    struct SubmitInfo
    {
        std::vector <std::tuple<Flags<PipelineStage>, SemaphoreHandle>> waitStages;
        std::vector<SemaphoreHandle> signalSemaphores;
        /**
         * @brief Construct a new Submit Info object
         *
         * @param waitStages Array of pairs of values FlagSet of pipeline stages and Semaphore for which to wait on those stages
         * @param signalSemaphores Semaphores signalled upon all command buffers complete execution
         */
        SubmitInfo(const std::vector < std::tuple<Flags<PipelineStage>, SemaphoreHandle>>& waitStages, const std::vector<SemaphoreHandle>& signalSemaphores)
            :waitStages(waitStages), signalSemaphores(signalSemaphores)
        {}
    };

    template<class T>
    concept Command = std::is_base_of<cmd::Command, T>::value;
    template<class... T>
    concept Commands = (std::is_base_of<cmd::Command, T>::value && ...);
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
         *@brief Append commands, has to be between \ref CommandBuffer::Begin() and \ref CommandBuffer::End()
         * Appends array commands to the buffer
         * @tparam T class derived from cmd::Command
         * @param commands Array of commands from cmd:: namespace
         */
        template<Commands... T>
        void AppendNoBegin(const T&... commands) { (..., _Append(commands)); }
        /**
         *@brief Append commands
         * Appends array commands to the buffer
         * @tparam T class derived from cmd::Command
         * @param commands Array of commands from cmd:: namespace
         */
        template<Commands... T>
        void Append(const T&... commands) { Begin(); (..., _Append(commands)); End(); }
        /**
         *@brief End appending to command buffer
        * Has to be called before submiting
        */
        void End();

        /**
         *@brief Submit command buffers and all relevant data
        *
        * @param submits Command buffers and relevant data
        * @param fence Fence to be signaled upon all submits finish
        */
        void Submit(const std::vector<SubmitInfo>& submits, const Fence& fence);
        /**
         *@brief Submit command buffer and all relevant data
         *
         *@param submits Command buffers and relevant data
         *@return Fence to be signaled upon all submits finish
         */
        Fence Submit(const std::vector<SubmitInfo>& submits)
        {
            Fence fence;
            Submit(submits, fence);
            return fence;
        }

    private:
        template<Command T>
        void _Append(const T& command) { command(*this); };

    private:
        CommandBufferHandle m_handle;
        const Queue* m_queue;
    };
}