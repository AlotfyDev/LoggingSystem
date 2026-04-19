#pragma once

/*
------------------------------------------------------------------------------
D_Preparation/warn_preparation_binding.hpp

Reference Version
-----------------
WARN_SLICE_BASELINE_V1

Role in the architecture
------------------------
WarnPreparationBinding is the WARN-pipeline specialization of the generic
preparation binding family.

It answers one narrow question:
    "Which preparation-stage components constitute the preparation stack for the
     WARN pipeline right now?"

Why this file exists in this batch
----------------------------------
After the default reusable preparation implementations exist, each pipeline
needs an explicit preparation binding alias that states:
- which metadata injector it uses
- which timestamp stabilizer it uses
- which content schema applier it uses
- which envelope assembler it uses
- which record stabilizer it uses

This keeps preparation specialization explicit at the binding level, even when
the current implementations are still default/shared.

Current minimal scope
---------------------
This file currently binds:
- DefaultMetadataInjector
- DefaultTimestampStabilizer
- DefaultContentSchemaApplier
- DefaultEnvelopeAssembler
- DefaultRecordStabilizer

into:
- WarnPreparationBinding

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- replacing one or more default preparation components with WARN-specific
  specializations
- policy-bound preparation variants
- WARN-local schema shaping rules
- stronger compile-time validation against preparation contracts

What should NOT live here
-------------------------
This file must NOT:
- implement preparation behavior itself
- own state
- own routing
- own dispatch
- become a pipeline runner
- hold mutable runtime state

Design rule
-----------
This file is a binding alias only.
Its job is to make pipeline-local preparation composition explicit.
------------------------------------------------------------------------------
*/

#include "logging_system/A_Core/preparation_binding.hpp"

#include "logging_system/D_Preparation/default_content_schema_applier.hpp"
#include "logging_system/D_Preparation/default_envelope_assembler.hpp"
#include "logging_system/D_Preparation/default_metadata_injector.hpp"
#include "logging_system/D_Preparation/default_record_stabilizer.hpp"
#include "logging_system/D_Preparation/default_timestamp_stabilizer.hpp"

namespace logging_system::D_Preparation {

using WarnPreparationBinding = logging_system::A_Core::PreparationBinding<
    DefaultMetadataInjector,
    DefaultTimestampStabilizer,
    DefaultContentSchemaApplier,
    DefaultEnvelopeAssembler,
    DefaultRecordStabilizer>;

}  // namespace logging_system::D_Preparation