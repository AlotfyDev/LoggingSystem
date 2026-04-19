#pragma once

#include "logging_system/A_Core/pipeline_binding.hpp"

#include "logging_system/D_Preparation/info_preparation_binding.hpp"
#include "logging_system/E_Resolvers/info_resolver_binding.hpp"
#include "logging_system/F_Dispatch/info_dispatch_binding.hpp"
#include "logging_system/G_Routing/info_repository_route.hpp"

namespace logging_system::K_Pipelines {

using InfoPipelineBinding = logging_system::A_Core::PipelineBinding<
    logging_system::D_Preparation::InfoPreparationBinding,
    logging_system::E_Resolvers::InfoResolverBinding,
    logging_system::F_Dispatch::InfoDispatchBinding,
    logging_system::G_Routing::InfoRepositoryRoute>;

}  // namespace logging_system::K_Pipelines