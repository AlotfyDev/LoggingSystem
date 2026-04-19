#pragma once

/*
------------------------------------------------------------------------------
K_Pipelines/error_pipeline_binding.hpp

Reference Version
-----------------
ERROR_SLICE_BASELINE_V1

Role in the architecture
------------------------
ErrorPipelineBinding is the final compile-time assembly of the ERROR ingest/runtime
pipeline.

It answers one narrow question:
    "What are the four binding pillars that define the ERROR pipeline?"

Those four pillars are:
- preparation binding
- resolver binding
- dispatch binding
- repository route

Why this file exists in this batch
----------------------------------
This file is the next complete per-pipeline assembly point in the system.
After:
- preparation defaults/binding
- resolver defaults/binding
- dispatch defaults/binding
- repository route specialization

the architecture needs one place where these are assembled into a single
pipeline-local compile-time identity.

Current minimal scope
---------------------
This file currently assembles:
- ErrorPreparationBinding
- ErrorResolverBinding
- ErrorDispatchBinding
- ErrorRepositoryRoute

into:
- ErrorPipelineBinding

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- stronger compile-time validation against pipeline contracts
- pipeline-local helper aliases
- pipeline-local static traits
- optional facade/runner integration hooks
- documentation hooks for pipeline packs and system composition

What should NOT live here
-------------------------
This file must NOT:
- implement pipeline execution
- implement dispatch
- implement state mutation
- implement resolver logic
- become a system root

Design rule
-----------
This file is a final binding assembly artifact only.
It gives the ERROR pipeline a compile-time identity without executing it.
------------------------------------------------------------------------------
*/

#include "logging_system/A_Core/pipeline_binding.hpp"

#include "logging_system/D_Preparation/error_preparation_binding.hpp"
#include "logging_system/E_Resolvers/error_resolver_binding.hpp"
#include "logging_system/F_Dispatch/error_dispatch_binding.hpp"
#include "logging_system/G_Routing/error_repository_route.hpp"

namespace logging_system::K_Pipelines {

using ErrorPipelineBinding = logging_system::A_Core::PipelineBinding<
    logging_system::D_Preparation::ErrorPreparationBinding,
    logging_system::E_Resolvers::ErrorResolverBinding,
    logging_system::F_Dispatch::ErrorDispatchBinding,
    logging_system::G_Routing::ErrorRepositoryRoute>;

}  // namespace logging_system::K_Pipelines