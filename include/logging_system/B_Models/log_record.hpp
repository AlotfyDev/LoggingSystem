#pragma once

/*
------------------------------------------------------------------------------
B_Models/log_record.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the log record as the registry slot model of the logging
system.

It answers narrow questions such as:
    "What is the runtime/storage entry that holds a prepared envelope after
     admission into the log registry?"
    "How can the record carry both the envelope and the slot/record state
     without pretending to be a semantic successor of the envelope?"
    "How can the record expose a composite slot identity with:
         - a primary registry index
         - an optional storage-local locator
     for specialized backends such as shared-memory registries?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- the envelope is the prepared semantic package
- the envelope contains:
  - wrapped content
  - metadata
  - timestamp
  - schema identity
- the record is not a semantic transformation above the envelope
- the record is the registry slot that contains:
  - the envelope
  - the slot/record state

It was also clarified that:
- the primary identity used by resolvers, dispatcher, and consumers should be a
  typed numeric slot index
- some registry backends may additionally require a storage-local locator such
  as a shared-memory slot location or memory offset
- this should be modeled as a second identity component, not as an alias of the
  primary index

This means the record model must provide:
- a stable typed slot identity
- a primary numeric slot index
- an optional storage-local locator
- a place for the envelope
- a place for slot/record state
- no responsibility for content validation
- no responsibility for metadata/timestamp injection
- no responsibility for semantic envelope assembly

Current minimal scope
---------------------
This file currently provides:
- `RecordStorageLocator`
- `RecordSlotIdentity`
- `RecordState`
- `LogRecord<TEnvelope>`

The record currently contains:
- `slot_identity`
- `envelope`
- `state`

This file intentionally models the slot/record entry only.
It does not model state-machine transitions or admission policies directly.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer lifecycle-state metadata
- state transition timestamps
- assigned adapter identity
- dispatch round markers
- retry/failure accounting markers
- record-local audit fields

What should NOT live here
-------------------------
This file must NOT:
- validate content
- assemble envelopes
- inject metadata
- inject timestamps
- become a state-machine manager
- become a storage service

Design rule
-----------
This file defines the registry slot model only.
A log record is a slot entry that contains:
- envelope
- state

Its identity is composite:
- a primary typed numeric slot index
- an optional storage-local locator for specialized registry backends
------------------------------------------------------------------------------
*/

#include <cstddef>
#include <optional>
#include <utility>

#include "logging_system/B_Models/log_envelope.hpp"

namespace logging_system::B_Models {

struct RecordStorageLocator final {
    std::size_t value{0};

    RecordStorageLocator() = default;

    explicit RecordStorageLocator(std::size_t value_in)
        : value(value_in) {}
};

struct RecordSlotIdentity final {
    std::size_t slot_index{0};
    std::optional<RecordStorageLocator> storage_locator{std::nullopt};

    RecordSlotIdentity() = default;

    explicit RecordSlotIdentity(std::size_t slot_index_in)
        : slot_index(slot_index_in) {}

    RecordSlotIdentity(
        std::size_t slot_index_in,
        std::optional<RecordStorageLocator> storage_locator_in)
        : slot_index(slot_index_in),
          storage_locator(std::move(storage_locator_in)) {}
};

enum class RecordState {
    Unknown,
    Pending,
    Dispatched,
    Failed,
    Retired
};

template <typename TEnvelope>
struct LogRecord final {
    using EnvelopeType = TEnvelope;

    RecordSlotIdentity slot_identity{};
    TEnvelope envelope{};
    RecordState state{RecordState::Unknown};

    LogRecord() = default;

    LogRecord(
        RecordSlotIdentity slot_identity_in,
        TEnvelope envelope_in,
        RecordState state_in = RecordState::Unknown)
        : slot_identity(std::move(slot_identity_in)),
          envelope(std::move(envelope_in)),
          state(state_in) {}
};

}  // namespace logging_system::B_Models