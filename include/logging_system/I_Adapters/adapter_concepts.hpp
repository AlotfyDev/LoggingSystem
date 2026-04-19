#pragma once

/*
------------------------------------------------------------------------------
I_Adapters/adapter_concepts.hpp

Role in the architecture
------------------------
This file provides the shared compile-time contract vocabulary for adapter-like
types.

It answers one narrow question:
    "Does this type look like a logging-system adapter according to the minimal
     emission boundary of the current slice?"

Why this file exists in S2
--------------------------
The INFO slice now has:
- pipeline identity
- resolver defaults
- dispatch defaults
- route vocabulary
- dispatch/emission contracts

Before the slice becomes runnable, it needs a minimal adapter boundary that can
be:
- checked at compile time
- consumed by the default adapter-emission bridge
- exercised by the pipeline runner

Current minimal scope
---------------------
This file currently provides:
- a trait-based architectural contract for adapter-like types
- the contract checks whether a type exposes:
    emit_signal(record)

This intentionally matches the current default adapter-emission bridge:
- `DefaultAdapterEmission::emit(adapter, context)`
  forwards to:
- `adapter.emit_signal(context.record)`

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer adapter capability traits
- result-type validation
- previewer-capable adapter traits
- observability/otel-specific capability traits
- binding-level integration helpers
- adapter-family marker traits

What should NOT live here
-------------------------
This file must NOT:
- implement adapters
- choose adapters
- own registries
- execute dispatch
- become a runtime validator

Design rule
-----------
This file provides compile-time adapter contract vocabulary only.
It validates type shape, not runtime behavior.
------------------------------------------------------------------------------
*/

#include <type_traits>
#include <utility>

namespace logging_system::I_Adapters {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename TAdapter, typename TRecord, typename = void>
struct is_log_adapter : std::false_type {};

template <typename TAdapter, typename TRecord>
struct is_log_adapter<
    TAdapter,
    TRecord,
    std::void_t<
        decltype(std::declval<remove_cvref_t<TAdapter>&>().emit_signal(
            std::declval<const TRecord&>()))>>
    : std::true_type {};

template <typename TAdapter, typename TRecord>
inline constexpr bool is_log_adapter_v =
    is_log_adapter<TAdapter, TRecord>::value;

template <typename TAdapter, typename TRecord>
struct adapter_concept
    : std::bool_constant<is_log_adapter_v<TAdapter, TRecord>> {};

template <typename TAdapter, typename TRecord>
inline constexpr bool adapter_concept_v =
    adapter_concept<TAdapter, TRecord>::value;

template <typename TAdapter, typename TRecord>
using require_adapter_t =
    std::enable_if_t<adapter_concept_v<TAdapter, TRecord>, int>;

}  // namespace logging_system::I_Adapters