#pragma once

#include "logging_system/A_Core/resolver_binding.hpp"

#include "logging_system/E_Resolvers/default_dispatcher_resolver.hpp"
#include "logging_system/E_Resolvers/default_readonly_resolver.hpp"
#include "logging_system/E_Resolvers/default_writer_resolver.hpp"

namespace logging_system::E_Resolvers {

using InfoResolverBinding = logging_system::A_Core::ResolverBinding<
    DefaultWriterResolver,
    DefaultDispatcherResolver,
    DefaultReadonlyResolver>;

}  // namespace logging_system::E_Resolvers