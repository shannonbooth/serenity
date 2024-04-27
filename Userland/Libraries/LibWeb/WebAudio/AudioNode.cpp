/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebAudio/AudioNode.h>

namespace Web::WebAudio {

AudioNode::~AudioNode() = default;

// https://webaudio.github.io/web-audio-api/#dom-audionode-connect
WebIDL::ExceptionOr<JS::NonnullGCPtr<AudioNode>> AudioNode::connect(JS::NonnullGCPtr<AudioNode> destination_node, WebIDL::UnsignedLong output, WebIDL::UnsignedLong input)
{
    (void)destination_node;
    (void)output;
    (void)input;
    return WebIDL::NotSupportedError::create(realm(), "FIXME: Implement AudioNode::connect (AudioNode)"_fly_string);
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-connect-destinationparam-output
WebIDL::ExceptionOr<JS::NonnullGCPtr<AudioNode>> AudioNode::connect(JS::NonnullGCPtr<AudioParam> destination_param, WebIDL::UnsignedLong output)
{
    (void)destination_param;
    (void)output;
    return WebIDL::NotSupportedError::create(realm(), "FIXME: Implement AudioNode::connect (AudioParam)"_fly_string);
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect
void AudioNode::disconnect()
{
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect-output
void AudioNode::disconnect(WebIDL::UnsignedLong output)
{
    (void)output;
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect-destinationnode
void AudioNode::disconnect(JS::NonnullGCPtr<AudioNode> destination_node)
{
    (void)destination_node;
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect-destinationnode-output
void AudioNode::disconnect(JS::NonnullGCPtr<AudioNode> destination_node, WebIDL::UnsignedLong output)
{
    (void)destination_node;
    (void)output;
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect-destinationnode-output-input
void AudioNode::disconnect(JS::NonnullGCPtr<AudioNode> destination_node, WebIDL::UnsignedLong output, WebIDL::UnsignedLong input)
{
    (void)destination_node;
    (void)output;
    (void)input;
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect-destinationparam
void AudioNode::disconnect(JS::NonnullGCPtr<AudioParam> destination_param)
{
    (void)destination_param;
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-disconnect-destinationparam-output
void AudioNode::disconnect(JS::NonnullGCPtr<AudioParam> destination_param, WebIDL::UnsignedLong output)
{
    (void)destination_param;
    (void)output;
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-context
JS::NonnullGCPtr<BaseAudioContext> AudioNode::context() const
{
    TODO();
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-numberofinputs
WebIDL::UnsignedLong AudioNode::number_of_inputs() const
{
    TODO();
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-numberofoutputs
WebIDL::UnsignedLong AudioNode::number_of_outputs() const
{
    TODO();
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-channelcount
WebIDL::UnsignedLong AudioNode::channel_count() const
{
    TODO();
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-channelcount
void AudioNode::set_channel_count(WebIDL::UnsignedLong) const
{
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-channelcountmode
Bindings::ChannelCountMode AudioNode::channel_count_mode() const
{
    TODO();
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-channelcountmode
void AudioNode::set_channel_count_mode(Bindings::ChannelCountMode)
{
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-channelinterpretation
Bindings::ChannelInterpretation AudioNode::channel_interpretation() const
{
    TODO();
}

// https://webaudio.github.io/web-audio-api/#dom-audionode-channelinterpretation
void AudioNode::set_channel_interpretation(Bindings::ChannelInterpretation)
{
}

void AudioNode::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    WEB_SET_PROTOTYPE_FOR_INTERFACE(AudioNode);
}

void AudioNode::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
}

}
