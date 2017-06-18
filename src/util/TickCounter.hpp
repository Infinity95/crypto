#pragma once

#ifndef AKI_ENGINE_TICK_COUNTER_HPP_
#define AKI_ENGINE_TICK_COUNTER_HPP_
#include <chrono>
#include "akiEngine/util/Logging.hpp"

namespace akiEngine
{

/**
 * \brief Average strategy that uses exponential smoothing.
 * 
 * This Average strategy calculates the average using exponential smoothing.
 * For more information see https://en.wikipedia.org/wiki/Exponential_smoothing
 */
template<unsigned int num, unsigned int den>
class TickCounterExpSmoothing
{
private:

    double m_smoothing;
    double m_oneMinusSmoothing;
    double m_tps;
    const double m_epsilon = 0.000'000'000'01;

public:
    TickCounterExpSmoothing() noexcept;

    /**
     * \brief Calculates the average based on the time delta to the last tick.
     */
    double update(const std::chrono::nanoseconds& timeDelta) noexcept;
};

template<size_t sampleSize>
class TickCounterSimpleMovingAverage
{
private:
    static_assert(sampleSize > 0, "Sample size has to be greater than 0");
    double m_sum = 0;
    double m_tps = 0;
    double m_tpsSamples[sampleSize];
    size_t m_currentPosition = 0;

    const double m_epsilon = 0.000'000'000'01;

public:

    TickCounterSimpleMovingAverage() noexcept;

    double update(const std::chrono::nanoseconds& timeDelta) noexcept;
};

/**
 * \brief This class is used to measure ticks.
 * 
 * It can be used anywhere in the program.
 * Each time a tick elapses the counter needs to be increment by
 * calling either ++tickCounter or tickCounter++
 */
template <class AverageStrategy>
class TickCounter final : public AverageStrategy
{
private:
    double m_tps;
    unsigned long long m_ticks;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTickTime;

public:

    /**
     * \brief Constructs a tick counter
     */
    TickCounter() noexcept;

    /**
     * \brief Gets the current ticks per second
     */
    auto getTicksPerSecond() const noexcept
    {
        return static_cast<unsigned long long>(round(m_tps));
    }

    /**
     * \brief Gets the tick count since the counter was created
     */
    auto getTicks() const noexcept
    {
        return m_ticks;
    }

    /**
     * \brief Advances the tick count by one
     */
    TickCounter& operator++() noexcept;

    /**
     * \brief Advances the tick count by one
     */
    TickCounter operator++(int) noexcept;
private:
};
}

#include "TickCounter.inl"

#endif // AKI_ENGINE_TICK_COUNTER_HPP_
