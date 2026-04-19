#pragma once

/*
------------------------------------------------------------------------------
G_Routing/repository_route.hpp

Role in the architecture
------------------------
RepositoryRoute is the shared base routing vocabulary for repository/route
ownership across pipelines.

It answers one narrow question:
    "What is the minimal shared shape of a pipeline route toward its target
     repository/storage family?"

Why this file exists in S1
--------------------------
Per-pipeline route specializations such as InfoRepositoryRoute should not float
without a shared vocabulary beneath them.

This file provides that shared vocabulary, so pipeline-specific route files can
specialize over a stable base concept instead of inventing their own route
shape independently.

Current minimal scope
---------------------
This file currently provides:
- a shared RepositoryRoute struct containing:
  - level_key
  - route_key

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- repository family hints
- backend/profile identifiers
- tenant or namespace hints
- capability/profile attachments
- route-local policy hints
- correlation or shard-routing annotations

What should NOT live here
-------------------------
This file must NOT:
- resolve routes dynamically
- access state
- access adapters
- perform dispatch
- perform repository operations
- become a registry/catalog

Design rule
-----------
This file is shared route vocabulary only.
It defines the base route shape that pipeline-specific route specializations may
reuse or mirror.
------------------------------------------------------------------------------
*/

#include <string>
#include <utility>

namespace logging_system::G_Routing {

struct RepositoryRoute final {
    std::string level_key{};
    std::string route_key{};

    RepositoryRoute() = default;

    RepositoryRoute(std::string level_key_in, std::string route_key_in)
        : level_key(std::move(level_key_in)),
          route_key(std::move(route_key_in)) {}
};

}  // namespace logging_system::G_Routing