/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Bindings/OscillatorNodePrototype.h>
#include <LibWeb/WebAudio/AudioScheduledSourceNode.h>

namespace Web::WebAudio {

struct OscillatorOptions : public AudioNodeOptions {
    Bindings::OscillatorType type { Bindings::OscillatorType::Sine };
    float frequency { 440 };
    float detune { 0 };
    JS::GCPtr<PeriodicWave> periodic_wave;
};

class OscillatorNode : public AudioScheduledSourceNode {
    WEB_PLATFORM_OBJECT(OscillatorNode, AudioScheduledSourceNode);
    JS_DECLARE_ALLOCATOR(OscillatorNode);

public:
    virtual ~OscillatorNode() override;

    static WebIDL::ExceptionOr<JS::NonnullGCPtr<OscillatorNode>> create(JS::Realm&, JS::NonnullGCPtr<BaseAudioContext>, OscillatorOptions const& = {});
    static WebIDL::ExceptionOr<JS::NonnullGCPtr<OscillatorNode>> construct_impl(JS::Realm&, JS::NonnullGCPtr<BaseAudioContext>, OscillatorOptions const& = {});

protected:
    virtual void initialize(JS::Realm&) override;
    virtual void visit_edges(Cell::Visitor&) override;
};

}
