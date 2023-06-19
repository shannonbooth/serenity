/*
 * Copyright (c) 2023, Shannon Booth <shannon.ml.booth@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/NativeFunction.h>
#include <LibWeb/Bindings/ByteLengthQueuingStrategyPrototype.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/HTML/WindowOrWorkerGlobalScope.h>
#include <LibWeb/Streams/ByteLengthQueuingStrategy.h>
#include <LibWeb/WebIDL/CallbackType.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::Streams {

// https://streams.spec.whatwg.org/#blqs-constructor
WebIDL::ExceptionOr<JS::NonnullGCPtr<ByteLengthQueuingStrategy>> ByteLengthQueuingStrategy::construct_impl(JS::Realm& realm, QueuingStrategyInit const& init)
{
    // The new ByteLengthQueuingStrategy(init) constructor steps are:
    // 1. Set this.[[highWaterMark]] to init["highWaterMark"].
    return MUST_OR_THROW_OOM(realm.heap().allocate<ByteLengthQueuingStrategy>(realm, realm, init.high_water_mark));
}

// https://streams.spec.whatwg.org/#blqs-internal-slots
// Instances of ByteLengthQueuingStrategy have a [[highWaterMark]] internal slot, storing the value given in the constructor.
ByteLengthQueuingStrategy::ByteLengthQueuingStrategy(JS::Realm& realm, double high_water_mark)
    : PlatformObject(realm)
    , m_high_water_mark(high_water_mark)
{
    auto& global_object = realm.global_object();
    auto& global_objects_relevant_realm = HTML::relevant_realm(global_object);

    // 1. Let steps be the following steps, given chunk:
    auto steps = [](auto& vm) -> JS::ThrowCompletionOr<JS::Value> {
        auto chunk = vm.argument(0);

        // 1. Return ? GetV(chunk, "byteLength").
        return chunk.get(vm, vm.names.byteLength);
    };

    // 2. Let F be ! CreateBuiltinFunction(steps, 1, "size", « », globalObject’s relevant Realm).
    auto function = JS::NativeFunction::create(global_objects_relevant_realm, move(steps), 0, "size", &global_objects_relevant_realm);

    // 3. Set globalObject’s byte length queuing strategy size function to a Function that represents a reference to F, with callback context equal to globalObject’s relevant settings object.
    auto& window_or_worker = dynamic_cast<HTML::WindowOrWorkerGlobalScopeMixin&>(global_object);
    window_or_worker.set_byte_length_queuing_strategy_size_function(global_object.heap().allocate_without_realm<WebIDL::CallbackType>(function, HTML::relevant_settings_object(global_object)));
}

// https://streams.spec.whatwg.org/#blqs-size
WebIDL::CallbackType* ByteLengthQueuingStrategy::size()
{
    auto& realm = this->realm();

    // 1.  Return this's relevant global object's count queuing strategy size function.
    auto& window_or_worker = dynamic_cast<HTML::WindowOrWorkerGlobalScopeMixin&>(realm.global_object());
    return window_or_worker.byte_length_queuing_strategy_size_function();
}

ByteLengthQueuingStrategy::~ByteLengthQueuingStrategy() = default;

JS::ThrowCompletionOr<void> ByteLengthQueuingStrategy::initialize(JS::Realm& realm)
{
    MUST_OR_THROW_OOM(Base::initialize(realm));
    set_prototype(&Bindings::ensure_web_prototype<Bindings::CountQueuingStrategyPrototype>(realm, "ByteLengthQueuingStrategy"));

    return {};
}

}
