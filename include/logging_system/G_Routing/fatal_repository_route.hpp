#pragma once

/*
------------------------------------------------------------------------------
G_Routing/fatal_repository_route.hpp

Reference Version
-----------------
FATAL_SLICE_BASELINE_V1

Role in the architecture
------------------------
FatalRepositoryRoute is the minimal per-pipeline repository/route specialization
for the FATAL pipeline.

It answers one narrow question:
    "When a record belongs to the FATAL pipeline, what repository/route identity
     should the pipeline bind to by default?"

Why this file exists in this batch
----------------------------------
The binding family becomes meaningful only when repository/route ownership is
made explicit per pipeline. Before this file exists, FatalPipelineBinding would
still be missing one of its four core pillars:
- preparation binding
- resolver binding
- dispatch binding
- repository route

Current minimal scope
---------------------
This file currently provides:
- an explicit route key for the FATAL pipeline
- a simple route type carrying:
  - level_key
  - route_key
- a static default route factory

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- repository/backend hints
- level-to-storage family specialization
- route-context-aware resolution
- tenant-aware route shaping
- per-pipeline repository policy attachments
- capability/profile-aware repository target enrichment

What should NOT live here
-------------------------
This file must NOT:
- own storage
- implement resolver logic
- execute dispatch
- access adapters
- mutate state
- become a registry or catalog

Design rule
-----------
This file expresses per-pipeline route ownership only.
It is a route identity/policy artifact, not a storage implementation.
------------------------------------------------------------------------------
*/

#include <string>
#include <utility>

namespace logging_system::G_Routing {

struct FatalRepositoryRoute final {
    std::string level_key{"FATAL"};
    std::string route_key{"fatal_repository"};

    FatalRepositoryRoute() = default;

    FatalRepositoryRoute(std::string level_key_in, std::string route_key_in)
        : level_key(std::move(level_key_in)),
          route_key(std::move(route_key_in)) {}

    static FatalRepositoryRoute make_default() {
        return FatalRepositoryRoute{"FATAL", "fatal_repository"};
    }
};

}  // namespace logging_system::G_Routing