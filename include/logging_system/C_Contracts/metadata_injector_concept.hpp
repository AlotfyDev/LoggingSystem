#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/metadata_injector_concept.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V2

Role in the architecture
------------------------
This file defines the compile-time contract vocabulary for metadata injectors
used by consuming-pipeline assemblers.

It answers narrow questions such as:
    "What does it mean for a preparation component to qualify as a metadata
     injector in this system?"
    "Which input and output expectations should an assembler rely on when it
     composes a metadata injector?"
    "How can the project preserve C++17 compatibility while still expressing
     architectural contracts in a concept-like form?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- metadata is a preparation concern
- metadata is not passed by the consumer on every API call
- metadata is assigned administratively and then made available to the
  assembler
- metadata/timestamp concerns should be vertically closed before duplicating
  level-API logic
- assembler composition should depend on explicit preparation contracts rather
  than informal convention
- injectors should support both metadata provision AND envelope injection for
  assembler composition

This means the project needs a metadata injector contract that:
- is C++17 compatible
- describes the expected metadata value type
- describes the expected public injector entry shape (provision + injection)
- supports composition into envelope assemblers
- can be checked at compile time by higher-level bindings if needed

Current minimal scope
---------------------
This file currently provides:
- `has_metadata_type<T>`
- `has_get_metadata_method<T>`
- `has_envelope_type<T>`
- `has_inject_into_method<T>`
- `is_metadata_injector<T>`
- `is_metadata_injector_v<T>`

The current contract expects a metadata injector to expose:
- `MetadataType` (nested type alias for metadata type)
- `EnvelopeType` (nested type alias for envelope type)
- `get_metadata() const` (metadata provision method)
- `inject_into(EnvelopeType&, const MetadataType&)` (envelope injection method)

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer static checks around assignability/copyability
- compatibility checks against envelope/assembler families
- explicit support for alternative injector entry shapes
- traits for mutable vs immutable metadata providers
- specialized injection strategies (conditional, validating, etc.)
- envelope type compatibility checking

What should NOT live here
-------------------------
This file must NOT:
- define a concrete metadata model
- implement injector logic
- mutate envelopes directly
- become an administrative service
- become a runtime registry layer

Design rule
-----------
This file defines architectural contract vocabulary only.
It expresses what higher-level preparation components may assume about a
metadata injector without implementing injector behavior itself.
------------------------------------------------------------------------------
*/

#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename, typename = void>
struct has_metadata_type : std::false_type {};

template <typename T>
struct has_metadata_type<T, std::void_t<typename T::MetadataType>>
    : std::true_type {};

template <typename, typename = void>
struct has_get_metadata_method : std::false_type {};

template <typename T>
struct has_get_metadata_method<
    T,
    std::void_t<decltype(std::declval<const T&>().get_metadata())>>
    : std::true_type {};

// Check if T has EnvelopeType nested alias
template <typename, typename = void>
struct has_envelope_type : std::false_type {};

template <typename T>
struct has_envelope_type<T, std::void_t<typename T::EnvelopeType>>
    : std::true_type {};

// Check if T has inject_into method for envelope injection (templated version)
template <typename, typename = void>
struct has_inject_into_method : std::false_type {};

template <typename T>
struct has_inject_into_method<T,
    std::void_t<decltype(std::declval<T&>().inject_into(
        std::declval<int&>(),  // Placeholder - method is templated
        std::declval<const typename T::MetadataType&>()))>>
    : std::true_type {};

// Updated contract: supports both metadata provision AND envelope injection
// Note: EnvelopeType is not strictly required since inject_into is templated
template <typename T>
struct is_metadata_injector
    : std::integral_constant<
          bool,
          has_metadata_type<T>::value &&
              has_get_metadata_method<T>::value &&
              has_inject_into_method<T>::value> {};

template <typename T>
inline constexpr bool is_metadata_injector_v =
    is_metadata_injector<T>::value;

}  // namespace logging_system::C_Contracts