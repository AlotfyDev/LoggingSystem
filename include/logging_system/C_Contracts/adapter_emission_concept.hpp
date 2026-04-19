#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/adapter_emission_concept.hpp

Role in the architecture
------------------------
This file provides the architectural "concept" for adapter-emission-like bridge
types.

It answers one narrow question:
    "Can this type act as the emission bridge between a dispatch context and an
     adapter-like object?"

Why this file exists in S1
--------------------------
The dispatch layer now includes:
- dispatch context
- queue policy
- failure policy
- default adapter emission
- default executor

Before higher layers depend on them, the architecture needs a compile-time
contract vocabulary that can validate emission-bridge-like types consistently.

Current minimal scope
---------------------
This file currently checks whether a type provides:
- `emit(adapter, context)`

The exact result type is intentionally not overconstrained yet.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer result-type checks
- capability-aware emission traits
- binding-level integration assertions
- adapter family specialization helpers

What should NOT live here
-------------------------
This file must NOT:
- implement emission
- choose adapters
- resolve routes
- execute retries
- mutate state
- become a runtime validator

Design rule
-----------
This file provides compile-time shape validation only.
It checks whether a type exposes adapter-emission-like behavior.
------------------------------------------------------------------------------
*/

#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename TEmission, typename TAdapter, typename TDispatchContext, typename = void>
struct is_adapter_emission : std::false_type {};

template <typename TEmission, typename TAdapter, typename TDispatchContext>
struct is_adapter_emission<
    TEmission,
    TAdapter,
    TDispatchContext,
    std::void_t<
        decltype(remove_cvref_t<TEmission>::emit(
            std::declval<TAdapter&>(),
            std::declval<const TDispatchContext&>()))>>
    : std::true_type {};

template <typename TEmission, typename TAdapter, typename TDispatchContext>
inline constexpr bool is_adapter_emission_v =
    is_adapter_emission<TEmission, TAdapter, TDispatchContext>::value;

template <typename TEmission, typename TAdapter, typename TDispatchContext>
struct adapter_emission_concept
    : std::bool_constant<is_adapter_emission_v<TEmission, TAdapter, TDispatchContext>> {};

template <typename TEmission, typename TAdapter, typename TDispatchContext>
inline constexpr bool adapter_emission_concept_v =
    adapter_emission_concept<TEmission, TAdapter, TDispatchContext>::value;

template <typename TEmission, typename TAdapter, typename TDispatchContext>
using require_adapter_emission_t =
    std::enable_if_t<adapter_emission_concept_v<TEmission, TAdapter, TDispatchContext>, int>;

}  // namespace logging_system::C_Contracts