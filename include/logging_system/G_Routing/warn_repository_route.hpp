#pragma once

/*
------------------------------------------------------------------------------
G_Routing/warn_repository_route.hpp

Reference Version
-----------------
WARN_SLICE_BASELINE_V1

Role in the architecture
------------------------
WarnRepositoryRoute is the minimal per-pipeline repository/route specialization
for the WARN pipeline.

It answers one narrow question:
    "When a record belongs to the WARN pipeline, what repository/route identity
     should the pipeline bind to by default?"

Why this file exists in this batch
----------------------------------
The binding family becomes meaningful only when repository/route ownership is
made explicit per pipeline. Before this file exists, WarnPipelineBinding would
still be missing one of its four core pillars:
- preparation binding
- resolver binding
- dispatch binding
- repository route

Current minimal scope
---------------------
This file currently provides:
- an explicit route key for the WARN pipeline
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

struct WarnRepositoryRoute final {
    std::string level_key{"WARN"};
    std::string route_key{"warn_repository"};

    WarnRepositoryRoute() = default;

    WarnRepositoryRoute(std::string level_key_in, std::string route_key_in)
        : level_key(std::move(level_key_in)),
          route_key(std::move(route_key_in)) {}

    static WarnRepositoryRoute make_default() {
        return WarnRepositoryRoute{"WARN", "warn_repository"};
    }
};

}  // namespace logging_system::G_Routing