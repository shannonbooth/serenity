/*
 * Copyright (c) 2021-2023, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <LibJS/Heap/MarkedVector.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/Temporal/AbstractOperations.h>

namespace JS::Temporal {

#define JS_ENUMERATE_TIME_ZONE_METHODS                                                        \
    __JS_ENUMERATE(GetPossibleInstantsFor, getPossibleInstantsFor, get_possible_instants_for) \
    __JS_ENUMERATE(GetOffsetNanosecondsFor, getOffsetNanosecondsFor, get_offset_nanoseconds_for)

enum class TimeZoneMethod {
#define __JS_ENUMERATE(PascalName, camelName, snake_name) \
    PascalName,
    JS_ENUMERATE_TIME_ZONE_METHODS
#undef __JS_ENUMERATE
};

// 11.5.1 Time Zone Methods Records, https://tc39.es/proposal-temporal/#sec-temporal-time-zone-methods-records
struct TimeZoneMethodsRecord {
    // The time zone object, or a string indicating a built-in time zone.
    Variant<String, NonnullGCPtr<Object>> receiver; // [[Receiver]]

    // The time zone's getOffsetNanosecondsFor method. For a built-in time zone this is always %Temporal.TimeZone.prototype.getOffsetNanosecondsFor%.
    GCPtr<FunctionObject> get_offset_nanoseconds_for; // [[GetOffsetNanosecondsFor]]

    // The time zone's getPossibleInstantsFor method. For a built-in time zone this is always %Temporal.TimeZone.prototype.getPossibleInstantsFor%.
    GCPtr<FunctionObject> get_possible_instants_for; // [[GetPossibleInstantsFor]]
};
class TimeZone final : public Object {
    JS_OBJECT(TimeZone, Object);
    JS_DECLARE_ALLOCATOR(TimeZone);

public:
    // Needs to store values in the range -8.64 * 10^13 to 8.64 * 10^13
    using OffsetType = double;

    virtual ~TimeZone() override = default;

    [[nodiscard]] String const& identifier() const { return m_identifier; }
    [[nodiscard]] Optional<OffsetType> const& offset_nanoseconds() const { return m_offset_nanoseconds; }

    void set_identifier(String identifier) { m_identifier = move(identifier); }
    void set_offset_nanoseconds(OffsetType offset_nanoseconds) { m_offset_nanoseconds = offset_nanoseconds; }

private:
    explicit TimeZone(Object& prototype);

    // 11.5 Properties of Temporal.TimeZone Instances, https://tc39.es/proposal-temporal/#sec-properties-of-temporal-timezone-instances
    String m_identifier;                       // [[Identifier]]
    Optional<OffsetType> m_offset_nanoseconds; // [[OffsetNanoseconds]]
};

bool is_available_time_zone_name(StringView time_zone);
ThrowCompletionOr<String> canonicalize_time_zone_name(VM&, StringView time_zone);
ThrowCompletionOr<TimeZone*> create_temporal_time_zone(VM&, StringView identifier, FunctionObject const* new_target = nullptr);
ISODateTime get_iso_parts_from_epoch(VM&, Crypto::SignedBigInteger const& epoch_nanoseconds);
BigInt* get_named_time_zone_next_transition(VM&, StringView time_zone_identifier, BigInt const& epoch_nanoseconds);
BigInt* get_named_time_zone_previous_transition(VM&, StringView time_zone_identifier, BigInt const& epoch_nanoseconds);
ThrowCompletionOr<String> format_time_zone_offset_string(VM&, double offset_nanoseconds);
ThrowCompletionOr<String> format_iso_time_zone_offset_string(VM&, double offset_nanoseconds);
ThrowCompletionOr<Object*> to_temporal_time_zone(VM&, Value temporal_time_zone_like);
ThrowCompletionOr<double> get_offset_nanoseconds_for(VM&, Value time_zone, Instant&);
ThrowCompletionOr<String> builtin_time_zone_get_offset_string_for(VM&, Value time_zone, Instant&);
ThrowCompletionOr<PlainDateTime*> builtin_time_zone_get_plain_date_time_for(VM&, Value time_zone, Instant&, Object& calendar);
ThrowCompletionOr<Instant*> builtin_time_zone_get_instant_for(VM&, Value time_zone, PlainDateTime&, StringView disambiguation);
ThrowCompletionOr<Instant*> disambiguate_possible_instants(VM&, MarkedVector<Instant*> const& possible_instants, Value time_zone, PlainDateTime&, StringView disambiguation);
ThrowCompletionOr<MarkedVector<Instant*>> get_possible_instants_for(VM&, Value time_zone, PlainDateTime&);
ThrowCompletionOr<bool> time_zone_equals(VM&, Object& one, Object& two);

ThrowCompletionOr<void> time_zone_methods_record_lookup(VM&, TimeZoneMethodsRecord&, TimeZoneMethod);
ThrowCompletionOr<TimeZoneMethodsRecord> create_time_zone_methods_record(VM&, NonnullGCPtr<Object> time_zone, Vector<TimeZoneMethod> const&);
bool time_zone_methods_record_has_looked_up(TimeZoneMethodsRecord const&, TimeZoneMethod);
bool time_zone_methods_record_is_builtin(TimeZoneMethodsRecord const&);
ThrowCompletionOr<Value> time_zone_methods_record_call(VM&, TimeZoneMethodsRecord const&, TimeZoneMethod, Span<Value> arguments);

// FIXME: Wrong place for this one!
ThrowCompletionOr<NonnullGCPtr<PlainDateTime>> get_plain_date_time_for(VM&, TimeZoneMethodsRecord const&, Instant&, Variant<String, NonnullGCPtr<Object>> const& calendar, Optional<double> const& precalculated_offset_nanoseconds = {});
}
