#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/writer_resolver_concept.hpp

Role in the architecture
------------------------
This file provides the architectural "concept" for writer-resolver-like types.

It answers one narrow question:
    "Can this type act as a writer-side resolver in the pipeline architecture?"

Why this file exists in S1
--------------------------
Writer resolver implementations already exist at the default level.
Before runner/facade/system layers depend on them more heavily, the
architecture needs a compile-time contract vocabulary for:
- resolve_write_target(...)
- build_handoff_event(...)

Current minimal scope
---------------------
This file currently checks whether a type provides:
- `resolve_write_target(module, level_key, record)`
- `build_handoff_event(target)`

The exact return types are intentionally not overconstrained yet.
The goal is minimal compile-time shape validation.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- return-type validation
- stronger descriptor-shape checks
- pipeline-local resolver trait helpers
- integration with binding-level assertions

What should NOT live here
-------------------------
This file must NOT:
- implement resolver behavior
- access state directly
- execute writes
- become a runtime validator

Design rule
-----------
This file provides compile-time shape validation only.
It checks whether a type exposes writer-resolver-like operations.
------------------------------------------------------------------------------
*/

#include <string>
#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename TResolver, typename TModule, typename TRecord, typename = void>
struct is_writer_resolver : std::false_type {};

template <typename TResolver, typename TModule, typename TRecord>
struct is_writer_resolver<
    TResolver,
    TModule,
    TRecord,
    std::void_t<
        decltype(remove_cvref_t<TResolver>::resolve_write_target(
            std::declval<const TModule&>(),
            std::declval<const std::string&>(),
            std::declval<const TRecord&>())),
        decltype(remove_cvref_t<TResolver>::build_handoff_event(
            remove_cvref_t<TResolver>::resolve_write_target(
                std::declval<const TModule&>(),
                std::declval<const std::string&>(),
                std::declval<const TRecord&>())))>>
    : std::true_type {};

template <typename TResolver, typename TModule, typename TRecord>
inline constexpr bool is_writer_resolver_v =
    is_writer_resolver<TResolver, TModule, TRecord>::value;

template <typename TResolver, typename TModule, typename TRecord>
struct writer_resolver_concept
    : std::bool_constant<is_writer_resolver_v<TResolver, TModule, TRecord>> {};

template <typename TResolver, typename TModule, typename TRecord>
inline constexpr bool writer_resolver_concept_v =
    writer_resolver_concept<TResolver, TModule, TRecord>::value;

template <typename TResolver, typename TModule, typename TRecord>
using require_writer_resolver_t =
    std::enable_if_t<writer_resolver_concept_v<TResolver, TModule, TRecord>, int>;

}  // namespace logging_system::C_Contracts