#pragma once

/*
------------------------------------------------------------------------------
B_Models/content_contracts.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the typed content-family vocabulary for the consuming
pipelines.

It answers narrow questions such as:
    "What is the marker root for all consumable content families?"
    "How are level-specific content families represented as typed wrappers over
     user-defined schema payloads?"
    "How does the system provide default content-family wrappers while still
     allowing user-defined schema ownership?"

Why this file exists in this stage
----------------------------------
The consuming-side correction clarified that:
- consuming/level APIs must remain template-based
- the API input should be content-only
- contextual semantics, if any, belong to content-schema meaning rather than a
  separate API parameter
- metadata and timestamp are injected later during envelope assembly
- validation is an envelope-assembler concern, not an API concern

This means the content layer must provide:
- a marker root for content families
- explicit level-content wrapper families
- a place for default schema wrappers
- no validation logic
- no metadata/timestamp logic
- no registry/slot state logic

Current minimal scope
---------------------
This file currently provides:
- `IContent`
- generic level-content wrapper templates:
  - `LogDebugContent<TSchema>`
  - `LogInfoContent<TSchema>`
  - `LogWarnContent<TSchema>`
  - `LogErrorContent<TSchema>`
  - `LogFatalContent<TSchema>`
  - `LogTraceContent<TSchema>`
- default schema payload models for the built-in level families
- default aliases:
  - `DefaultLogDebugContent`
  - `DefaultLogInfoContent`
  - `DefaultLogWarnContent`
  - `DefaultLogErrorContent`
  - `DefaultLogFatalContent`
  - `DefaultLogTraceContent`
- content contract descriptor vocabulary:
  - `ContentForm`
  - `PrimitiveKind`
  - `FieldContract`
  - `ContentContract`

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- additional non-loglevel content families
- stronger compile-time traits for family/schema compatibility
- default schema trait helpers
- static contract identity helpers
- richer schema notes for contextual semantic grouping

What should NOT live here
-------------------------
This file must NOT:
- perform validation
- inject metadata
- inject timestamps
- assemble envelopes
- own slot/record state
- become a registry/service layer

Design rule
-----------
This file defines typed content-family vocabulary only.
It provides the marker root, family wrappers, and schema-contract descriptors,
while leaving validation/injection/assembly to later preparation components.
------------------------------------------------------------------------------
*/

#include <cstddef>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace logging_system::B_Models {

// -----------------------------------------------------------------------------
// Contract descriptor vocabulary
// -----------------------------------------------------------------------------

enum class ContentForm {
    Unknown,
    Scalar,
    Object,
    Sequence,
    NativeStructLike
};

enum class PrimitiveKind {
    Unknown,
    StringUtf8,
    Boolean,
    Int32,
    Int64,
    UInt32,
    UInt64,
    Float32,
    Float64,
    Bytes,
    Object,
    Sequence
};

struct FieldContract final {
    std::string name{};
    std::size_t ordinal{0};
    bool required{false};

    ContentForm logical_form{ContentForm::Unknown};
    PrimitiveKind primitive_kind{PrimitiveKind::Unknown};

    std::vector<std::string> enum_values{};
    std::optional<std::size_t> min_text_length{std::nullopt};

    std::optional<std::size_t> fixed_width_bytes{std::nullopt};
    std::optional<std::size_t> alignment_bytes{std::nullopt};
    bool padding_sensitive{false};

    std::string notes{};
};

struct ContentContract final {
    std::string schema_id{};
    std::string schema_name{};

    ContentForm content_form{ContentForm::Object};
    bool allow_additional_fields{true};

    std::vector<FieldContract> fields{};
    std::string notes{};

    [[nodiscard]] const FieldContract* find_field(
        const std::string& field_name) const noexcept {
        for (const auto& field : fields) {
            if (field.name == field_name) {
                return &field;
            }
        }
        return nullptr;
    }
};

// -----------------------------------------------------------------------------
// Typed content-family layer
// -----------------------------------------------------------------------------

struct IContent {};

// Default schema payload models for built-in level families.
// These are intentionally minimal and exist only to provide default wrapped
// content families. User-defined schema payloads remain fully supported via
// the template wrappers below.

struct DefaultDebugSchema final {
    std::string message{};
};

struct DefaultInfoSchema final {
    std::string message{};
};

struct DefaultWarnSchema final {
    std::string message{};
};

struct DefaultErrorSchema final {
    std::string message{};
};

struct DefaultFatalSchema final {
    std::string message{};
};

struct DefaultTraceSchema final {
    std::string message{};
};

template <typename TSchema>
struct LogDebugContent final : IContent {
    using SchemaType = TSchema;
    TSchema schema{};

    LogDebugContent() = default;

    explicit LogDebugContent(TSchema schema_in)
        : schema(std::move(schema_in)) {}
};

template <typename TSchema>
struct LogInfoContent final : IContent {
    using SchemaType = TSchema;
    TSchema schema{};

    LogInfoContent() = default;

    explicit LogInfoContent(TSchema schema_in)
        : schema(std::move(schema_in)) {}
};

template <typename TSchema>
struct LogWarnContent final : IContent {
    using SchemaType = TSchema;
    TSchema schema{};

    LogWarnContent() = default;

    explicit LogWarnContent(TSchema schema_in)
        : schema(std::move(schema_in)) {}
};

template <typename TSchema>
struct LogErrorContent final : IContent {
    using SchemaType = TSchema;
    TSchema schema{};

    LogErrorContent() = default;

    explicit LogErrorContent(TSchema schema_in)
        : schema(std::move(schema_in)) {}
};

template <typename TSchema>
struct LogFatalContent final : IContent {
    using SchemaType = TSchema;
    TSchema schema{};

    LogFatalContent() = default;

    explicit LogFatalContent(TSchema schema_in)
        : schema(std::move(schema_in)) {}
};

template <typename TSchema>
struct LogTraceContent final : IContent {
    using SchemaType = TSchema;
    TSchema schema{};

    LogTraceContent() = default;

    explicit LogTraceContent(TSchema schema_in)
        : schema(std::move(schema_in)) {}
};

// Default built-in family aliases.

using DefaultLogDebugContent = LogDebugContent<DefaultDebugSchema>;
using DefaultLogInfoContent  = LogInfoContent<DefaultInfoSchema>;
using DefaultLogWarnContent  = LogWarnContent<DefaultWarnSchema>;
using DefaultLogErrorContent = LogErrorContent<DefaultErrorSchema>;
using DefaultLogFatalContent = LogFatalContent<DefaultFatalSchema>;
using DefaultLogTraceContent = LogTraceContent<DefaultTraceSchema>;

// Optional compile-time helpers for family detection.
// These do not perform validation; they only help later layers reason about
// wrapped content families.

template <typename T>
struct is_content_family : std::is_base_of<IContent, std::remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_content_family_v = is_content_family<T>::value;

}  // namespace logging_system::B_Models