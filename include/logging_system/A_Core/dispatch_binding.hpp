#pragma once

#include <type_traits>

namespace logging_system::A_Core {

template <
    typename TDispatchExecutor,
    typename TQueuePolicy,
    typename TFailurePolicy,
    typename TAdapterEmission>
struct DispatchBinding final {
    using DispatchExecutor = TDispatchExecutor;
    using QueuePolicy = TQueuePolicy;
    using FailurePolicy = TFailurePolicy;
    using AdapterEmission = TAdapterEmission;

    static constexpr bool is_dispatch_binding = true;
};

template <typename T>
inline constexpr bool is_dispatch_binding_v =
    std::remove_cv_t<std::remove_reference_t<T>>::is_dispatch_binding;

}  // namespace logging_system::A_Core