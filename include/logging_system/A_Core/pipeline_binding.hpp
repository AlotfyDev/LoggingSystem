#pragma once

#include "logging_system/A_Core/binding_concepts.hpp"
#include "logging_system/A_Core/binding_tags.hpp"

namespace logging_system::A_Core {

template <
    typename TPreparationBinding,
    typename TResolverBinding,
    typename TDispatchBinding,
    typename TRepositoryRoute>
struct PipelineBinding final {
    using binding_tag = pipeline_binding_tag;

    static_assert(
        preparation_binding_concept_v<TPreparationBinding>,
        "TPreparationBinding must satisfy the architectural preparation binding concept");

    static_assert(
        resolver_binding_concept_v<TResolverBinding>,
        "TResolverBinding must satisfy the architectural resolver binding concept");

    static_assert(
        dispatch_binding_concept_v<TDispatchBinding>,
        "TDispatchBinding must satisfy the architectural dispatch binding concept");

    using Preparation = TPreparationBinding;
    using Resolver = TResolverBinding;
    using Dispatch = TDispatchBinding;
    using RepositoryRoute = TRepositoryRoute;
};

}  // namespace logging_system::A_Core