#pragma once

/*
------------------------------------------------------------------------------
E_Resolvers/info_resolver_binding.hpp

Role in the architecture
------------------------
InfoResolverBinding is the INFO-pipeline specialization of the generic resolver
binding family.

It answers one narrow question:
    "Which resolver implementations constitute the resolver stack for the INFO
     pipeline right now?"

Why this file exists in this batch
----------------------------------
After default reusable resolver implementations exist, each pipeline needs an
explicit binding alias that states:
- which writer resolver it uses
- which dispatcher resolver it uses
- which readonly resolver it uses

This keeps specialization explicit at the binding level, even when current
implementations are still default/shared.

Current minimal scope
---------------------
This file currently binds:
- DefaultWriterResolver
- DefaultDispatcherResolver
- DefaultReadonlyResolver

into:
- InfoResolverBinding

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- replacing one or more default resolvers with INFO-specific specializations
- route-aware resolver wrappers
- policy-bound resolver implementations
- stronger compile-time validation against resolver contracts

What should NOT live here
-------------------------
This file must NOT:
- implement resolver logic itself
- own routing/state/adapters
- become a pipeline runner
- perform execution
- hold mutable state

Design rule
-----------
This file is a binding alias only.
Its job is to make pipeline-local resolver composition explicit.
------------------------------------------------------------------------------
*/

#include "logging_system/A_Core/resolver_binding.hpp"

#include "logging_system/E_Resolvers/default_dispatcher_resolver.hpp"
#include "logging_system/E_Resolvers/default_readonly_resolver.hpp"
#include "logging_system/E_Resolvers/default_writer_resolver.hpp"

namespace logging_system::E_Resolvers {

using InfoResolverBinding = logging_system::A_Core::ResolverBinding<
    DefaultWriterResolver,
    DefaultDispatcherResolver,
    DefaultReadonlyResolver>;

}  // namespace logging_system::E_Resolvers