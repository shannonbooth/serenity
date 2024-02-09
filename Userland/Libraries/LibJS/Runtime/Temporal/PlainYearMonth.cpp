/*
 * Copyright (c) 2021-2023, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2023-2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/TypeCasts.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/Array.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/Temporal/AbstractOperations.h>
#include <LibJS/Runtime/Temporal/Calendar.h>
#include <LibJS/Runtime/Temporal/Duration.h>
#include <LibJS/Runtime/Temporal/PlainDate.h>
#include <LibJS/Runtime/Temporal/PlainYearMonth.h>
#include <LibJS/Runtime/Temporal/PlainYearMonthConstructor.h>
#include <LibJS/Runtime/Temporal/TimeZone.h>

namespace JS::Temporal {

JS_DEFINE_ALLOCATOR(PlainYearMonth);

// 9 Temporal.PlainYearMonth Objects, https://tc39.es/proposal-temporal/#sec-temporal-plainyearmonth-objects
PlainYearMonth::PlainYearMonth(i32 iso_year, u8 iso_month, u8 iso_day, Variant<String, NonnullGCPtr<Object>> calendar, Object& prototype)
    : Object(ConstructWithPrototypeTag::Tag, prototype)
    , m_iso_year(iso_year)
    , m_iso_month(iso_month)
    , m_iso_day(iso_day)
    , m_calendar(move(calendar))
{
}

void PlainYearMonth::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    if (auto const* calander = m_calendar.get_pointer<NonnullGCPtr<Object>>())
        visitor.visit(*calander);
}

// 9.5.1 ToTemporalYearMonth ( item [ , options ] ), https://tc39.es/proposal-temporal/#sec-temporal-totemporalyearmonth
ThrowCompletionOr<PlainYearMonth*> to_temporal_year_month(VM& vm, Value item, Object const* options)
{
    // 1. If options is not present, set options to undefined.
    // 2. Assert: Type(options) is Object or Undefined.

    // 3. If Type(item) is Object, then
    if (item.is_object()) {
        auto& item_object = item.as_object();

        // a. If item has an [[InitializedTemporalYearMonth]] internal slot, then
        if (is<PlainYearMonth>(item_object)) {
            // i. Return item.
            return static_cast<PlainYearMonth*>(&item_object);
        }

        // b. Let calendar be ? GetTemporalCalendarWithISODefault(item).
        auto* calendar = TRY(get_temporal_calendar_with_iso_default(vm, item_object));

        // c. Let fieldNames be ? CalendarFields(calendar, « "month", "monthCode", "year" »).
        auto field_names = TRY(calendar_fields(vm, *calendar, { "month"sv, "monthCode"sv, "year"sv }));

        // d. Let fields be ? PrepareTemporalFields(item, fieldNames, «»).
        auto* fields = TRY(prepare_temporal_fields(vm, item_object, field_names, Vector<StringView> {}));

        // e. Return ? CalendarYearMonthFromFields(calendar, fields, options).
        return calendar_year_month_from_fields(vm, *calendar, *fields, options);
    }

    // 4. Perform ? ToTemporalOverflow(options).
    (void)TRY(to_temporal_overflow(vm, options));

    // 5. Let string be ? ToString(item).
    auto string = TRY(item.to_string(vm));

    // 6. Let result be ? ParseTemporalYearMonthString(string).
    auto result = TRY(parse_temporal_year_month_string(vm, string));

    // 7. Let calendar be ? ToTemporalCalendarWithISODefault(result.[[Calendar]]).
    auto* calendar = TRY(to_temporal_calendar_with_iso_default(vm, result.calendar.has_value() ? PrimitiveString::create(vm, *result.calendar) : js_undefined()));

    // 8. Set result to ? CreateTemporalYearMonth(result.[[Year]], result.[[Month]], calendar, result.[[Day]]).
    auto* creation_result = TRY(create_temporal_year_month(vm, result.year, result.month, *calendar, result.day));

    // 9. NOTE: The following operation is called without options, in order for the calendar to store a canonical value in the [[ISODay]] internal slot of the result.
    // 10. Return ? CalendarYearMonthFromFields(calendar, result).
    return calendar_year_month_from_fields(vm, *calendar, *creation_result);
}

// 9.5.2 RegulateISOYearMonth ( year, month, overflow ), https://tc39.es/proposal-temporal/#sec-temporal-regulateisoyearmonth
ThrowCompletionOr<ISOYearMonth> regulate_iso_year_month(VM& vm, double year, double month, StringView overflow)
{
    // 1. Assert: year and month are integers.
    VERIFY(year == trunc(year) && month == trunc(month));

    // 2. Assert: overflow is either "constrain" or "reject".
    // NOTE: Asserted by the VERIFY_NOT_REACHED at the end

    // 3. If overflow is "constrain", then
    if (overflow == "constrain"sv) {
        // IMPLEMENTATION DEFINED: This is an optimization that allows us to treat `year` (a double) as normal integer from this point onwards.
        // This does not change the exposed behavior as the subsequent call to CreateTemporalYearMonth will check that its value is a valid ISO
        // values (for years: -273975 - 273975) which is a subset of this check.
        // If RegulateISOYearMonth is ever used outside ISOYearMonthFromFields, this may need to be changed.
        if (!AK::is_within_range<i32>(year))
            return vm.throw_completion<RangeError>(ErrorType::TemporalInvalidPlainYearMonth);

        // a. Set month to the result of clamping month between 1 and 12.
        month = clamp(month, 1, 12);

        // b. Return the Record { [[Year]]: year, [[Month]]: month }.
        return ISOYearMonth { .year = static_cast<i32>(year), .month = static_cast<u8>(month), .reference_iso_day = 0 };
    }
    // 4. Else,
    else {
        // a. Assert: overflow is "reject".
        VERIFY(overflow == "reject"sv);

        // IMPLEMENTATION DEFINED: This is an optimization that allows us to treat these doubles as normal integers from this point onwards.
        // This does not change the exposed behavior as the call to IsValidISOMonth and subsequent call to CreateTemporalDateTime will check
        // that these values are valid ISO values (for years: -273975 - 273975, for months: 1 - 12) all of which are subsets of this check.
        if (!AK::is_within_range<i32>(year) || !AK::is_within_range<u8>(month))
            return vm.throw_completion<RangeError>(ErrorType::TemporalInvalidPlainYearMonth);

        // b. If month < 1 or month > 12, throw a RangeError exception.
        if (month < 1 || month > 12)
            return vm.throw_completion<RangeError>(ErrorType::TemporalInvalidPlainYearMonth);

        // c. Return the Record { [[Year]]: year, [[Month]]: month }.
        return ISOYearMonth { .year = static_cast<i32>(year), .month = static_cast<u8>(month), .reference_iso_day = 0 };
    }
}

// 9.5.3 ISOYearMonthWithinLimits ( year, month ), https://tc39.es/proposal-temporal/#sec-temporal-isoyearmonthwithinlimits
bool iso_year_month_within_limits(i32 year, u8 month)
{
    // 1. Assert: year and month are integers.

    // 2. If year < -271821 or year > 275760, then
    if (year < -271821 || year > 275760) {
        // a. Return false.
        return false;
    }

    // 3. If year is -271821 and month < 4, then
    if (year == -271821 && month < 4) {
        // a. Return false.
        return false;
    }

    // 4. If year is 275760 and month > 9, then
    if (year == 275760 && month > 9) {
        // a. Return false.
        return false;
    }

    // 5. Return true.
    return true;
}

// 9.5.4 BalanceISOYearMonth ( year, month ), https://tc39.es/proposal-temporal/#sec-temporal-balanceisoyearmonth
ISOYearMonth balance_iso_year_month(double year, double month)
{
    // 1. Assert: year and month are integers.
    VERIFY(year == trunc(year) && month == trunc(month));

    // 2. Set year to year + floor((month - 1) / 12).
    year += floor((month - 1) / 12);

    // 3. Set month to ((month - 1) modulo 12) + 1.
    month = modulo(month - 1, 12) + 1;

    // 4. Return the Record { [[Year]]: year, [[Month]]: month }.
    return ISOYearMonth { .year = static_cast<i32>(year), .month = static_cast<u8>(month), .reference_iso_day = 0 };
}

// 9.5.5 CreateTemporalYearMonth ( isoYear, isoMonth, calendar, referenceISODay [ , newTarget ] ), https://tc39.es/proposal-temporal/#sec-temporal-createtemporalyearmonth
ThrowCompletionOr<PlainYearMonth*> create_temporal_year_month(VM& vm, i32 iso_year, u8 iso_month, Object& calendar, u8 reference_iso_day, FunctionObject const* new_target)
{
    auto& realm = *vm.current_realm();

    // 1. Assert: isoYear, isoMonth, and referenceISODay are integers.
    // 2. Assert: Type(calendar) is Object.

    // 3. If IsValidISODate(isoYear, isoMonth, referenceISODay) is false, throw a RangeError exception.
    if (!is_valid_iso_date(iso_year, iso_month, reference_iso_day))
        return vm.throw_completion<RangeError>(ErrorType::TemporalInvalidPlainYearMonth);

    // 4. If ! ISOYearMonthWithinLimits(isoYear, isoMonth) is false, throw a RangeError exception.
    if (!iso_year_month_within_limits(iso_year, iso_month))
        return vm.throw_completion<RangeError>(ErrorType::TemporalInvalidPlainYearMonth);

    // 5. If newTarget is not present, set newTarget to %Temporal.PlainYearMonth%.
    if (!new_target)
        new_target = realm.intrinsics().temporal_plain_year_month_constructor();

    // 6. Let object be ? OrdinaryCreateFromConstructor(newTarget, "%Temporal.PlainYearMonth.prototype%", « [[InitializedTemporalYearMonth]], [[ISOYear]], [[ISOMonth]], [[ISODay]], [[Calendar]] »).
    // 7. Set object.[[ISOYear]] to isoYear.
    // 8. Set object.[[ISOMonth]] to isoMonth.
    // 9. Set object.[[Calendar]] to calendar.
    // 10. Set object.[[ISODay]] to referenceISODay.
    auto object = TRY(ordinary_create_from_constructor<PlainYearMonth>(vm, *new_target, &Intrinsics::temporal_plain_year_month_prototype, iso_year, iso_month, reference_iso_day, calendar));

    // 11. Return object.
    return object.ptr();
}

// 9.5.6 TemporalYearMonthToString ( yearMonth, showCalendar ), https://tc39.es/proposal-temporal/#sec-temporal-temporalyearmonthtostring
ThrowCompletionOr<String> temporal_year_month_to_string(VM& vm, PlainYearMonth& year_month, StringView show_calendar)
{
    // 1. Assert: Type(yearMonth) is Object.
    // 2. Assert: yearMonth has an [[InitializedTemporalYearMonth]] internal slot.

    // 3. Let year be ! PadISOYear(yearMonth.[[ISOYear]]).
    // 4. Let month be ToZeroPaddedDecimalString(yearMonth.[[ISOMonth]], 2).
    // 5. Let result be the string-concatenation of year, the code unit 0x002D (HYPHEN-MINUS), and month.
    auto result = TRY_OR_THROW_OOM(vm, String::formatted("{}-{:02}", MUST_OR_THROW_OOM(pad_iso_year(vm, year_month.iso_year())), year_month.iso_month()));

    // 6. Let calendarID be ? ToString(yearMonth.[[Calendar]]).
    auto calendar_id = TRY(Value(&year_month.calendar()).to_string(vm));

    // 7. If showCalendar is one of "always" or "critical", or if calendarID is not "iso8601", then
    if (show_calendar.is_one_of("always"sv, "critical"sv) || calendar_id != "iso8601") {
        // a. Let day be ToZeroPaddedDecimalString(yearMonth.[[ISODay]], 2).
        // b. Set result to the string-concatenation of result, the code unit 0x002D (HYPHEN-MINUS), and day.
        result = TRY_OR_THROW_OOM(vm, String::formatted("{}-{:02}", result, year_month.iso_day()));
    }

    // 8. Let calendarString be ! FormatCalendarAnnotation(calendarID, showCalendar).
    auto calendar_string = MUST_OR_THROW_OOM(format_calendar_annotation(vm, calendar_id, show_calendar));

    // 9. Set result to the string-concatenation of result and calendarString.
    // 10. Return result.
    return TRY_OR_THROW_OOM(vm, String::formatted("{}{}", result, calendar_string));
}

// 12.2.2 CreateCalendarMethodsRecord ( calendar, methods ), https://tc39.es/proposal-temporal/#sec-temporal-createcalendarmethodsrecord
ThrowCompletionOr<CalendarMethodsRecord> create_calendar_methods_record(VM& vm, Variant<String, NonnullGCPtr<Object>> const& calendar, Vector<CalendarMethod> const& methods)
{
    // 1. Let record be the Calendar Methods Record { [[Receiver]]: calendar, [[DateAdd]]: undefined, [[DateFromFields]]: undefined, [[DateUntil]]: undefined, [[Day]]: undefined, [[Fields]]: undefined, [[MergeFields]]: undefined, [[MonthDayFromFields]]: undefined, [[YearMonthFromFields]]: undefined }.
    CalendarMethodsRecord record { .receiver = calendar };

    // 2. For each element methodName in methods, do
    for (auto const& method_name : methods) {
        // a. Perform ? CalendarMethodsRecordLookup(record, methodName).
        TRY(calendar_methods_record_lookup(vm, record, method_name));
    }

    // 3. Return record.
    return record;
}

// 12.2.4 CalendarMethodsRecordLookup ( calendarRec, methodName ), https://tc39.es/proposal-temporal/#sec-temporal-calendarmethodsrecordlookup
ThrowCompletionOr<void> calendar_methods_record_lookup(VM& vm, CalendarMethodsRecord& calendar_record, CalendarMethod method_name)
{
    auto& realm = *vm.current_realm();

    // 1. Assert: CalendarMethodsRecordHasLookedUp(calendarRec, methodName) is false.
    // 2. If methodName is dateAdd, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[DateAdd]] to %Temporal.Calendar.prototype.dateAdd%.
    //     b. Else,
    //         i. Set calendarRec.[[DateAdd]] to ? GetMethod(calendarRec.[[Receiver]], "dateAdd").
    //         ii. If calendarRec.[[DateAdd]] is undefined, throw a TypeError exception.
    // 3. Else if methodName is dateFromFields, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[DateFromFields]] to %Temporal.TimeZone.prototype.dateFromFields%.
    //     b. Else,
    //     i. Set calendarRec.[[DateFromFields]] to ? GetMethod(calendarRec.[[Receiver]], "dateFromFields").
    //     ii. If calendarRec.[[DateFromFields]] is undefined, throw a TypeError exception.
    // 4. Else if methodName is dateUntil, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[DateUntil]] to %Temporal.TimeZone.prototype.dateUntil%.
    //     b. Else,
    //         i. Set calendarRec.[[DateUntil]] to ? GetMethod(calendarRec.[[Receiver]], "dateUntil").
    //         ii. If calendarRec.[[DateUntil]] is undefined, throw a TypeError exception.
    // 5. Else if methodName is day, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[Day]] to %Temporal.TimeZone.prototype.day%.
    //     b. Else,
    //         i. Set calendarRec.[[Day]] to ? GetMethod(calendarRec.[[Receiver]], "day").
    //         ii. If calendarRec.[[Day]] is undefined, throw a TypeError exception.
    // 6. Else if methodName is fields, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[Fields]] to %Temporal.TimeZone.prototype.fields%.
    //     b. Else,
    //         i. Set calendarRec.[[Fields]] to ? GetMethod(calendarRec.[[Receiver]], "fields").
    //         ii. If calendarRec.[[Fields]] is undefined, throw a TypeError exception.
    // 7. Else if methodName is mergeFields, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[MergeFields]] to %Temporal.TimeZone.prototype.mergeFields%.
    //     b. Else,
    //         i. Set calendarRec.[[MergeFields]] to ? GetMethod(calendarRec.[[Receiver]], "mergeFields").
    //         ii. If calendarRec.[[MergeFields]] is undefined, throw a TypeError exception.
    // 8. Else if methodName is monthDayFromFields, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[MonthDayFromFields]] to %Temporal.TimeZone.prototype.monthDayFromFields%.
    //     b. Else,
    //         i. Set calendarRec.[[MonthDayFromFields]] to ? GetMethod(calendarRec.[[Receiver]], "monthDayFromFields").
    //         ii. If calendarRec.[[MonthDayFromFields]] is undefined, throw a TypeError exception.
    // 9. Else if methodName is yearMonthFromFields, then
    //     a. If calendarRec.[[Receiver]] is a String, then
    //         i. Set calendarRec.[[YearMonthFromFields]] to %Temporal.TimeZone.prototype.yearMonthFromFields%.
    //     b. Else,
    //         i. Set calendarRec.[[YearMonthFromFields]] to ? GetMethod(calendarRec.[[Receiver]], "yearMonthFromFields").
    //         ii. If calendarRec.[[YearMonthFromFields]] is undefined, throw a TypeError exception.
    switch (method_name) {
#define __JS_ENUMERATE(PascalName, camelName, snake_name)                                                                                        \
    case CalendarMethod::PascalName: {                                                                                                           \
        VERIFY(!calendar_record.snake_name);                                                                                                     \
        if (calendar_record.receiver.has<String>()) {                                                                                            \
            Value prototype_value { realm.intrinsics().temporal_calendar_prototype() };                                                          \
            calendar_record.snake_name = MUST(prototype_value.get_method(vm, vm.names.camelName));                                               \
        } else {                                                                                                                                 \
            calendar_record.snake_name = TRY(Value { calendar_record.receiver.get<NonnullGCPtr<Object>>() }.get_method(vm, vm.names.camelName)); \
            if (!calendar_record.snake_name)                                                                                                     \
                return vm.throw_completion<TypeError>(ErrorType::IsUndefined, #camelName##sv);                                                   \
        }                                                                                                                                        \
    }
        JS_ENUMERATE_CALENDAR_METHODS
#undef __JS_ENUMERATE
    }

    // 10. Return unused.
    return {};
}

// 12.2.5 CalendarMethodsRecordHasLookedUp ( calendarRec, methodName ), https://tc39.es/proposal-temporal/#sec-temporal-calendarmethodsrecordhaslookedup
bool calendar_methods_record_has_looked_up(CalendarMethodsRecord const& calendar_record, CalendarMethod method_name)
{
    // 1. If methodName is DATE-ADD, then
    //     a. Let method be calendarRec.[[DateAdd]].
    // 2. Else if methodName is DATE-FROM-FIELDS, then
    //     a. Let method be calendarRec.[[DateFromFields]].
    // 3. Else if methodName is DATE-UNTIL, then
    //     a. Let method be calendarRec.[[DateUntil]].
    // 4. Else if methodName is DAY, then
    //     a. Let method be calendarRec.[[Day]].
    // 5. Else if methodName is FIELDS, then
    //     a. Let method be calendarRec.[[Fields]].
    // 6. Else if methodName is MERGE-FIELDS, then
    //     a. Let method be calendarRec.[[MergeFields]].
    // 7. Else if methodName is MONTH-DAY-FROM-FIELDS, then
    //     a. Let method be calendarRec.[[MonthDayFromFields]].
    // 8. Else if methodName is YEAR-MONTH-FROM-FIELDS, then
    //     a. Let method be calendarRec.[[YearMonthFromFields]].
    // 9. If method is undefined, return false.
    // 10. Return true.
    switch (method_name) {
#define __JS_ENUMERATE(PascalName, camelName, snake_name) \
    case CalendarMethod::PascalName: {                    \
        return calendar_record.snake_name != nullptr;     \
    }
        JS_ENUMERATE_CALENDAR_METHODS
#undef __JS_ENUMERATE
    }
}

// 12.2.6 CalendarMethodsRecordIsBuiltin ( calendarRec ), https://tc39.es/proposal-temporal/#sec-temporal-calendarmethodsrecordisbuiltin
bool calendar_methods_record_is_builtin(CalendarMethodsRecord const& calendar_record)
{
    // 1. If calendarRec.[[Receiver]] is a String, return true.
    if (calendar_record.receiver.has<String>())
        return true;

    // 2. Return false.
    return false;
}

// 12.2.7 CalendarMethodsRecordCall ( calendarRec, methodName, arguments ), https://tc39.es/proposal-temporal/#sec-temporal-calendarmethodsrecordcall
ThrowCompletionOr<Value> calendar_methods_record_call(VM& vm, CalendarMethodsRecord const& calendar_record, CalendarMethod method_name, Span<Value> arguments)
{
    // 1. Assert: CalendarMethodsRecordHasLookedUp(calendarRec, methodName) is true.
    VERIFY(calendar_methods_record_has_looked_up(calendar_record, method_name));

    // 2. Let receiver be calendarRec.[[Receiver]].
    // 3. If CalendarMethodsRecordIsBuiltin(calendarRec) is true, then
    //     a. Set receiver to ! CreateTemporalTimeZone(calendarRec.[[Receiver]]).
    GCPtr<Object> receiver;
    if (calendar_record.receiver.has<String>())
        receiver = MUST(create_temporal_time_zone(vm, calendar_record.receiver.get<String>()));
    else
        receiver = calendar_record.receiver.get<NonnullGCPtr<Object>>();

    // 4. If methodName is DATE-ADD, then
    //     a. Return ? Call(calendarRec.[[DateAdd]], receiver, arguments).
    // 5. If methodName is DATE-FROM-FIELDS, then
    //     a. Return ? Call(calendarRec.[[DateFromFields]], receiver, arguments).
    // 6. If methodName is DATE-UNTIL, then
    //     a. Return ? Call(calendarRec.[[DateUntil]], receiver, arguments).
    // 7. If methodName is DAY, then
    //     a. Return ? Call(calendarRec.[[Day]], receiver, arguments).
    // 8. If methodName is FIELDS, then
    //     a. Return ? Call(calendarRec.[[Fields]], receiver, arguments).
    // 9. If methodName is MERGE-FIELDS, then
    //     a. Return ? Call(calendarRec.[[MergeFields]], receiver, arguments).
    // 10. If methodName is MONTH-DAY-FROM-FIELDS, then
    //     a. Return ? Call(calendarRec.[[MonthDayFromFields]], receiver, arguments).
    // 11. If methodName is YEAR-MONTH-FROM-FIELDS, then
    //     a. Return ? Call(calendarRec.[[YearMonthFromFields]], receiver, arguments).
    switch (method_name) {
#define __JS_ENUMERATE(PascalName, camelName, snake_name)                      \
    case CalendarMethod::PascalName: {                                         \
        return TRY(call(vm, calendar_record.snake_name, receiver, arguments)); \
    }
        JS_ENUMERATE_CALENDAR_METHODS
#undef __JS_ENUMERATE
    }
}

// 9.5.7 DifferenceTemporalPlainYearMonth ( operation, yearMonth, other, options ), https://tc39.es/proposal-temporal/#sec-temporal-differencetemporalplainyearmonth
ThrowCompletionOr<Duration*> difference_temporal_plain_year_month(VM& vm, DifferenceOperation operation, PlainYearMonth& year_month, Value other_value, Value options_value)
{
    // 1. If operation is since, let sign be -1. Otherwise, let sign be 1.
    i8 sign = operation == DifferenceOperation::Since ? -1 : 1;

    // 2. Set other to ? ToTemporalYearMonth(other).
    auto* other = TRY(to_temporal_year_month(vm, other_value));

    // 3. Let calendar be yearMonth.[[Calendar]].
    auto& calendar = year_month.calendar();

    // 4. If ? CalendarEquals(calendar, other.[[Calendar]]) is false, throw a RangeError exception.
    if (!TRY(calendar_equals(vm, calendar, other->calendar())))
        return vm.throw_completion<RangeError>(ErrorType::TemporalDifferentCalendars);

    // 5. Let resolvedOptions be ? SnapshotOwnProperties(? GetOptionsObject(options), null).
    auto resolved_options = TRY(TRY(get_options_object(vm, options_value))->snapshot_own_properties(vm, nullptr));

    // 6. Let settings be ? GetDifferenceSettings(operation, resolvedOptions, date, « "week", "day" », "month", "year").
    auto settings = TRY(get_difference_settings(vm, operation, resolved_options, UnitGroup::Date, { "week"sv, "day"sv }, { "month"sv }, "year"sv));

    // 7. If yearMonth.[[ISOYear]] = other.[[ISOYear]] and yearMonth.[[ISOMonth]] = other.[[ISOMonth]] and yearMonth.[[ISODay]] = other.[[ISODay]], then
    if (year_month.iso_year() == other->iso_year() && year_month.iso_month() == other->iso_month() && year_month.iso_day() == other->iso_day()) {
        // a. Return ! CreateTemporalDuration(0, 0, 0, 0, 0, 0, 0, 0, 0, 0).
        return MUST(create_temporal_duration(vm, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    }

    // 8. Perform ! CreateDataPropertyOrThrow(resolvedOptions, "largestUnit", settings.[[LargestUnit]]).
    MUST(resolved_options->create_data_property_or_throw(vm.names.largestUnit, PrimitiveString::create(vm, settings.largest_unit)));

    // 9. Let calendarRec be ? CreateCalendarMethodsRecord(calendar, « dateAdd, dateFromFields, dateUntil, fields »).
    auto calendar_record = create_calendar_methods_record(vm, NonnullGCPtr<Object> { calendar }, Vector<CalendarMethod> { CalendarMethod::DateAdd, CalendarMethod::DateFromFields, CalendarMethod::DateUntil, CalendarMethod::Fields });

    // 10. Let fieldNames be ? CalendarFields(calendarRec, « "monthCode", "year" »).
    // FIXME: Pass through calendar record
    auto field_names = TRY(calendar_fields(vm, calendar, { "monthCode"sv, "year"sv }));

    // 11. Let thisFields be ? PrepareTemporalFields(yearMonth, fieldNames, «»).
    auto* this_fields = TRY(prepare_temporal_fields(vm, year_month, field_names, Vector<StringView> {}));

    // 12. Perform ! CreateDataPropertyOrThrow(thisFields, "day", 1𝔽).
    MUST(this_fields->create_data_property_or_throw(vm.names.day, Value(1)));

    // 13. Let thisDate be ? CalendarDateFromFields(calendarRec, thisFields).
    // FIXME: Pass through calendar record
    auto* this_date = TRY(calendar_date_from_fields(vm, calendar, *this_fields));

    // 14. Let otherFields be ? PrepareTemporalFields(other, fieldNames, «»).
    auto* other_fields = TRY(prepare_temporal_fields(vm, *other, field_names, Vector<StringView> {}));

    // 15. Perform ! CreateDataPropertyOrThrow(otherFields, "day", 1𝔽).
    MUST(other_fields->create_data_property_or_throw(vm.names.day, Value(1)));

    // 16. Let otherDate be ? CalendarDateFromFields(calendarRec, otherFields).
    // FIXME: Pass through calendar record
    auto* other_date = TRY(calendar_date_from_fields(vm, calendar, *other_fields));

    // 17. Perform ! CreateDataPropertyOrThrow(resolvedOptions, "largestUnit", settings.[[LargestUnit]]).
    MUST(resolved_options->create_data_property_or_throw(vm.names.largestUnit, PrimitiveString::create(vm, settings.largest_unit)));

    // 18. Let result be ? CalendarDateUntil(calendarRec, thisDate, otherDate, resolvedOptions).
    // FIXME: Pass through calendar record
    auto* duration = TRY(calendar_date_until(vm, calendar, this_date, other_date, *resolved_options));
    auto result = DurationRecord { duration->years(), duration->months(), 0, 0, 0, 0, 0, 0, 0, 0 };

    // 19. If settings.[[SmallestUnit]] is not "month" or settings.[[RoundingIncrement]] ≠ 1, then
    if (settings.smallest_unit != "month"sv || settings.rounding_increment != 1) {
        // a. Let roundRecord be ? RoundDuration(result.[[Years]], result.[[Months]], 0, 0, 0, 0, 0, 0, 0, 0, settings.[[RoundingIncrement]], settings.[[SmallestUnit]], settings.[[RoundingMode]], thisDate, calendarRec).
        // FIXME: Pass through calendar record
        auto round_record = TRY(round_duration(vm, result.years, result.months, 0, 0, 0, 0, 0, 0, 0, 0, settings.rounding_increment, settings.smallest_unit, settings.rounding_mode, this_date));

        // FIXME: b. Let roundResult be roundRecord.[[DurationRecord]].
        // FIXME: c. Set result to ? BalanceDateDurationRelative(roundResult.[[Years]], roundResult.[[Months]], 0, 0, settings.[[LargestUnit]], settings.[[SmallestUnit]], thisDate, calendarRec).
        result = round_record.duration_record;
    }

    // 20. Return ! CreateTemporalDuration(sign × result.[[Years]], sign × result.[[Months]], 0, 0, 0, 0, 0, 0, 0, 0).
    return MUST(create_temporal_duration(vm, sign * result.years, sign * result.months, 0, 0, 0, 0, 0, 0, 0, 0));
}

// 9.5.8 AddDurationToOrSubtractDurationFromPlainYearMonth ( operation, yearMonth, temporalDurationLike, options ), https://tc39.es/proposal-temporal/#sec-temporal-adddurationtoorsubtractdurationfromplainyearmonth
ThrowCompletionOr<PlainYearMonth*> add_duration_to_or_subtract_duration_from_plain_year_month(VM& vm, ArithmeticOperation operation, PlainYearMonth& year_month, Value temporal_duration_like, Value options_value)
{
    auto& realm = *vm.current_realm();

    realm.intrinsics().temporal_calendar_prototype();

    // 1. Let duration be ? ToTemporalDuration(temporalDurationLike).
    auto* duration = TRY(to_temporal_duration(vm, temporal_duration_like));

    // 2. If operation is subtract, then
    if (operation == ArithmeticOperation::Subtract) {
        // a. Set duration to ! CreateNegatedTemporalDuration(duration).
        duration = create_negated_temporal_duration(vm, *duration);
    }

    // 3. Let balanceResult be ? BalanceDuration(duration.[[Days]], duration.[[Hours]], duration.[[Minutes]], duration.[[Seconds]], duration.[[Milliseconds]], duration.[[Microseconds]], duration.[[Nanoseconds]], "day").
    auto balance_result = TRY(balance_duration(vm, duration->days(), duration->hours(), duration->minutes(), duration->seconds(), duration->milliseconds(), duration->microseconds(), Crypto::SignedBigInteger { duration->nanoseconds() }, "day"sv));

    // 4. Set options to ? GetOptionsObject(options).
    auto* options = TRY(get_options_object(vm, options_value));

    // 5. Let calendar be yearMonth.[[Calendar]].
    auto& calendar = year_month.calendar();

    // 6. Let fieldNames be ? CalendarFields(calendar, « "monthCode", "year" »).
    auto field_names = TRY(calendar_fields(vm, calendar, { "monthCode"sv, "year"sv }));

    // 7. Let fields be ? PrepareTemporalFields(yearMonth, fieldNames, «»).
    auto* fields = TRY(prepare_temporal_fields(vm, year_month, field_names, Vector<StringView> {}));

    // 8. Set sign to ! DurationSign(duration.[[Years]], duration.[[Months]], duration.[[Weeks]], balanceResult.[[Days]], 0, 0, 0, 0, 0, 0).
    auto sign = duration_sign(duration->years(), duration->months(), duration->weeks(), balance_result.days, 0, 0, 0, 0, 0, 0);

    double day;

    // 9. If sign < 0, then
    if (sign < 0) {
        // a. Let day be ? CalendarDaysInMonth(calendar, yearMonth).
        day = TRY(calendar_days_in_month(vm, calendar, year_month));
    }
    // 10. Else,
    else {
        // a. Let day be 1.
        day = 1;
    }

    // 11. Perform ! CreateDataPropertyOrThrow(fields, "day", 𝔽(day)).
    MUST(fields->create_data_property_or_throw(vm.names.day, Value(day)));

    // 12. Let date be ? CalendarDateFromFields(calendar, fields).
    auto* date = TRY(calendar_date_from_fields(vm, calendar, *fields));

    // 13. Let durationToAdd be ! CreateTemporalDuration(duration.[[Years]], duration.[[Months]], duration.[[Weeks]], balanceResult.[[Days]], 0, 0, 0, 0, 0, 0).
    auto* duration_to_add = MUST(create_temporal_duration(vm, duration->years(), duration->months(), duration->weeks(), balance_result.days, 0, 0, 0, 0, 0, 0));

    // 14. Let optionsCopy be OrdinaryObjectCreate(null).
    auto options_copy = Object::create(realm, nullptr);

    // 15. Let entries be ? EnumerableOwnPropertyNames(options, key+value).
    auto entries = TRY(options->enumerable_own_property_names(Object::PropertyKind::KeyAndValue));

    // 16. For each element entry of entries, do
    for (auto& entry : entries) {
        auto key = MUST(entry.as_array().get_without_side_effects(0).to_property_key(vm));
        auto value = entry.as_array().get_without_side_effects(1);

        // a. Perform ! CreateDataPropertyOrThrow(optionsCopy, entry[0], entry[1]).
        MUST(options_copy->create_data_property_or_throw(key, value));
    }

    // 17. Let addedDate be ? CalendarDateAdd(calendar, date, durationToAdd, options).
    auto* added_date = TRY(calendar_date_add(vm, calendar, date, *duration_to_add, options));

    // 18. Let addedDateFields be ? PrepareTemporalFields(addedDate, fieldNames, «»).
    auto* added_date_fields = TRY(prepare_temporal_fields(vm, *added_date, field_names, Vector<StringView> {}));

    // 19. Return ? CalendarYearMonthFromFields(calendar, addedDateFields, optionsCopy).
    return calendar_year_month_from_fields(vm, calendar, *added_date_fields, options_copy);
}

}
