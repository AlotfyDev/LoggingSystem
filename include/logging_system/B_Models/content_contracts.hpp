#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace logging_system::B_Models {

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

    const FieldContract* find_field(const std::string& field_name) const noexcept {
        for (const auto& field : fields) {
            if (field.name == field_name) {
                return &field;
            }
        }
        return nullptr;
    }
};

}  // namespace logging_system::B_Models