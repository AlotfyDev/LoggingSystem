#pragma once

#include "logging_system/A_Core/binding_tags.hpp"

namespace logging_system::A_Core {

template <
    typename TDispatchExecutor,
    typename TQueuePolicy,
    typename TFailurePolicy,
    typename TAdapterEmission>
struct DispatchBinding final {
    using binding_tag = dispatch_binding_tag;

    using DispatchExecutor = TDispatchExecutor;
    using QueuePolicy = TQueuePolicy;
    using FailurePolicy = TFailurePolicy;
    using AdapterEmission = TAdapterEmission;
};

}  // namespace logging_system::A_Core