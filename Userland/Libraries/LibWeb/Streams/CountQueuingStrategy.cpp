/*
 * Copyright (c) 2023, Shannon Booth <shannon.ml.booth@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/NativeFunction.h>
#include <LibWeb/Bindings/CountQueuingStrategyPrototype.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/HTML/WindowOrWorkerGlobalScope.h>
#include <LibWeb/Streams/CountQueuingStrategy.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::Streams {

// https://streams.spec.whatwg.org/#blqs-constructor
WebIDL::ExceptionOr<JS::NonnullGCPtr<CountQueuingStrategy>> CountQueuingStrategy::construct_impl(JS::Realm& realm, QueuingStrategyInit const& init)
{
    // The new CountQueuingStrategy(init) constructor steps are:
    // 1. Set this.[[highWaterMark]] to init["highWaterMark"].
    return MUST_OR_THROW_OOM(realm.heap().allocate<CountQueuingStrategy>(realm, realm, init.high_water_mark));
}

// https://streams.spec.whatwg.org/#cqs-internal-slots
// Instances of CountQueuingStrategy have a [[highWaterMark]] internal slot, storing the value given in the constructor.
CountQueuingStrategy::CountQueuingStrategy(JS::Realm& realm, double high_water_mark)
    : PlatformObject(realm)
    , m_high_water_mark(high_water_mark)
{
    // Additionally, every global object globalObject has an associated count queuing strategy size function, which is a Function whose value must be initialized as follows:
    auto& global_object = realm.global_object();
    auto& global_objects_relevant_realm = HTML::relevant_realm(global_object);

    // 1. Let steps be the following steps:
    auto steps = [](auto const&) -> JS::ThrowCompletionOr<JS::Value> {
        // 1. Return 1.
        return JS::Value(1);
    };

    // 2. Let F be ! CreateBuiltinFunction(steps, 0, "size", « », globalObject’s relevant Realm).
    auto function = JS::NativeFunction::create(global_objects_relevant_realm, move(steps), 0, "size", &global_objects_relevant_realm);

    // 3. Set globalObject’s count queuing strategy size function to a Function that represents a reference to F, with callback context equal to globalObject’s relevant settings object.
    auto& window_or_worker = dynamic_cast<HTML::WindowOrWorkerGlobalScopeMixin&>(global_object);
    window_or_worker.set_count_queuing_strategy_size_function(global_object.heap().allocate_without_realm<WebIDL::CallbackType>(function, HTML::relevant_settings_object(global_object)));
}

// https://streams.spec.whatwg.org/#cqs-size
WebIDL::CallbackType* CountQueuingStrategy::size()
{
    auto& realm = this->realm();

    // 1.  Return this's relevant global object's count queuing strategy size function.
    auto& window_or_worker = dynamic_cast<HTML::WindowOrWorkerGlobalScopeMixin&>(realm.global_object());
    return window_or_worker.count_queuing_strategy_size_function();
}

CountQueuingStrategy::~CountQueuingStrategy() = default;

JS::ThrowCompletionOr<void> CountQueuingStrategy::initialize(JS::Realm& realm)
{
    MUST_OR_THROW_OOM(Base::initialize(realm));
    set_prototype(&Bindings::ensure_web_prototype<Bindings::CountQueuingStrategyPrototype>(realm, "CountQueuingStrategy"));

    return {};
}

}
