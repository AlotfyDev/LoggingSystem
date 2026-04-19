#pragma once

/*
------------------------------------------------------------------------------
G_Routing/route_context.hpp

Role in the architecture
------------------------
RouteContext is the shared base context vocabulary for route-aware shaping.

It answers one narrow question:
    "What is the minimal shared context object that may accompany repository
     route shaping without forcing routing logic to inspect arbitrary payload
     internals?"

Why this file exists in S1
--------------------------
As the pipeline grows, routing and resolver layers may need a small shared
context carrier that expresses route-relevant metadata without hard-wiring:
- payload schema details
- adapter choices
- governance concerns
- state ownership

This file provides that minimal shared context vocabulary early, so later
pipeline-specific route/context enrichments can extend from something stable.

Current minimal scope
---------------------
This file currently provides:
- a RouteContext struct containing:
  - level_key
  - route_key_hint
  - pipeline_key

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- tenant/namespace hints
- repository profile hints
- capability family hints
- correlation identifiers
- shard/group hints
- route-intent flags
- pipeline-local binding annotations

What should NOT live here
-------------------------
This file must NOT:
- inspect payload content deeply
- execute route resolution
- own state
- own dispatch execution
- become a query/filter object
- become governance/configuration storage

Design rule
-----------
This file is shared route-context vocabulary only.
It carries route-relevant hints, not routing behavior.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>
#include <utility>

namespace logging_system::G_Routing {

struct RouteContext final {
    std::string level_key{};
    std::optional<std::string> route_key_hint{std::nullopt};
    std::optional<std::string> pipeline_key{std::nullopt};

    RouteContext() = default;

    RouteContext(
        std::string level_key_in,
        std::optional<std::string> route_key_hint_in = std::nullopt,
        std::optional<std::string> pipeline_key_in = std::nullopt)
        : level_key(std::move(level_key_in)),
          route_key_hint(std::move(route_key_hint_in)),
          pipeline_key(std::move(pipeline_key_in)) {}
};

}  // namespace logging_system::G_Routing