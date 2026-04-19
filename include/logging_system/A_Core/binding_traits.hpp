#pragma once

#include <type_traits>

namespace logging_system::A_Core {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename, typename = void>
struct has_binding_tag : std::false_type {};

template <typename T>
struct has_binding_tag<T, std::void_t<typename remove_cvref_t<T>::binding_tag>> : std::true_type {};

template <typename T>
inline constexpr bool has_binding_tag_v = has_binding_tag<T>::value;

template <typename T, typename TTag, typename = void>
struct is_binding_of_tag : std::false_type {};

template <typename T, typename TTag>
struct is_binding_of_tag<
    T,
    TTag,
    std::void_t<typename remove_cvref_t<T>::binding_tag>>
    : std::is_same<typename remove_cvref_t<T>::binding_tag, TTag> {};

template <typename T, typename TTag>
inline constexpr bool is_binding_of_tag_v = is_binding_of_tag<T, TTag>::value;

template <typename T, typename = void>
struct is_preparation_binding : std::false_type {};

template <typename T>
struct is_preparation_binding<
    T,
    std::enable_if_t<is_binding_of_tag_v<T, preparation_binding_tag>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_preparation_binding_v = is_preparation_binding<T>::value;

template <typename T, typename = void>
struct is_resolver_binding : std::false_type {};

template <typename T>
struct is_resolver_binding<
    T,
    std::enable_if_t<is_binding_of_tag_v<T, resolver_binding_tag>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_resolver_binding_v = is_resolver_binding<T>::value;

template <typename T, typename = void>
struct is_dispatch_binding : std::false_type {};

template <typename T>
struct is_dispatch_binding<
    T,
    std::enable_if_t<is_binding_of_tag_v<T, dispatch_binding_tag>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_dispatch_binding_v = is_dispatch_binding<T>::value;

template <typename T, typename = void>
struct is_pipeline_binding : std::false_type {};

template <typename T>
struct is_pipeline_binding<
    T,
    std::enable_if_t<is_binding_of_tag_v<T, pipeline_binding_tag>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_pipeline_binding_v = is_pipeline_binding<T>::value;

}  // namespace logging_system::A_Core