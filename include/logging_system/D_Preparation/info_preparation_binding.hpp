#pragma once

#include "logging_system/A_Core/preparation_binding.hpp"

#include "logging_system/D_Preparation/default_content_schema_applier.hpp"
#include "logging_system/D_Preparation/default_envelope_assembler.hpp"
#include "logging_system/D_Preparation/default_metadata_injector.hpp"
#include "logging_system/D_Preparation/default_record_stabilizer.hpp"
#include "logging_system/D_Preparation/default_timestamp_stabilizer.hpp"

namespace logging_system::D_Preparation {

using InfoPreparationBinding = logging_system::A_Core::PreparationBinding<
    DefaultMetadataInjector,
    DefaultTimestampStabilizer,
    DefaultContentSchemaApplier,
    DefaultEnvelopeAssembler,
    DefaultRecordStabilizer>;

}  // namespace logging_system::D_Preparation