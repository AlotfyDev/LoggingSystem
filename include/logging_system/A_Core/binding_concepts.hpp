#pragma once

#include <type_traits>

#include "logging_system/A_Core/binding_traits.hpp"

namespace logging_system::A_Core {

// C++17 note:
// "Concepts" here are architectural contract checks implemented via traits,
// not C++20 language-level concepts.

template <typename T>
struct preparation_binding_concept : std::bool_constant<is_preparation_binding_v<T>> {};

template <typename T>
inline constexpr bool preparation_binding_concept_v =
    preparation_binding_concept<T>::value;

template <typename T>
struct resolver_binding_concept : std::bool_constant<is_resolver_binding_v<T>> {};

template <typename T>
inline constexpr bool resolver_binding_concept_v =
    resolver_binding_concept<T>::value;

template <typename T>
struct dispatch_binding_concept : std::bool_constant<is_dispatch_binding_v<T>> {};

template <typename T>
inline constexpr bool dispatch_binding_concept_v =
    dispatch_binding_concept<T>::value;

template <typename T>
struct pipeline_binding_concept : std::bool_constant<is_pipeline_binding_v<T>> {};

template <typename T>
inline constexpr bool pipeline_binding_concept_v =
    pipeline_binding_concept<T>::value;

template <typename T>
using require_preparation_binding_t =
    std::enable_if_t<preparation_binding_concept_v<T>, int>;

template <typename T>
using require_resolver_binding_t =
    std::enable_if_t<resolver_binding_concept_v<T>, int>;

template <typename T>
using require_dispatch_binding_t =
    std::enable_if_t<dispatch_binding_concept_v<T>, int>;

template <typename T>
using require_pipeline_binding_t =
    std::enable_if_t<pipeline_binding_concept_v<T>, int>;

}  // namespace logging_system::A_Core