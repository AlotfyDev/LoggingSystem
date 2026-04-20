#pragma once

/*
------------------------------------------------------------------------------
B_Models/log_envelope.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the unified envelope model for the consuming pipelines.

It answers narrow questions such as:
    "What is the prepared package produced by envelope assembly before registry
     admission?"
    "How can the envelope remain structurally unified while still preserving
     type-safe level/content-family specialization?"
    "Which fields belong to the prepared envelope itself before slot/record
     state is introduced?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- the consuming/level APIs should accept content only
- contextual semantics, if any, belong to content-schema meaning rather than a
  separate API parameter
- metadata and timestamp are injected during envelope assembly
- the envelope is the semantic prepared package
- the log record is not the envelope itself, but the registry slot that later
  contains:
  - envelope
  - slot/record state

This means the envelope model must provide:
- one unified structural template
- content-family-safe specialization through types, not runtime flags
- metadata presence
- timestamp presence
- schema identity presence
- no slot state
- no registry semantics

Current minimal scope
---------------------
This file currently provides:
- `LogEnvelope<TWrappedContent, TMetadata>`
- built-in level/family aliases:
  - `DebugLogEnvelope<TSchema>`
  - `InfoLogEnvelope<TSchema>`
  - `WarnLogEnvelope<TSchema>`
  - `ErrorLogEnvelope<TSchema>`
  - `FatalLogEnvelope<TSchema>`
  - `TraceLogEnvelope<TSchema>`
- built-in default aliases:
  - `DefaultDebugLogEnvelope`
  - `DefaultInfoLogEnvelope`
  - `DefaultWarnLogEnvelope`
  - `DefaultErrorLogEnvelope`
  - `DefaultFatalLogEnvelope`
  - `DefaultTraceLogEnvelope`

The envelope currently contains:
- wrapped content family
- metadata
- timestamp
- content schema id

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer schema/contract identity helpers
- optional envelope notes or assembly markers
- profile-driven envelope markers
- transport-neutral envelope hints
- stronger compile-time helper traits for envelope families

What should NOT live here
-------------------------
This file must NOT:
- own slot/record state
- validate content
- inject metadata
- inject timestamps
- become a registry record
- become a runtime state object

Design rule
-----------
This file defines the prepared envelope package only.
It is structurally unified, but family-safe specialization is preserved through
typed content wrappers and typed aliases.
------------------------------------------------------------------------------
*/
#pragma once

/*
------------------------------------------------------------------------------
B_Models/log_envelope.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V2

Role in the architecture
------------------------
This file defines the unified envelope model for the consuming pipelines.

It answers narrow questions such as:
    "What is the prepared package produced by envelope assembly before registry
     admission?"
    "How can the envelope remain structurally unified while still preserving
     type-safe level/content-family specialization?"
    "How can content-update time be represented numerically and refreshed
     automatically when the envelope content changes?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- the consuming/level APIs should accept content only
- contextual semantics, if any, belong to content-schema meaning rather than a
  separate API parameter
- metadata and timestamp are injected during envelope assembly
- the envelope is the semantic prepared package
- the log record is not the envelope itself, but the registry slot that later
  contains:
  - envelope
  - slot/record state

It was also clarified that:
- the envelope object itself may be reusable inside the assembler
- therefore the primary timestamp of interest is not "object creation time"
- the primary timestamp of interest is the numeric time associated with the
  current content value held by the envelope
- that timestamp should be refreshed automatically whenever the content is
  replaced or updated through the envelope API

This means the envelope model must provide:
- one unified structural template
- content-family-safe specialization through types, not runtime flags
- metadata presence
- schema identity presence
- numeric content-update timestamp
- automatic timestamp refresh on content assignment
- no slot state
- no registry semantics

Current minimal scope
---------------------
This file currently provides:
- `LogEnvelope<TWrappedContent, TMetadata>`
- built-in level/family aliases:
  - `DebugLogEnvelope<TSchema>`
  - `InfoLogEnvelope<TSchema>`
  - `WarnLogEnvelope<TSchema>`
  - `ErrorLogEnvelope<TSchema>`
  - `FatalLogEnvelope<TSchema>`
  - `TraceLogEnvelope<TSchema>`
- built-in default aliases:
  - `DefaultDebugLogEnvelope`
  - `DefaultInfoLogEnvelope`
  - `DefaultWarnLogEnvelope`
  - `DefaultErrorLogEnvelope`
  - `DefaultFatalLogEnvelope`
  - `DefaultTraceLogEnvelope`

The envelope currently contains:
- wrapped content family
- metadata
- numeric content-update timestamp
- content schema id

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer schema/contract identity helpers
- optional envelope notes or assembly markers
- profile-driven envelope markers
- transport-neutral envelope hints
- stronger compile-time helper traits for envelope families

What should NOT live here
-------------------------
This file must NOT:
- own slot/record state
- validate content
- inject metadata
- become a registry record
- become a runtime state object

Design rule
-----------
This file defines the prepared envelope package only.
It is structurally unified, but family-safe specialization is preserved through
typed content wrappers and typed aliases.

The timestamp held here represents the most recent meaningful content update
time for the envelope content, not the lifetime of the envelope object itself.
------------------------------------------------------------------------------
*/

#include <cstdint>
#include <string>
#include <utility>

#include "logging_system/B_Models/content_contracts.hpp"
#include "logging_system/B_Models/log_metadata.hpp"
#include "logging_system/B_Models/utc_now_iso.hpp"

namespace logging_system::B_Models {

template <typename TWrappedContent, typename TMetadata>
struct LogEnvelope final {
    using WrappedContentType = TWrappedContent;
    using MetadataType = TMetadata;

    TWrappedContent content{};
    TMetadata metadata{};

    std::int64_t content_updated_at_epoch{0};
    std::string content_schema_id{};

    LogEnvelope() = default;

    LogEnvelope(
        TWrappedContent content_in,
        TMetadata metadata_in,
        std::string content_schema_id_in)
        : content(std::move(content_in)),
          metadata(std::move(metadata_in)),
          content_updated_at_epoch(current_epoch_millis()),
          content_schema_id(std::move(content_schema_id_in)) {}

    void assign_content(TWrappedContent content_in) {
        content = std::move(content_in);
        refresh_content_timestamp();
    }

    void assign_metadata(TMetadata metadata_in) {
        metadata = std::move(metadata_in);
    }

    void assign_content_schema_id(std::string content_schema_id_in) {
        content_schema_id = std::move(content_schema_id_in);
    }

    void refresh_content_timestamp() {
        content_updated_at_epoch = current_epoch_millis();
    }

private:
    static std::int64_t current_epoch_millis() {
        return utc_now_epoch_millis();
    }
};

// -----------------------------------------------------------------------------
// Family-safe built-in aliases
// -----------------------------------------------------------------------------

template <typename TSchema>
using DebugLogEnvelope = LogEnvelope<LogDebugContent<TSchema>, LogMetadata>;

template <typename TSchema>
using InfoLogEnvelope = LogEnvelope<LogInfoContent<TSchema>, LogMetadata>;

template <typename TSchema>
using WarnLogEnvelope = LogEnvelope<LogWarnContent<TSchema>, LogMetadata>;

template <typename TSchema>
using ErrorLogEnvelope = LogEnvelope<LogErrorContent<TSchema>, LogMetadata>;

template <typename TSchema>
using FatalLogEnvelope = LogEnvelope<LogFatalContent<TSchema>, LogMetadata>;

template <typename TSchema>
using TraceLogEnvelope = LogEnvelope<LogTraceContent<TSchema>, LogMetadata>;

// -----------------------------------------------------------------------------
// Built-in default aliases
// -----------------------------------------------------------------------------

using DefaultDebugLogEnvelope = DebugLogEnvelope<DefaultDebugSchema>;
using DefaultInfoLogEnvelope  = InfoLogEnvelope<DefaultInfoSchema>;
using DefaultWarnLogEnvelope  = WarnLogEnvelope<DefaultWarnSchema>;
using DefaultErrorLogEnvelope = ErrorLogEnvelope<DefaultErrorSchema>;
using DefaultFatalLogEnvelope = FatalLogEnvelope<DefaultFatalSchema>;
using DefaultTraceLogEnvelope = TraceLogEnvelope<DefaultTraceSchema>;

}  // namespace logging_system::B_Models