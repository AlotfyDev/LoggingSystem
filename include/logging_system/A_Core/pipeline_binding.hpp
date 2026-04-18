#pragma once

#include <type_traits>

#include "logging_system/A_Core/dispatch_binding.hpp"
#include "logging_system/A_Core/preparation_binding.hpp"
#include "logging_system/A_Core/resolver_binding.hpp"

namespace logging_system::A_Core {

template <
    typename TPreparationBinding,
    typename TResolverBinding,
    typename TDispatchBinding,
    typename TRepositoryRoute>
struct PipelineBinding final {
    static_assert(
        is_preparation_binding_v<TPreparationBinding>,
        "TPreparationBinding must be a logging_system::A_Core::PreparationBinding");

    static_assert(
        is_resolver_binding_v<TResolverBinding>,
        "TResolverBinding must be a logging_system::A_Core::ResolverBinding");

    static_assert(
        is_dispatch_binding_v<TDispatchBinding>,
        "TDispatchBinding must be a logging_system::A_Core::DispatchBinding");

    using Preparation = TPreparationBinding;
    using Resolver = TResolverBinding;
    using Dispatch = TDispatchBinding;
    using RepositoryRoute = TRepositoryRoute;

    static constexpr bool is_pipeline_binding = true;
};

template <typename T>
inline constexpr bool is_pipeline_binding_v =
    std::remove_cv_t<std::remove_reference_t<T>>::is_pipeline_binding;

}  // namespace logging_system::A_Core