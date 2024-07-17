#pragma once
#include "Queue.h"
#include "CmdPool.h"
#include "Structs.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "Synchronization.h"
#include "Buffer.h"
#include "Image.h"

namespace vg
{
    class CmdBuffer;
    namespace cmd
    {
        struct Command {};
        struct BindPipeline : Command
        {
            BindPipeline(GraphicsPipelineHandle pipeline) : pipeline(pipeline) {}

        private:
            GraphicsPipelineHandle pipeline;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct BindVertexBuffer : Command
        {
            BindVertexBuffer(BufferHandle buffers, uint64_t offset) :buffers(buffers), offset(offset) {}

        private:
            BufferHandle buffers;
            uint64_t offset;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct BindIndexBuffer : Command
        {
            BindIndexBuffer(BufferHandle buffer, uint64_t offset, IndexType type) :buffer(buffer), offset(offset), type(type) {}

        private:
            BufferHandle buffer;
            uint64_t offset;
            IndexType type;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct BindDescriptorSets : Command
        {
            BindDescriptorSets(PipelineLayoutHandle layout, uint32_t firstSet, std::vector<DescriptorSetHandle> descriptorSets) :layout(layout), firstSet(firstSet), descriptorSets(descriptorSets) {}

        private:
            PipelineLayoutHandle layout;
            uint32_t firstSet;
            std::vector<DescriptorSetHandle> descriptorSets;

            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct BeginRenderpass : Command
        {
            BeginRenderpass(const RenderPass& renderpass, const Framebuffer& framebuffer, int32_t offsetX, int32_t offsetY, uint32_t extendX, uint32_t extendY, float clearColorR = 0, float clearColorG = 0, float clearColorB = 0, float clearColorA = 0, float depthClearColor = 1, float stencilClearColor = 0)
                : renderpass(renderpass), framebuffer(framebuffer), offsetX(offsetX), offsetY(offsetY), extendX(extendX), extendY(extendY), clearColorR(clearColorR), clearColorG(clearColorG), clearColorB(clearColorB), clearColorA(clearColorA), depthClearColor(depthClearColor), stencilClearColor(stencilClearColor)
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
            float depthClearColor;
            float stencilClearColor;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct SetViewport : Command
        {
            SetViewport(const Viewport& viewport) : viewports{ viewport }, first(0) {}
            SetViewport(const std::vector<Viewport>& vieports, int first = 0) : viewports(vieports), first(first) {}

        private:
            uint32_t first;
            std::vector<Viewport> viewports;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct SetScissor : Command
        {
            SetScissor(const Scissor& scissor) : scissors{ scissor }, first(0) {}
            SetScissor(const std::vector<Scissor>& vieports, int first = 0) : scissors(vieports), first(first) {}

        private:
            uint32_t first;
            std::vector<Scissor> scissors;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
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
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct EndRenderpass : Command
        {
            EndRenderpass() {}

        private:
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
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
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
        struct CopyBuffer : Command
        {
            CopyBuffer(Buffer& src, Buffer& dst, const std::vector<BufferCopyRegion>& regions) : src(src), dst(dst), regions(regions) {}

        private:
            BufferHandle src;
            BufferHandle dst;
            std::vector<BufferCopyRegion> regions;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
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
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
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
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };

        struct ExecuteCommands : Command
        {
            ExecuteCommands(const std::vector<CmdBufferHandle>& cmdBuffers)
                : cmdBuffers(cmdBuffers)
            {}

        private:
            std::vector<CmdBufferHandle> cmdBuffers;
            void operator ()(CmdBuffer& commandBuffer) const;
            friend CmdBuffer;
        };
    }

    template<class T>
    concept Command = std::is_base_of<cmd::Command, T>::value;
    template<class... T>
    concept Commands = (std::is_base_of<cmd::Command, T>::value && ...);
    /**
     *@brief Array of commands
     * Used to send commands to the GPU in one big batch improving performance
     */
    class CmdBuffer
    {
    public:
        /**
         *@brief Construct a new Command Buffer object
         *
         * @param queue Queue object
         */
        CmdBuffer(const Queue& queue, bool isShortLived = true, CmdBufferLevel cmdLevel = CmdBufferLevel::Primary);
        CmdBuffer(const CmdPool& pool, CmdBufferLevel cmdLevel = CmdBufferLevel::Primary);

        CmdBuffer();
        CmdBuffer(CmdBuffer&& other) noexcept;
        CmdBuffer(const CmdBuffer& other) = delete;
        ~CmdBuffer();

        CmdBuffer& operator=(CmdBuffer&& other) noexcept;
        CmdBuffer& operator=(const CmdBuffer& other) = delete;
        operator const CmdBufferHandle& () const;

        /**
         *@brief Clear commands
         *
         */
        CmdBuffer& Clear();

        /**
         *@brief Begin appending to the command buffer
         *  Needs to be called before all Appends
         * @param usage how the recorded buffer will be used
         */
        CmdBuffer& Begin(Flags<CmdBufferUsage> usage = { CmdBufferUsage::OneTimeSubmit });

        CmdBuffer& Begin(Flags<CmdBufferUsage> usage,
            RenderPassHandle renderPass, uint32_t subpassIndex, FramebufferHandle framebuffer = FramebufferHandle(), bool occlusionQueryEnable = false, Flags<QueryControl> queryFlags = {}, Flags<PipelineStatistic> pipelineStatistics = {});

        /**
         *@brief Append commands, has to be between \ref CommandBuffer::Begin() and \ref CommandBuffer::End()
         * Appends array commands to the buffer
         * @tparam T class derived from cmd::Command
         * @param commands Array of commands from cmd:: namespace
         */
        template<Commands... T>
        CmdBuffer& Append(const T&... commands) { (..., _Append(commands)); return *this; }

        /**
         *@brief End appending to command buffer
        * Has to be called before submiting
        */
        CmdBuffer& End();

        /**
         *@brief Submit command buffers and all relevant data
        *
        * @param submits Synchronization info
        * @param fence Fence to be signaled upon all submits finish
        */
        CmdBuffer& Submit(const std::vector<std::tuple<Flags<PipelineStage>, SemaphoreHandle>>& waitStages, const std::vector<SemaphoreHandle>& signalSemaphores, const Fence& fence);
        /**
         *@brief Submit command buffer and all relevant data
         *
         *@param submits Synchronization info
         *@return Fence to be signaled upon all submits finish
         */

        Fence Submit(const std::vector<std::tuple<Flags<PipelineStage>, SemaphoreHandle>>& waitStages = {}, const std::vector<SemaphoreHandle>& signalSemaphores = {})
        {
            Fence fence;
            Submit(waitStages, signalSemaphores, fence);
            return fence;
        }

    private:
        template<Command T>
        void _Append(const T& command) { command(*this); };

    private:
        CmdBufferHandle m_handle;
        CmdPoolHandle m_commandPool;
        QueueHandle m_queue;
    };
}