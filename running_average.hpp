#pragma once

#include <vector>
#include <numeric>
#include <type_traits>
#include "esphome/core/log.h"


/**
 * @brief A circular-buffer based running (moving) average.
 *
 * Keeps the last `length` samples and computes their arithmetic mean.
 * The window length is set at construction time but can be changed via
 * resize(), which clears the history.
 *
 * @tparam T  Numeric value type (float, uint16_t, int, …).
 *            The internal accumulator always uses uint16_t for precision.
 */
template <typename T>
class RunningAverage
{
    static_assert(std::is_arithmetic_v<T>, "T must be a numeric type");

public:
    // ------------------------------------------------------------------ //
    //  Construction                                                        //
    // ------------------------------------------------------------------ //

    /**
     * @param length  Number of samples in the sliding window (≥ 1).
     */
    explicit RunningAverage(std::size_t length)
    {
        resize(length);
    }

    // ------------------------------------------------------------------ //
    //  Configuration                                                       //
    // ------------------------------------------------------------------ //

    /**
     * @brief Change the window length and reset all history.
     * @param length  New window size (≥ 1).
     */
    void resize(std::size_t length)
    {
        if (length == 0) {
			ESP_LOGE("RunningAverage", "Lengte moet >= 1 zijn");
            return; // stop gewoon, gooi geen exception
		}
        m_buffer.assign(length, T{});
        m_length  = length;
        m_head    = 0;
        m_count   = 0;
        m_sum     = 0;
    }

    /** @brief Discard all samples without changing the window length. */
    void reset()
    {
        resize(m_length);
    }

    // ------------------------------------------------------------------ //
    //  Data ingestion                                                      //
    // ------------------------------------------------------------------ //

    /**
     * @brief Push a new measurement into the window.
     * @param value  The new sample.
     * @return       The updated running average.
     */
    uint16_t push(T value)
    {
        // Subtract the oldest sample from the running sum
        m_sum -= static_cast<uint16_t>(m_buffer[m_head]);

        // Overwrite it with the new sample
        m_buffer[m_head] = value;
        m_sum += static_cast<uint16_t>(value);

        // Advance the circular-buffer head
        m_head = (m_head + 1) % m_length;

        // Track how many valid samples we have (up to m_length)
        if (m_count < m_length)
            ++m_count;

        return average();
    }

    // ------------------------------------------------------------------ //
    //  Queries                                                             //
    // ------------------------------------------------------------------ //

    /**
     * @brief Current running average.
     * @note  Returns 0.0 when no samples have been pushed yet.
     */
    uint16_t average() const
    {
        if (m_count == 0) return 0;
        return m_sum / (m_count);
    }
	
    /** @brief Configured window length. */
    std::size_t length() const { return m_length; }

    /** @brief Number of valid samples currently in the window (≤ length()). */
    std::size_t count()  const { return m_count; }

    /** @brief True once the window is fully populated. */
    bool is_full() const { return m_count == m_length; }

    /** @brief Convenience: same as average(). */
    operator uint16_t() const { return average(); }

private:
    std::vector<T> m_buffer;   ///< Circular buffer of raw samples
    std::size_t    m_length{}; ///< Window size
    std::size_t    m_head{};   ///< Write position in the circular buffer
    std::size_t    m_count{};  ///< Number of valid entries so far
    uint16_t         m_sum{};    ///< Running sum of the current window
};
