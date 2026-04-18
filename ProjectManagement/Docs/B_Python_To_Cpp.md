
، **القالب التنفيذي** مباشرة:
جدول mapping رسمي + شجرة ملفات C++ binding-centered للمسار `ingest/runtime`.

---

# 1) القاعدة المعتمدة من الآن

سأعتبر النمط الرسمي هو:

> **Generic pattern**
> +
> **Reusable shared implementation**
> +
> **Compile-time specialized binding family**
> +
> **Final per-pipeline assembly**

وده متسق مع:

* specialized pipelines
* resolver pipelines
* dedicated level APIs
* generic templated layer
  كما ورد في README 

---

# 2) الشجرة الرسمية المقترحة

```text
include/logging_system/
  core/
    binding_tags.hpp
    binding_traits.hpp
    binding_concepts.hpp
    component_ref.hpp
    component_pack.hpp
    compile_time_registry.hpp
    preparation_binding.hpp
    resolver_binding.hpp
    dispatch_binding.hpp
    pipeline_binding.hpp

  models/
    utc_now_iso.hpp
    log_record.hpp
    log_envelope.hpp
    content_contracts.hpp
    default_content_schema_catalog.hpp

  contracts/
    metadata_injector_concept.hpp
    timestamp_stabilizer_concept.hpp
    schema_applier_concept.hpp
    envelope_assembler_concept.hpp
    record_stabilizer_concept.hpp
    writer_resolver_concept.hpp
    dispatcher_resolver_concept.hpp
    readonly_resolver_concept.hpp
    dispatch_executor_concept.hpp
    adapter_emission_concept.hpp
    repository_route_concept.hpp
    state_admission_sink_concept.hpp

  preparation/
    metadata_injector.hpp
    timestamp_stabilizer.hpp
    content_schema_applier.hpp
    envelope_assembler.hpp
    record_stabilizer.hpp
    default_metadata_injector.hpp
    default_timestamp_stabilizer.hpp
    default_content_schema_applier.hpp
    default_envelope_assembler.hpp
    default_record_stabilizer.hpp
    info_preparation_binding.hpp
    debug_preparation_binding.hpp
    warn_preparation_binding.hpp
    error_preparation_binding.hpp
    fatal_preparation_binding.hpp
    trace_preparation_binding.hpp

  resolvers/
    writer_resolver.hpp
    dispatcher_resolver.hpp
    readonly_resolver.hpp
    default_writer_resolver.hpp
    default_dispatcher_resolver.hpp
    default_readonly_resolver.hpp
    info_resolver_binding.hpp
    debug_resolver_binding.hpp
    warn_resolver_binding.hpp
    error_resolver_binding.hpp
    fatal_resolver_binding.hpp
    trace_resolver_binding.hpp

  dispatch/
    dispatch_executor.hpp
    dispatch_failure_policy.hpp
    queue_policy.hpp
    adapter_emission.hpp
    dispatch_context.hpp
    inline_dispatch_executor.hpp
    thread_pool_dispatch_executor.hpp
    default_adapter_emission.hpp
    info_dispatch_binding.hpp
    debug_dispatch_binding.hpp
    warn_dispatch_binding.hpp
    error_dispatch_binding.hpp
    fatal_dispatch_binding.hpp
    trace_dispatch_binding.hpp

  routing/
    repository_route.hpp
    route_context.hpp
    info_repository_route.hpp
    debug_repository_route.hpp
    warn_repository_route.hpp
    error_repository_route.hpp
    fatal_repository_route.hpp
    trace_repository_route.hpp

  state/
    log_container_module.hpp
    level_containers.hpp
    slot_lifecycle.hpp
    listener_registry.hpp
    pending_record_queue.hpp
    retention_policy.hpp
    state_snapshot.hpp

  adapters/
    adapter_concepts.hpp
    adapter_registry.hpp
    adapter_binding_state.hpp
    noop_adapter.hpp
    otel_adapter.hpp
    observability_viewer_adapter.hpp

  governance/
    schema_registry.hpp
    policy_registry.hpp
    provider_catalog.hpp
    provider_catalog_service.hpp
    production_profile.hpp
    production_profile_service.hpp
    typed_configuration_gateway.hpp
    runtime_binding_catalog.hpp

  pipelines/
    pipeline_binding_aliases.hpp
    ingest_pipeline_facade.hpp
    pipeline_runner.hpp
    info_pipeline_binding.hpp
    debug_pipeline_binding.hpp
    warn_pipeline_binding.hpp
    error_pipeline_binding.hpp
    fatal_pipeline_binding.hpp
    trace_pipeline_binding.hpp

  level_api/
    log_debug.hpp
    log_info.hpp
    log_warn.hpp
    log_error.hpp
    log_fatal.hpp
    log_trace.hpp

  surfaces/
    consuming_surface.hpp
    administrative_surface.hpp
    managerial_surface.hpp

  system/
    system_bindings.hpp
    system_runtime_context.hpp
    logging_system_core.hpp
    logging_system_builder.hpp

apps/
  logging_cli/
    main.cpp
```

---

# 3) جدول الـ mapping الرسمي

## 3.1 Python family → C++ target family

| Python source family            | C++ target                                                                    | classification                                   |
| ------------------------------- | ----------------------------------------------------------------------------- | ------------------------------------------------ |
| `models/*`                      | `models/*`                                                                    | abstract generic                                 |
| `ports/*`                       | `surfaces/*` + selected compile-time contracts                                | façade / contract expression                     |
| `level_api/*`                   | `level_api/*` + `pipelines/*_pipeline_binding.hpp`                            | per-pipeline final assembly                      |
| `handlers/log_level_handler.py` | **لا يُنقل حرفيًا كـ shared handler**                                         | يتفكك إلى binding-centered ingest pattern        |
| `resolvers/*`                   | `resolvers/*` + `contracts/*resolver*_concept.hpp` + `*_resolver_binding.hpp` | generic pattern + specialized binding family     |
| `log_container_module/*`        | `state/log_container_module.hpp`                                              | shared reusable implementation                   |
| `containers/*`                  | `state/*`                                                                     | shared reusable implementation                   |
| `resource_management/*`         | `dispatch/*` و/أو `system/*`                                                  | shared execution support                         |
| `adapters/*`                    | `adapters/*`                                                                  | shared reusable implementation                   |
| `provider_catalogs/*`           | `governance/*`                                                                | shared reusable implementation                   |
| `production_profiles/*`         | `governance/*`                                                                | shared reusable implementation                   |
| `configurator/*`                | `governance/typed_configuration_gateway.hpp`                                  | shared reusable implementation                   |
| `services/logging_service.py`   | **لا يُنقل كملف مقابل 1:1**                                                   | يتفكك إلى `surfaces/` + `system/` + `pipelines/` |

---

## 3.2 ملفات Python المركزية وتفكيكها

| Python file                                 | C++ decomposition                                                                                                | note                       |
| ------------------------------------------- | ---------------------------------------------------------------------------------------------------------------- | -------------------------- |
| `services/logging_service.py`               | `system/logging_system_core.hpp`, `system/system_bindings.hpp`, `surfaces/*.hpp`, `pipelines/*`                  | لا monolith                |
| `handlers/log_level_handler.py`             | `core/pipeline_binding.hpp`, `pipelines/ingest_pipeline_facade.hpp`, `preparation/*`, `resolvers/*`, `routing/*` | shared handler يُفكك       |
| `level_api/log_info.py`                     | `level_api/log_info.hpp` + `pipelines/info_pipeline_binding.hpp`                                                 | thin API bound to pipeline |
| `level_api/log_debug.py`                    | `level_api/log_debug.hpp` + `pipelines/debug_pipeline_binding.hpp`                                               | same pattern               |
| `level_api/log_warn.py`                     | `level_api/log_warn.hpp` + `pipelines/warn_pipeline_binding.hpp`                                                 | same pattern               |
| `level_api/log_error.py`                    | `level_api/log_error.hpp` + `pipelines/error_pipeline_binding.hpp`                                               | same pattern               |
| `level_api/log_fatal.py`                    | `level_api/log_fatal.hpp` + `pipelines/fatal_pipeline_binding.hpp`                                               | same pattern               |
| `level_api/log_trace.py`                    | `level_api/log_trace.hpp` + `pipelines/trace_pipeline_binding.hpp`                                               | same pattern               |
| `log_container_module/service.py`           | `state/log_container_module.hpp`                                                                                 | shared state core          |
| `containers/level_containers.py`            | `state/level_containers.hpp`                                                                                     | shared state indexing      |
| `containers/slot_lifecycle.py`              | `state/slot_lifecycle.hpp`                                                                                       | shared lifecycle machine   |
| `resolvers/writer_resolver_pipeline.py`     | `resolvers/writer_resolver.hpp` + `*_resolver_binding.hpp`                                                       | specialized binding family |
| `resolvers/dispatcher_resolver_pipeline.py` | `resolvers/dispatcher_resolver.hpp` + `*_resolver_binding.hpp`                                                   | specialized binding family |
| `resolvers/readonly_resolver_pipeline.py`   | `resolvers/readonly_resolver.hpp` + `*_resolver_binding.hpp`                                                     | specialized binding family |

---

# 4) أهم قاعدة تنفيذية

## 4.1 ما لا نفعله

لا ننقل:

* `LoggingService.log()`
* shared `LogLevelHandler`
* generic shared convergence point

لأن الـ call/dependency map أثبت أن Python الحالية ما زالت centralized هنا 
والـ divergence map أثبت أن هذا هو الانحراف الأكبر عن intended per-loglevel architecture 

## 4.2 ما نفعله بدلًا من ذلك

لكل pipeline:

```cpp
using InfoPipelineBinding = PipelineBinding<
    InfoPreparationBinding,
    InfoResolverBinding,
    InfoDispatchBinding,
    InfoRepositoryRoute>;
```

وهكذا لكل level.

---

# 5) تصنيف الملفات حسب الطبقة

## Abstract Generic

* `core/*`
* `contracts/*`
* `models/*`

## Shared Reusable Implementation

* `state/*`
* `adapters/*`
* `governance/*`
* default implementations inside:

  * `preparation/*`
  * `resolvers/*`
  * `dispatch/*`

## Specialized Binding Family

* `preparation/*_preparation_binding.hpp`
* `resolvers/*_resolver_binding.hpp`
* `dispatch/*_dispatch_binding.hpp`
* `routing/*_repository_route.hpp`

## Final Per-Pipeline Assembly

* `pipelines/*_pipeline_binding.hpp`
* `level_api/log_*.hpp`

---

# 6) مثال INFO end-to-end

```text
preparation/info_preparation_binding.hpp
resolvers/info_resolver_binding.hpp
dispatch/info_dispatch_binding.hpp
routing/info_repository_route.hpp
pipelines/info_pipeline_binding.hpp
level_api/log_info.hpp
```

والـ binding النهائية:

```cpp
using InfoPipelineBinding = PipelineBinding<
    InfoPreparationBinding,
    InfoResolverBinding,
    InfoDispatchBinding,
    InfoRepositoryRoute>;
```

---

# 7) الحكم النهائي

هذه الشجرة ليست مجرد فصل:

* shared
* وper-level

بل فصل رباعي أدق:

1. generic abstraction pattern
2. reusable shared implementation
3. compile-time specialized binding family
4. final per-pipeline assembly

وده هو الشكل الصحيح لمسار C++ عندك.

الخطوة التالية الأنسب الآن هي أن أبدأ بـ **أول batch C++ فعلي وفق الشجرة الجديدة**:
`core/pipeline_binding.hpp` + `core/preparation_binding.hpp` + `core/resolver_binding.hpp` + `core/dispatch_binding.hpp`
