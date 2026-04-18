C:.
│   Project_Initial_Tree.md
│
├───apps
├───include
│   └───logging_system
│       ├───A_Core
│       │       binding_concepts.hpp
│       │       binding_tags.hpp
│       │       binding_traits.hpp
│       │       compile_time_registry.hpp
│       │       component_pack.hpp
│       │       component_ref.hpp
│       │       dispatch_binding.hpp
│       │       pipeline_binding.hpp
│       │       preparation_binding.hpp
│       │       resolver_binding.hpp
│       │
│       ├───B_Models
│       │       content_contracts.hpp
│       │       default_content_schema_catalog.hpp
│       │       log_envelope.hpp
│       │       log_record.hpp
│       │       utc_now_iso.hpp
│       │
│       ├───C_Contracts
│       │       adapter_emission_concept.hpp
│       │       dispatcher_resolver_concept.hpp
│       │       dispatch_executor_concept.hpp
│       │       envelope_assembler_concept.hpp
│       │       metadata_injector_concept.hpp
│       │       readonly_resolver_concept.hpp
│       │       record_stabilizer_concept.hpp
│       │       repository_route_concept.hpp
│       │       schema_applier_concept.hpp
│       │       state_admission_sink_concept.hpp
│       │       timestamp_stabilizer_concept.hpp
│       │       writer_resolver_concept.hpp
│       │
│       ├───D_Preparation
│       │       content_schema_applier.hpp
│       │       debug_preparation_binding.hpp
│       │       default_content_schema_applier.hpp
│       │       default_envelope_assembler.hpp
│       │       default_metadata_injector.hpp
│       │       default_record_stabilizer.hpp
│       │       default_timestamp_stabilizer.hpp
│       │       envelope_assembler.hpp
│       │       error_preparation_binding.hpp
│       │       fatal_preparation_binding.hpp
│       │       info_preparation_binding.hpp
│       │       metadata_injector.hpp
│       │       record_stabilizer.hpp
│       │       timestamp_stabilizer.hpp
│       │       trace_preparation_binding.hpp
│       │       warn_preparation_binding.hpp
│       │
│       ├───E_Resolvers
│       │       debug_resolver_binding.hpp
│       │       default_dispatcher_resolver.hpp
│       │       default_readonly_resolver.hpp
│       │       default_writer_resolver.hpp
│       │       dispatcher_resolver.hpp
│       │       error_resolver_binding.hpp
│       │       fatal_resolver_binding.hpp
│       │       info_resolver_binding.hpp
│       │       readonly_resolver.hpp
│       │       trace_resolver_binding.hpp
│       │       warn_resolver_binding.hpp
│       │       writer_resolver.hpp
│       │
│       ├───F_Dispatch
│       │       adapter_emission.hpp
│       │       debug_dispatch_binding.hpp
│       │       default_adapter_emission.hpp
│       │       dispatch_context.hpp
│       │       dispatch_executor.hpp
│       │       dispatch_failure_policy.hpp
│       │       error_dispatch_binding.hpp
│       │       fatal_dispatch_binding.hpp
│       │       info_dispatch_binding.hpp
│       │       inline_dispatch_executor.hpp
│       │       queue_policy.hpp
│       │       thread_pool_dispatch_executor.hpp
│       │       trace_dispatch_binding.hpp
│       │       warn_dispatch_binding.hpp
│       │
│       ├───G_Routing
│       │       debug_repository_route.hpp
│       │       error_repository_route.hpp
│       │       fatal_repository_route.hpp
│       │       info_repository_route.hpp
│       │       repository_route.hpp
│       │       route_context.hpp
│       │       trace_repository_route.hpp
│       │       warn_repository_route.hpp
│       │
│       ├───H_State
│       │       level_containers.hpp
│       │       listener_registry.hpp
│       │       log_container_module.hpp
│       │       pending_record_queue.hpp
│       │       retention_policy.hpp
│       │       slot_lifecycle.hpp
│       │       state_snapshot.hpp
│       │
│       ├───I_Adapters
│       │       adapter_binding_state.hpp
│       │       adapter_concepts.hpp
│       │       adapter_registry.hpp
│       │       noop_adapter.hpp
│       │       observability_viewer_adapter.hpp
│       │       otel_adapter.hpp
│       │
│       ├───J_Governance
│       │       policy_registry.hpp
│       │       production_profile.hpp
│       │       production_profile_service.hpp
│       │       provider_catalog.hpp
│       │       provider_catalog_service.hpp
│       │       runtime_binding_catalog.hpp
│       │       schema_registry.hpp
│       │       typed_configuration_gateway.hpp
│       │
│       ├───K_Pipelines
│       │       debug_pipeline_binding.hpp
│       │       error_pipeline_binding.hpp
│       │       fatal_pipeline_binding.hpp
│       │       info_pipeline_binding.hpp
│       │       ingest_pipeline_facade.hpp
│       │       pipeline_binding_aliases.hpp
│       │       pipeline_runner.hpp
│       │       trace_pipeline_binding.hpp
│       │       warn_pipeline_binding.hpp
│       │
│       ├───L_Level_api
│       │       log_debug.hpp
│       │       log_error.hpp
│       │       log_fatal.hpp
│       │       log_info.hpp
│       │       log_trace.hpp
│       │       log_warn.hpp
│       │
│       ├───M_Surfaces
│       │       administrative_surface.hpp
│       │       consuming_surface.hpp
│       │       managerial_surface.hpp
│       │
│       └───N_System
│               logging_system_builder.hpp
│               logging_system_core.hpp
│               system_bindings.hpp
│               system_runtime_context.hpp
│
└───ProjectManagement
    └───Docs
            A_Cpp_Architectural_Skelton.md
            B_Python_To_Cpp.md