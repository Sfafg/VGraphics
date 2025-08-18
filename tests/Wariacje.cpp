#include <iostream>
#include <chrono>
#include <math.h>
#include "Instance.h"
#include "Device.h"
#include "Shader.h"
#include "CmdBuffer.h"
#include "Synchronization.h"
#include "Flags.h"
#include "Buffer.h"
#include "MemoryManager.h"
#include "DescriptorPool.h"
#include "FormatInfo.h"
#include "CmdPool.h"
#include "ComputePipeline.h"
#include "PipelineCache.h"
#include "QueryPool.h"

using namespace std::chrono_literals;
using namespace vg;

int main() {
    // Stwórz instancje.
    vg::instance = Instance(
        {},
        [](MessageSeverity severity, const char *message) {
            if (severity < MessageSeverity::Warning) return;
            std::cout << message << '\n' << '\n';
        },
        false
    );

    // Stwórz kolejki, i wybierz urządzenie.
    Queue computeQueue({QueueType::Compute}, 1.0f);
    Device computeDevice(
        {&computeQueue}, {}, {},
        [](auto id, auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits,
           DeviceFeatures features) { return (type == DeviceType::Integrated); }
    );
    vg::currentDevice = &computeDevice;

    int n = 2, k = 3;
    int iloscWariacji = pow(n, k);

    // Stwórz buffer dla wariacji.
    Buffer bufferWariacji(iloscWariacji * k * sizeof(int), {BufferUsage::StorageBuffer});
    Allocate(&bufferWariacji, {MemoryProperty::HostVisible, MemoryProperty::HostCoherent});

    // Stwórz pipeline i descriptory.
    Shader computeShader(ShaderStage::Compute, "resources/shaders/wariacje.comp.spv");
    ComputePipeline computePipeline(
        computeShader, PipelineLayout(
                           {{DescriptorSetLayoutBinding(0, DescriptorType::StorageBuffer, 1, ShaderStage::Compute)}},
                           {PushConstantRange(ShaderStage::Compute, 0, sizeof(int) * 3)}
                       )
    );

    DescriptorPool descriptorPool(1, {{DescriptorType::StorageBuffer, 1}});
    vg::DescriptorSet descriptorSet;
    descriptorPool.Allocate(computePipeline.GetPipelineLayout().GetDescriptorSets()[0], &descriptorSet);
    descriptorSet.AttachBuffer(DescriptorType::StorageBuffer, bufferWariacji, 0, bufferWariacji.GetSize(), 0, 0);

    int threadGroupCount = std::ceil(iloscWariacji / 1024.0); // dzielone przez 1024 bo taki jest rozmiar grupy
    auto start = std::chrono::high_resolution_clock::now();
    CmdBuffer cmdBuffer(computeQueue);
    cmdBuffer.Begin({})
        .Append(
            cmd::BindPipeline(computePipeline),
            cmd::PushConstants(
                computePipeline.GetPipelineLayout(), ShaderStage::Compute, 0, std::make_tuple(k, n, iloscWariacji)
            ),
            cmd::BindDescriptorSets(
                computePipeline.GetPipelineLayout(), PipelineBindPoint::Compute, 0, {descriptorSet}
            ),
            cmd::Dispatch(threadGroupCount, 1, 1)
        )
        .End()
        .Submit()
        .Await();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Wariacji n = " << n << " z k = " << k << " jest " << iloscWariacji << '\n'
              << "Wygnerowano w: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << '\n'
              << "Czas na wariacje: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) / float(iloscWariacji) << '\n';

    int *wariacje = (int *)bufferWariacji.MapMemory();
    if (n <= 4 && k <= 4) {
        for (int i = 0; i < iloscWariacji; i++) {
            for (int j = 0; j < k - 1; j++) std::cout << (int)wariacje[i * k + j] << ", ";
            std::cout << (int)wariacje[i * k + k - 1] << '\n';
        }
    }
}
