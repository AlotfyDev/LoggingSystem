#pragma once

/*
------------------------------------------------------------------------------
N_System/pipeline_catalog.hpp

Reference Version
-----------------
SYSTEM_GOVERNANCE_P1_BASELINE_V1

Role in the architecture
------------------------
PipelineCatalog is the compile-time catalog of the level-specialized pipelines
available in the logging system.

It answers narrow questions such as:
    "Which finalized per-level pipeline bindings constitute the pipeline pack
     of this system build?"
    "How does the system root refer to the full set of available pipelines
     without collapsing them into a generic shared level-dispatch function?"

Why this file exists in this stage
----------------------------------
The Python architecture clearly exposes:
- dedicated level APIs
- specialized pipeline intent
- separated runtime/governance surfaces

But that intent remains partly distributed across modules rather than being
expressed as one explicit pipeline-pack artifact.

This file exists to make that intent explicit in C++:
- INFO
- DEBUG
- WARN
- ERROR
- FATAL
- TRACE

are represented as a compile-time catalog of distinct pipeline identities.

Current minimal scope
---------------------
This file currently provides:
- `PipelineCatalog`
- explicit aliases for:
  - `Info`
  - `Debug`
  - `Warn`
  - `Error`
  - `Fatal`
  - `Trace`

This is intentionally a compile-time catalog only.
It does not perform runtime lookup or string-based dispatch.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- pipeline family traits
- compile-time iteration helpers
- pipeline-tag mapping helpers
- optional pipeline pack validation
- compatibility checks against active profiles/configuration

What should NOT live here
-------------------------
This file must NOT:
- perform runtime level routing
- become a registry of mutable pipeline instances
- own state
- own governance data
- become a replacement for dedicated level APIs

Design rule
-----------
This file is a compile-time pipeline-pack declaration only.
Its job is to make the system's per-level pipeline family explicit.
------------------------------------------------------------------------------
*/

#include "logging_system/K_Pipelines/debug_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/error_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/fatal_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/info_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/trace_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/warn_pipeline_binding.hpp"

namespace logging_system::N_System {

struct PipelineCatalog final {
    using Info = logging_system::K_Pipelines::InfoPipelineBinding;
    using Debug = logging_system::K_Pipelines::DebugPipelineBinding;
    using Warn = logging_system::K_Pipelines::WarnPipelineBinding;
    using Error = logging_system::K_Pipelines::ErrorPipelineBinding;
    using Fatal = logging_system::K_Pipelines::FatalPipelineBinding;
    using Trace = logging_system::K_Pipelines::TracePipelineBinding;
};

}  // namespace logging_system::N_System