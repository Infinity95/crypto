#include "akiEngine/util/TickCounter.hpp"

namespace akiEngine
{
template <unsigned int num, unsigned int den>
TickCounterExpSmoothing<num, den>::TickCounterExpSmoothing() noexcept:
    m_smoothing(static_cast<double>(num) / den),
    m_oneMinusSmoothing(1 - m_smoothing),
    m_tps(1)
{
}

template <unsigned int num, unsigned int den>
double TickCounterExpSmoothing<num, den>::update(const std::chrono::nanoseconds& timeDelta) noexcept
{
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(timeDelta).count() + m_epsilon;
    auto currTps = 1 / (millis * 0.001);
    m_tps = m_tps * m_smoothing + currTps * m_oneMinusSmoothing;

    return m_tps;
}

template <size_t sampleSize>
TickCounterSimpleMovingAverage<sampleSize>::TickCounterSimpleMovingAverage() noexcept
{
    for (auto& sample : m_tpsSamples)
    {
        sample = 0;
    }
}

template <size_t sampleSize>
double TickCounterSimpleMovingAverage<sampleSize>::update(const std::chrono::nanoseconds& timeDelta) noexcept
{
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(timeDelta).count() + m_epsilon;
    auto currTps = 1 / (millis * 0.001);

    m_sum -= m_tpsSamples[m_currentPosition];
    m_sum += currTps;

    m_tpsSamples[m_currentPosition] = currTps;

    ++m_currentPosition;
    m_currentPosition %= sampleSize;

    m_tps = m_sum / sampleSize;

    return m_tps;
}

template <class AverageStrategy>
TickCounter<AverageStrategy>::TickCounter() noexcept :
    m_tps(60),
    m_ticks(0),
    m_lastTickTime(std::chrono::high_resolution_clock::now())
{
}

template <class AverageStrategy>
TickCounter<AverageStrategy>& TickCounter<AverageStrategy>::operator++() noexcept
{
    ++m_ticks;

    auto now = std::chrono::high_resolution_clock::now();
    auto delta = now - m_lastTickTime;

    m_tps = AverageStrategy::update(delta);

    m_lastTickTime = now;

    return *this;
}

template <class AverageStrategy>
TickCounter<AverageStrategy> TickCounter<AverageStrategy>::operator++(int) noexcept
{
    auto result{*this};
    ++*this;
    return result;
}
}
