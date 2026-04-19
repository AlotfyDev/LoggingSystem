#pragma once

#include "logging_system/A_Core/dispatch_binding.hpp"

#include "logging_system/F_Dispatch/default_adapter_emission.hpp"
#include "logging_system/F_Dispatch/dispatch_failure_policy.hpp"
#include "logging_system/F_Dispatch/queue_policy.hpp"
#include "logging_system/F_Dispatch/thread_pool_dispatch_executor.hpp"

namespace logging_system::F_Dispatch {

using InfoDispatchBinding = logging_system::A_Core::DispatchBinding<
    ThreadPoolDispatchExecutor,
    QueuePolicy,
    DispatchFailurePolicy,
    DefaultAdapterEmission>;

}  // namespace logging_system::F_Dispatch