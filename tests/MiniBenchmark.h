#pragma once
#include <iosfwd>
#include <functional>
#include <vector>
#include <chrono>
#include <iostream>
#include <assert.h>
#include <windows.h>

namespace MiniBenchmark
{
    extern void SetHightPriority();

    struct Samples;
    extern Samples Benchmark(const std::function<void()>& function, unsigned int sampleCount, unsigned int iterationCount = 1);
    extern Samples Benchmark(const std::function<void(int)>& function, unsigned int sampleCount, unsigned int iterationCount = 1);

    struct Samples
    {
    public:
        std::vector<float> values;

    public:
        float Median() const;
        float Average() const;
        float Variance() const;
        Samples DownSampled(unsigned int newSampleCount) const;
    };
    std::ostream& operator<<(std::ostream&, const Samples& o);

    void SetHightPriority()
    {
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    }

    Samples Benchmark(const std::function<void()>& function, unsigned int sampleCount, unsigned int iterationCount)
    {
        Samples samples;
        samples.values.reserve(sampleCount);

        for (unsigned int i = 0; i < sampleCount * 0.25f; i++)
            for (unsigned int j = 0; j < iterationCount; j++)
                function();

        for (unsigned int i = 0; i < sampleCount; i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            for (unsigned int j = 0; j < iterationCount; j++)
            {
                function();
            }
            auto end = std::chrono::high_resolution_clock::now();
            samples.values.push_back(std::chrono::duration<float>(end - start).count());
        }

        return samples;
    }
    Samples Benchmark(const std::function<void(int)>& function, unsigned int sampleCount, unsigned int iterationCount)
    {
        Samples samples;
        samples.values.reserve(sampleCount);

        for (unsigned int i = 0; i < sampleCount * 0.25f; i++)
            for (unsigned int j = 0; j < iterationCount; j++)
                function(0);

        for (unsigned int i = 0; i < sampleCount; i++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            for (unsigned int j = 0; j < iterationCount; j++)
            {
                function(iterationCount * sampleCount);
            }
            auto end = std::chrono::high_resolution_clock::now();
            samples.values.push_back(std::chrono::duration<float>(end - start).count());
        }

        return samples;
    }

    float Samples::Median() const
    {
        std::vector<float> temp(values.begin(), values.end());
        std::sort(temp.begin(), temp.end());

        if (temp.size() % 2 == 0)
            return temp[temp.size() / 2];
        else
            return (temp[temp.size() / 2] + temp[temp.size() / 2 + 1]) * 0.5f;
    }
    float Samples::Average() const
    {
        float sum = 0.0f;
        for (int i = 0; i < values.size(); i++)
        {
            sum += values[i];
        }

        return sum / values.size();
    }
    float Samples::Variance() const
    {
        float mean = Average();
        float variance = 0;
        for (int i = 0; i < values.size(); i++)
        {
            variance += (values[i] - mean) * (values[i] - mean);
        }

        return variance / values.size();
    }
    Samples Samples::DownSampled(unsigned int newSampleCount) const
    {
        assert(newSampleCount < values.size());
        Samples newSamples;
        newSamples.values.resize(newSampleCount);

        float ratio = newSampleCount / (float) values.size();
        for (int i = 0; i < values.size(); i++)
        {
            newSamples.values[i * ratio] += values[i] * ratio;
        }

        return newSamples;
    }
    std::ostream& operator<<(std::ostream& os, const Samples& o)
    {
        for (int i = 0; i < o.values.size() - 1; i++)
        {
            os << "(" << i << ',' << o.values[i] * 1000 << "), ";
        }
        os << "(" << o.values.size() - 1 << ',' << o.values[o.values.size() - 1] * 1000 << ')';

        return os;
    }
}