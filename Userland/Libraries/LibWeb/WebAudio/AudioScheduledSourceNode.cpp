/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/AudioScheduledSourceNodePrototype.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/HTML/EventNames.h>
#include <LibWeb/WebAudio/AudioScheduledSourceNode.h>

namespace Web::WebAudio {

AudioScheduledSourceNode::~AudioScheduledSourceNode() = default;

JS::GCPtr<WebIDL::CallbackType> AudioScheduledSourceNode::onended()
{
    return event_handler_attribute(HTML::EventNames::ended);
}

void AudioScheduledSourceNode::set_onended(JS::GCPtr<WebIDL::CallbackType> value)
{
    set_event_handler_attribute(HTML::EventNames::ended, value);
}

WebIDL::ExceptionOr<void> AudioScheduledSourceNode::start(double when)
{
    (void)when;
    return WebIDL::NotSupportedError::create(realm(), "FIXME: Implement AudioScheduledSourceNode::start"_fly_string);
}

WebIDL::ExceptionOr<void> AudioScheduledSourceNode::stop(double when)
{
    (void)when;
    return WebIDL::NotSupportedError::create(realm(), "FIXME: Implement AudioScheduledSourceNode::stop"_fly_string);
}

void AudioScheduledSourceNode::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    WEB_SET_PROTOTYPE_FOR_INTERFACE(AudioScheduledSourceNode);
}

void AudioScheduledSourceNode::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
}

}
