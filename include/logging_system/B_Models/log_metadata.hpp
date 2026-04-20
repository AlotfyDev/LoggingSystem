#pragma once

/*
------------------------------------------------------------------------------
B_Models/log_metadata.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the metadata model used by the consuming pipelines before
registry admission.

It answers narrow questions such as:
    "What is the minimum metadata carried into the envelope?"
    "Which metadata field is administratively assigned and then injected
     automatically during assembly?"
    "How can metadata remain stable, explicit, and extendable without becoming
     part of the per-call consuming API surface?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- the consuming/level APIs should accept content only
- writer identity should not be passed on every API call
- writer identity is assigned once at system creation time, or later changed
  only through administrative configuration
- metadata and timestamp are injected during envelope assembly
- metadata must therefore exist as its own model before assembler work is
  formalized

This means the metadata layer must provide:
- a stable model for administratively assigned writer identity
- a minimal and explicit starting point
- no assembly logic
- no validation logic
- no registry/slot state logic

Current minimal scope
---------------------
This file currently provides:
- `LogMetadata`
- required metadata field:
  - `writer_id`
- helper operations for:
  - empty/non-empty writer identity checks
  - replacing the administratively assigned writer identity

This file is intentionally minimal.
It does not attempt to model every possible metadata field yet.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- source identity
- tenant / namespace markers
- deployment profile markers
- correlation identifiers
- optional semantic labels
- audit-oriented metadata extensions

What should NOT live here
-------------------------
This file must NOT:
- generate timestamps
- validate content
- assemble envelopes
- own record/slot state
- become an administrative service
- become a registry object

Design rule
-----------
This file defines the metadata value model only.
It represents administratively assigned metadata that later gets injected into
the envelope during assembly.
------------------------------------------------------------------------------
*/

#include <string>
#include <utility>

namespace logging_system::B_Models {

struct LogMetadata final {
    std::string writer_id{};

    LogMetadata() = default;

    explicit LogMetadata(std::string writer_id_in)
        : writer_id(std::move(writer_id_in)) {}

    [[nodiscard]] bool has_writer_id() const noexcept {
        return !writer_id.empty();
    }

    void assign_writer_id(std::string writer_id_in) {
        writer_id = std::move(writer_id_in);
    }
};

}  // namespace logging_system::B_Models