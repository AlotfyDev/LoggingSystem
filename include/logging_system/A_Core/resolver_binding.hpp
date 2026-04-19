#pragma once

#include "logging_system/A_Core/binding_tags.hpp"

namespace logging_system::A_Core {

template <
    typename TWriterResolver,
    typename TDispatcherResolver,
    typename TReadonlyResolver>
struct ResolverBinding final {
    using binding_tag = resolver_binding_tag;

    using WriterResolver = TWriterResolver;
    using DispatcherResolver = TDispatcherResolver;
    using ReadonlyResolver = TReadonlyResolver;
};

}  // namespace logging_system::A_Core