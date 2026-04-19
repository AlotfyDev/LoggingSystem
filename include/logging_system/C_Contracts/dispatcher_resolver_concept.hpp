#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/dispatcher_resolver_concept.hpp

Role in the architecture
------------------------
This file provides the architectural "concept" for dispatcher-resolver-like
types.

It answers one narrow question:
    "Can this type act as a dispatch-side resolver in the pipeline architecture?"

Why this file exists in S1
--------------------------
Dispatch resolver implementations already exist at the default level.
Before dispatch bindings and pipeline runners rely on them more deeply, the
architecture needs a compile-time contract vocabulary for:
- resolve_dispatch_candidate(...)
- resolve_dispatch_receiver_binding(...)

Current minimal scope
---------------------
This file currently checks whether a type provides:
- `resolve_dispatch_candidate(module, level_key, record)`
- `resolve_dispatch_receiver_binding(candidate)`

The exact descriptor types are intentionally not overconstrained yet.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- stronger candidate/receiver descriptor validation
- route-aware descriptor trait checks
- binding-level integration assertions
- pipeline-local dispatch-resolver trait helpers

What should NOT live here
-------------------------
This file must NOT:
- implement dispatch resolution
- execute dispatch
- own adapters
- mutate state
- become a runtime validator

Design rule
-----------
This file provides compile-time shape validation only.
It checks whether a type exposes dispatcher-resolver-like operations.
------------------------------------------------------------------------------
*/

#include <string>
#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename TResolver, typename TModule, typename TRecord, typename = void>
struct is_dispatcher_resolver : std::false_type {};

template <typename TResolver, typename TModule, typename TRecord>
struct is_dispatcher_resolver<
    TResolver,
    TModule,
    TRecord,
    std::void_t<
        decltype(remove_cvref_t<TResolver>::resolve_dispatch_candidate(
            std::declval<const TModule&>(),
            std::declval<const std::string&>(),
            std::declval<const TRecord&>())),
        decltype(remove_cvref_t<TResolver>::resolve_dispatch_receiver_binding(
            remove_cvref_t<TResolver>::resolve_dispatch_candidate(
                std::declval<const TModule&>(),
                std::declval<const std::string&>(),
                std::declval<const TRecord&>())))>>
    : std::true_type {};

template <typename TResolver, typename TModule, typename TRecord>
inline constexpr bool is_dispatcher_resolver_v =
    is_dispatcher_resolver<TResolver, TModule, TRecord>::value;

template <typename TResolver, typename TModule, typename TRecord>
struct dispatcher_resolver_concept
    : std::bool_constant<is_dispatcher_resolver_v<TResolver, TModule, TRecord>> {};

template <typename TResolver, typename TModule, typename TRecord>
inline constexpr bool dispatcher_resolver_concept_v =
    dispatcher_resolver_concept<TResolver, TModule, TRecord>::value;

template <typename TResolver, typename TModule, typename TRecord>
using require_dispatcher_resolver_t =
    std::enable_if_t<dispatcher_resolver_concept_v<TResolver, TModule, TRecord>, int>;

}  // namespace logging_system::C_Contracts