#pragma once

/*
------------------------------------------------------------------------------
D_Preparation/timestamp_stabilizer.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the timestamp stabilizer interface and implementations for the
preparation layer.

It answers narrow questions such as:
    "How can timestamp stabilization be abstracted as a composable component?"
    "What timestamp strategies are available for different deployment scenarios?"
    "How can timestamp stabilizers be injected into envelope assemblers?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- timestamp injection is a preparation concern
- timestamps should be stabilized during envelope assembly
- different timestamp strategies may be needed for different contexts
- timestamp stabilization should be composable and testable

This means the preparation layer needs:
- a common timestamp stabilizer interface
- multiple timestamp strategies for different use cases
- composability with envelope assemblers
- testability through dependency injection

Current minimal scope
---------------------
This file currently provides:
- `TimestampStabilizer` concept for interface definition
- `TimestampStabilizerBase<TDerived>` CRTP base class
- `UtcEpochMillisStabilizer` - production UTC timestamp strategy
- `MonotonicStabilizer` - relative time measurements
- `FixedTimeStabilizer` - deterministic testing timestamps
- `CustomTimeSourceStabilizer` - dependency injection for testing

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- high-precision timestamp stabilizers (microseconds/nanoseconds)
- time zone aware stabilizers
- distributed system offset stabilizers
- NTP-synchronized stabilizers
- performance monitoring stabilizers

What should NOT live here
-------------------------
This file must NOT:
- define envelope structures
- perform envelope assembly
- manage time zones or localization
- become a global time service
- handle timestamp formatting/display

Design rule
-----------
This file defines timestamp stabilization strategies only.
Each stabilizer provides a consistent interface for timestamp generation
and envelope timestamp injection, enabling composable timestamp handling
across different deployment and testing scenarios.
------------------------------------------------------------------------------
*/

#include <chrono>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace logging_system::D_Preparation {

// Local implementation to avoid LSP issues with cross-namespace includes
[[nodiscard]] inline std::int64_t current_utc_epoch_millis() {
    const auto now = std::chrono::system_clock::now();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());
    return static_cast<std::int64_t>(millis.count());
}

// -----------------------------------------------------------------------------
// Timestamp Stabilizer Type Traits (C++17 alternative to concepts)
// -----------------------------------------------------------------------------

// Primary template - not a timestamp stabilizer
template <typename T, typename = void>
struct is_timestamp_stabilizer : std::false_type {};

// Specialization - check if T has required interface using SFINAE
template <typename T>
struct is_timestamp_stabilizer<T,
    std::void_t<
        decltype(std::declval<const T&>().stabilize()),
        std::enable_if_t<std::is_same_v<
            decltype(std::declval<const T&>().stabilize()), int64_t>>,
        std::enable_if_t<std::is_default_constructible_v<T>>
    >> : std::true_type {};

// Helper variable template
template <typename T>
inline constexpr bool is_timestamp_stabilizer_v = is_timestamp_stabilizer<T>::value;

// Concept-like check with static_assert (can be used in templates)
#define TIMESTAMP_STABILIZER_CHECK(T) \
    static_assert(is_timestamp_stabilizer_v<T>, \
        "Type " #T " does not satisfy TimestampStabilizer requirements: " \
        "must have stabilize() -> int64_t and be default constructible")

// -----------------------------------------------------------------------------
// Base CRTP Class for Common Functionality
// -----------------------------------------------------------------------------

// CRTP base class providing common timestamp stabilization functionality
template <typename TDerived>
class TimestampStabilizerBase {
public:
    // Stabilize timestamp into an envelope by setting content_updated_at_epoch
    template <typename TEnvelope>
    void stabilize_into(TEnvelope& envelope) const {
        envelope.content_updated_at_epoch = static_cast<const TDerived&>(*this).stabilize();
    }

    // Stabilize timestamp into an envelope and return by value
    template <typename TEnvelope>
    TEnvelope stabilize_into(TEnvelope envelope) const {
        envelope.content_updated_at_epoch = static_cast<const TDerived&>(*this).stabilize();
        return envelope;
    }

protected:
    // Allow derived classes to access base functionality
    TimestampStabilizerBase() = default;
};

// -----------------------------------------------------------------------------
// Production UTC Timestamp Stabilizer
// -----------------------------------------------------------------------------

// Production default: UTC epoch milliseconds from system clock
class UtcEpochMillisStabilizer final : public TimestampStabilizerBase<UtcEpochMillisStabilizer> {
public:
    UtcEpochMillisStabilizer() = default;

    // Get current UTC timestamp in milliseconds since epoch
    [[nodiscard]] std::int64_t stabilize() const {
        return current_utc_epoch_millis();
    }
};

// -----------------------------------------------------------------------------
// Monotonic Clock Stabilizer
// -----------------------------------------------------------------------------

// For relative time measurements: monotonic clock (not affected by system time changes)
class MonotonicStabilizer final : public TimestampStabilizerBase<MonotonicStabilizer> {
public:
    MonotonicStabilizer() = default;

    // Get monotonic timestamp in nanoseconds (relative time, not wall time)
    [[nodiscard]] int64_t stabilize() const {
        const auto now = std::chrono::steady_clock::now();
        const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch());
        return static_cast<int64_t>(nanos.count());
    }
};

// -----------------------------------------------------------------------------
// Fixed Time Stabilizer (Testing)
// -----------------------------------------------------------------------------

// For testing and deterministic behavior: always returns the same timestamp
class FixedTimeStabilizer final : public TimestampStabilizerBase<FixedTimeStabilizer> {
public:
    // Construct with a fixed timestamp value
    explicit FixedTimeStabilizer(int64_t fixed_timestamp)
        : fixed_timestamp_(fixed_timestamp) {}

    // Return the fixed timestamp
    [[nodiscard]] int64_t stabilize() const {
        return fixed_timestamp_;
    }

private:
    int64_t fixed_timestamp_;
};

// -----------------------------------------------------------------------------
// Custom Time Source Stabilizer (Dependency Injection)
// -----------------------------------------------------------------------------

// For testing and advanced scenarios: inject custom time source function
class CustomTimeSourceStabilizer final : public TimestampStabilizerBase<CustomTimeSourceStabilizer> {
public:
    // Time source function type
    using TimeSource = std::function<int64_t()>;

    // Construct with custom time source function
    explicit CustomTimeSourceStabilizer(TimeSource time_source)
        : time_source_(std::move(time_source)) {}

    // Call the injected time source function
    [[nodiscard]] int64_t stabilize() const {
        return time_source_();
    }

private:
    TimeSource time_source_;
};

// -----------------------------------------------------------------------------
// High Precision Timestamp Stabilizer
// -----------------------------------------------------------------------------

// For high-precision timing: UTC epoch microseconds
class HighPrecisionStabilizer final : public TimestampStabilizerBase<HighPrecisionStabilizer> {
public:
    HighPrecisionStabilizer() = default;

    // Get current UTC timestamp in microseconds since epoch
    [[nodiscard]] int64_t stabilize() const {
        const auto now = std::chrono::system_clock::now();
        const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch());
        return static_cast<int64_t>(micros.count());
    }
};

// -----------------------------------------------------------------------------
// Offset Timestamp Stabilizer (Distributed Systems)
// -----------------------------------------------------------------------------

// For distributed systems: apply offset to UTC time
class OffsetStabilizer final : public TimestampStabilizerBase<OffsetStabilizer> {
public:
    // Construct with time offset in milliseconds
    explicit OffsetStabilizer(int64_t offset_ms)
        : offset_ms_(offset_ms) {}

    // Get UTC timestamp with offset applied
    [[nodiscard]] std::int64_t stabilize() const {
        return current_utc_epoch_millis() + offset_ms_;
    }

private:
    int64_t offset_ms_;
};

// -----------------------------------------------------------------------------
// Additional Type Traits (for backward compatibility)
// -----------------------------------------------------------------------------

// Alias for the trait we already defined above
template <typename T>
using is_timestamp_stabilizer_t = is_timestamp_stabilizer<T>;

}  // namespace logging_system::D_Preparation