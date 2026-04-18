#pragma once

#include <type_traits>

namespace logging_system::A_Core {

template <
    typename TWriterResolver,
    typename TDispatcherResolver,
    typename TReadonlyResolver>
struct ResolverBinding final {
    using WriterResolver = TWriterResolver;
    using DispatcherResolver = TDispatcherResolver;
    using ReadonlyResolver = TReadonlyResolver;

    static constexpr bool is_resolver_binding = true;
};

template <typename T>
inline constexpr bool is_resolver_binding_v =
    std::remove_cv_t<std::remove_reference_t<T>>::is_resolver_binding;

}  // namespace logging_system::A_Core