#pragma once

/*
------------------------------------------------------------------------------
L_Level_api/log_debug.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
LogDebug is the thin dedicated DEBUG-level API façade for the consuming pipeline.

It answers narrow questions such as:
    "How does external code submit DEBUG-family content without passing through
     a generic shared service.log(level=...) convergence point?"
    "How can the DEBUG API remain an independent user-visible surface while
     owning its specialized assembler by composition?"
    "How can the API stay generic by default over content type, while still
     allowing higher layers to instantiate it with a concrete content type and
     a concrete specialized assembler?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- the level API must no longer be record-driven
- the API should accept content only
- the API is an independent surface object
- the assembler is an owned hidden dependency inside the API
- the user should see only the API
- the API forwards content to its owned assembler
- metadata comes from administrative assignment into the assembler
- content validation, if enabled, belongs to the validating assembler variant
- schema and specialized content-type administrative controls must remain
  outside the consumer-visible API surface

It was also clarified that:
- the API–assembler relationship is permanent and one-to-one
- repeated external lookup is unnecessary
- composition-based ownership is the cleanest binding model here
- administrative and managerial layers may restrict content type from the
  source, but the default API shape remains template-based

Current minimal scope
---------------------
This file currently provides:
- `LogDebug<TContent, TAssembler, TApiId>`
- fixed `level_key()`
- one user-visible thin method:
  - `AcceptLog(...)`
- API identity:
  - public getter
  - private setter
- private administrative content-type/schema control placeholders
- a static factory method for controlled API construction

The consumer sees only:
- a ready API object
- `AcceptLog(ContentType content)`

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- DEBUG-specific convenience aliases
- concrete default aliases for default DEBUG content wrappers
- stronger compile-time constraints for content/assembler compatibility
- integration with broader consuming surfaces
- final administrative schema/content-type control semantics once SystemAdmin
  is fully defined

What should NOT live here
-------------------------
This file must NOT:
- own registry state
- own dispatch/runtime logic
- own governance/configuration logic
- perform content validation itself
- inject metadata itself
- expose schema-level administrative concerns to consumers
- collapse back into shared `service.log(...)` routing

Design rule
-----------
This file defines a thin DEBUG-level API façade only.

The API is independent as a user-visible surface.
Its specialized assembler is owned internally by composition.
The consumer sees only content acceptance.
Administrative controls over identity and specialized content/schema binding
remain hidden behind private members and friend access.
------------------------------------------------------------------------------
*/

#include <utility>

namespace logging_system::L_Level_api {

class SystemAdmin;

template <typename TContent, typename TAssembler, typename TApiId>
class LogDebug final {
public:
    using ContentType = TContent;
    using AssemblerType = TAssembler;
    using ApiIdType = TApiId;

    LogDebug() = default;

    [[nodiscard]] static LogDebug Create(
        TApiId api_id,
        TAssembler assembler) {
        LogDebug api{};
        api.assembler_ = std::move(assembler);
        api.set_api_id_(std::move(api_id));
        return api;
    }

    static constexpr const char* level_key() noexcept {
        return "DEBUG";
    }

    [[nodiscard]] const TApiId& api_id() const noexcept {
        return api_id_;
    }

    [[nodiscard]] auto AcceptLog(TContent content) const {
        return assembler_.accept_content(std::move(content));
    }

private:
    friend class SystemAdmin;

    void set_api_id_(TApiId api_id_in) {
        api_id_ = std::move(api_id_in);
    }

    template <typename TRestrictedContent>
    void bind_content_type_restriction_() {
        content_type_restricted_ = true;
    }

    void clear_content_type_restriction_() {
        content_type_restricted_ = false;
    }

    [[nodiscard]] bool is_content_type_restricted_() const noexcept {
        return content_type_restricted_;
    }

    TApiId api_id_{};
    TAssembler assembler_{};
    bool content_type_restricted_{false};
};

}  // namespace logging_system::L_Level_api