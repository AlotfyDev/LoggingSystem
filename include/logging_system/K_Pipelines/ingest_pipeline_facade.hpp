#pragma once

/*
------------------------------------------------------------------------------
K_Pipelines/ingest_pipeline_facade.hpp

Role in the architecture
------------------------
IngestPipelineFacade is the generic compile-time façade for one pipeline binding.

It answers one narrow question:
    "How should generic code talk to a concrete pipeline binding as a unified
     ingest/runtime pipeline shape?"

Why this file exists in this batch
----------------------------------
After a pipeline binding exists (such as InfoPipelineBinding), the system needs
a generic façade that exposes its four binding pillars in a normalized way:
- preparation binding
- resolver binding
- dispatch binding
- repository route

Without this file, pipeline-specific code would have to reach directly into
PipelineBinding internals everywhere, which would make later pipeline runner
and thin level APIs unnecessarily coupled to the raw binding structure.

Current minimal scope
---------------------
This file currently provides:
- a generic `IngestPipelineFacade<TPipelineBinding>`
- normalized access to:
  - Preparation
  - Resolver
  - Dispatch
  - RepositoryRoute
- a `make_default_route()` helper

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- pipeline-local contract validation
- pipeline-local helper factories
- pipeline-local static traits
- richer accessors for component packs
- compile-time checks against resolver/dispatch/preparation concepts

What should NOT live here
-------------------------
This file must NOT:
- execute the pipeline
- mutate state
- perform adapter emission
- perform schema validation itself
- become a system composition root

Design rule
-----------
This file is a generic façade over one pipeline binding.
It exposes pipeline composition shape, not pipeline execution behavior.
------------------------------------------------------------------------------
*/

#include "logging_system/A_Core/pipeline_binding.hpp"

namespace logging_system::K_Pipelines {

template <typename TPipelineBinding>
struct IngestPipelineFacade final {
    using PipelineBinding = TPipelineBinding;

    using Preparation = typename PipelineBinding::Preparation;
    using Resolver = typename PipelineBinding::Resolver;
    using Dispatch = typename PipelineBinding::Dispatch;
    using RepositoryRoute = typename PipelineBinding::RepositoryRoute;

    static RepositoryRoute make_default_route() {
        return RepositoryRoute::make_default();
    }
};

}  // namespace logging_system::K_Pipelines