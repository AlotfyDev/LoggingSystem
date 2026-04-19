#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/repository_route_concept.hpp

Role in the architecture
------------------------
This file provides the architectural "concept" for repository-route-like types.

Important note
--------------
"Concept" here is an architectural contract term.
Because the target is C++17, this file implements the contract through traits
and detection idioms, not through C++20 language-level concepts.

It answers one narrow question:
    "Does this type look like a repository route according to the shared route
     vocabulary of the system?"

Why this file exists in S1
--------------------------
The INFO slice now has:
- shared route vocabulary
- route context vocabulary
- per-pipeline route specialization

Before higher layers rely on them, the architecture needs a compile-time
contract vocabulary that can validate route-like types consistently.

Current minimal scope
---------------------
This file currently checks whether a type exposes:
- `level_key`
- `route_key`

This intentionally mirrors the current shared route base vocabulary and does not
assume richer repository metadata yet.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- validation of richer route metadata
- route factory detection
- route-context-aware detection
- route traits for pipeline-local specialization
- integration with binding-level static assertions

What should NOT live here
-------------------------
This file must NOT:
- resolve routes
- own route instances
- access state
- execute dispatch
- become a runtime validator

Design rule
-----------
This file provides a compile-time contract vocabulary only.
It validates type shape, not runtime behavior.
------------------------------------------------------------------------------
*/

#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename, typename = void>
struct is_repository_route : std::false_type {};

template <typename T>
struct is_repository_route<
    T,
    std::void_t<
        decltype(std::declval<remove_cvref_t<T>&>().level_key),
        decltype(std::declval<remove_cvref_t<T>&>().route_key)>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_repository_route_v = is_repository_route<T>::value;

template <typename T>
struct repository_route_concept : std::bool_constant<is_repository_route_v<T>> {};

template <typename T>
inline constexpr bool repository_route_concept_v =
    repository_route_concept<T>::value;

template <typename T>
using require_repository_route_t =
    std::enable_if_t<repository_route_concept_v<T>, int>;

}  // namespace logging_system::C_Contracts