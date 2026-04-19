#pragma once

/*
------------------------------------------------------------------------------
K_Pipelines/fatal_pipeline_binding.hpp

Reference Version
-----------------
FATAL_SLICE_BASELINE_V1

Role in the architecture
------------------------
FatalPipelineBinding is the final compile-time assembly of the FATAL ingest/runtime
pipeline.

It answers one narrow question:
    "What are the four binding pillars that define the FATAL pipeline?"

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
- FatalPreparationBinding
- FatalResolverBinding
- FatalDispatchBinding
- FatalRepositoryRoute

into:
- FatalPipelineBinding

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
It gives the FATAL pipeline a compile-time identity without executing it.
------------------------------------------------------------------------------
*/

#include "logging_system/A_Core/pipeline_binding.hpp"

#include "logging_system/D_Preparation/fatal_preparation_binding.hpp"
#include "logging_system/E_Resolvers/fatal_resolver_binding.hpp"
#include "logging_system/F_Dispatch/fatal_dispatch_binding.hpp"
#include "logging_system/G_Routing/fatal_repository_route.hpp"

namespace logging_system::K_Pipelines {

using FatalPipelineBinding = logging_system::A_Core::PipelineBinding<
    logging_system::D_Preparation::FatalPreparationBinding,
    logging_system::E_Resolvers::FatalResolverBinding,
    logging_system::F_Dispatch::FatalDispatchBinding,
    logging_system::G_Routing::FatalRepositoryRoute>;

}  // namespace logging_system::K_Pipelines