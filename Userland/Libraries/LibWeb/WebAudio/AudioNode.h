/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Forward.h>
#include <LibWeb/Bindings/AudioNodePrototype.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::WebAudio {

class AudioNode : public DOM::EventTarget {
    WEB_PLATFORM_OBJECT(AudioNode, DOM::EventTarget);
    JS_DECLARE_ALLOCATOR(AudioNode);

public:
    virtual ~AudioNode() override;

    WebIDL::ExceptionOr<JS::NonnullGCPtr<AudioNode>> connect(JS::NonnullGCPtr<AudioNode> destination_node, WebIDL::UnsignedLong output = 0, WebIDL::UnsignedLong input = 0);
    WebIDL::ExceptionOr<JS::NonnullGCPtr<AudioNode>> connect(JS::NonnullGCPtr<AudioParam> destination_param, WebIDL::UnsignedLong output = 0);

    void disconnect();
    void disconnect(WebIDL::UnsignedLong output);
    void disconnect(JS::NonnullGCPtr<AudioNode> destination_node);
    void disconnect(JS::NonnullGCPtr<AudioNode> destination_node, WebIDL::UnsignedLong output);
    void disconnect(JS::NonnullGCPtr<AudioNode> destination_node, WebIDL::UnsignedLong output, WebIDL::UnsignedLong input);
    void disconnect(JS::NonnullGCPtr<AudioParam> destination_param);
    void disconnect(JS::NonnullGCPtr<AudioParam> destination_param, WebIDL::UnsignedLong output);

    JS::NonnullGCPtr<BaseAudioContext> context() const;
    WebIDL::UnsignedLong number_of_inputs() const;
    WebIDL::UnsignedLong number_of_outputs() const;

    WebIDL::UnsignedLong channel_count() const;
    void set_channel_count(WebIDL::UnsignedLong) const;

    Bindings::ChannelCountMode channel_count_mode() const;
    void set_channel_count_mode(Bindings::ChannelCountMode);

    Bindings::ChannelInterpretation channel_interpretation() const;
    void set_channel_interpretation(Bindings::ChannelInterpretation);

protected:
    virtual void initialize(JS::Realm&) override;
    virtual void visit_edges(Cell::Visitor&) override;
};

}
