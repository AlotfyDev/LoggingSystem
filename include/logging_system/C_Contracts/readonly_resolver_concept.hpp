#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/readonly_resolver_concept.hpp

Role in the architecture
------------------------
This file provides the architectural "concept" for readonly-resolver-like types.

It answers narrow questions such as:
    "Can this type resolve a level-scoped readonly descriptor?"
    "Can this type resolve a module inspection descriptor?"

Why this file exists in S1
--------------------------
The resolver family should be contractually complete:
- writer
- dispatcher
- readonly

Before readonly-facing layers grow on top of it, the architecture needs a
compile-time contract vocabulary for:
- resolve_level(...)
- resolve_inspection(...)

Current minimal scope
---------------------
This file currently checks whether a type provides:
- `resolve_level(module, level_key)`
- `resolve_inspection(module)`

The exact descriptor types are intentionally not overconstrained yet.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- stronger descriptor-shape checks
- query/projection-oriented readonly traits
- binding-level integration assertions
- pipeline-local readonly-resolver helpers

What should NOT live here
-------------------------
This file must NOT:
- execute queries
- render output
- mutate state
- inspect payload semantics
- become a runtime validator

Design rule
-----------
This file provides compile-time shape validation only.
It checks whether a type exposes readonly-resolver-like operations.
------------------------------------------------------------------------------
*/

#include <string>
#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename TResolver, typename TModule, typename = void>
struct is_readonly_resolver : std::false_type {};

template <typename TResolver, typename TModule>
struct is_readonly_resolver<
    TResolver,
    TModule,
    std::void_t<
        decltype(remove_cvref_t<TResolver>::resolve_level(
            std::declval<const TModule&>(),
            std::declval<const std::string&>())),
        decltype(remove_cvref_t<TResolver>::resolve_inspection(
            std::declval<const TModule&>()))>>
    : std::true_type {};

template <typename TResolver, typename TModule>
inline constexpr bool is_readonly_resolver_v =
    is_readonly_resolver<TResolver, TModule>::value;

template <typename TResolver, typename TModule>
struct readonly_resolver_concept
    : std::bool_constant<is_readonly_resolver_v<TResolver, TModule>> {};

template <typename TResolver, typename TModule>
inline constexpr bool readonly_resolver_concept_v =
    readonly_resolver_concept<TResolver, TModule>::value;

template <typename TResolver, typename TModule>
using require_readonly_resolver_t =
    std::enable_if_t<readonly_resolver_concept_v<TResolver, TModule>, int>;

}  // namespace logging_system::C_Contracts