/*
 * Copyright (c) 2023, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteBuffer.h>
#include <LibJS/Forward.h>
#include <LibJS/Heap/Heap.h>
#include <LibJS/Runtime/Promise.h>
#include <LibJS/Runtime/Realm.h>
#include <LibJS/Runtime/TypedArray.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/FileAPI/Blob.h>
#include <LibWeb/FileAPI/FileReader.h>
#include <LibWeb/HTML/EventLoop/EventLoop.h>
#include <LibWeb/HTML/EventNames.h>
#include <LibWeb/Streams/ReadableStream.h>
#include <LibWeb/Streams/ReadableStreamDefaultReader.h>
#include <LibWeb/WebIDL/CallbackType.h>
#include <LibWeb/WebIDL/DOMException.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::FileAPI {

FileReader::~FileReader() = default;

FileReader::FileReader(JS::Realm& realm)
    : DOM::EventTarget(realm)
{
}

void FileReader::initialize(JS::Realm& realm)
{
    DOM::EventTarget::initialize(realm);
    set_prototype(&Bindings::ensure_web_prototype<Bindings::FileReaderPrototype>(realm, "FileReader"));
}

JS::NonnullGCPtr<FileReader> FileReader::create(JS::Realm& realm)
{
    return realm.heap().allocate<FileReader>(realm, realm);
}

WebIDL::ExceptionOr<JS::NonnullGCPtr<FileReader>> FileReader::construct_impl(JS::Realm& realm)
{
    return FileReader::create(realm);
}

// https://w3c.github.io/FileAPI/#readOperation
WebIDL::ExceptionOr<void> FileReader::read_operation(Blob& blob, Type type, Optional<String> const& encoding_name)
{
    auto& realm = this->realm();

    // 1. If fr’s state is "loading", throw an InvalidStateError DOMException.
    if (m_state == State::Loading)
        WebIDL::InvalidStateError::create(realm, "Loading"); // FIXME:

    // 2. Set fr’s state to "loading".
    m_state = State::Loading;

    // 3. Set fr’s result to null.
    m_result = {};

    // 4. Set fr’s error to null.
    m_error = {};

    // 5. Let stream be the result of calling get stream on blob.
    auto stream = TRY(blob.get_stream());

    // 6. Let reader be the result of getting a reader from stream.
    auto reader = TRY(stream->get_reader());

    // 7. Let bytes be an empty byte sequence.
    ByteBuffer bytes;

    // 8. Let chunkPromise be the result of reading a chunk from stream with reader.
    auto chunk_promise = TRY(reader.get<JS::NonnullGCPtr<Streams::ReadableStreamDefaultReader>>()->read());

    // 9. Let isFirstChunk be true.
    bool is_first_chunk = true;

    // FIXME: 10. In parallel, while true:
    while (true) {
        // 1. Wait for chunkPromise to be fulfilled or rejected.
        HTML::main_thread_event_loop().spin_until([&]() {
            return chunk_promise->state() == JS::Promise::State::Fulfilled || chunk_promise->state() == JS::Promise::State::Rejected;
        });

        // 2. If chunkPromise is fulfilled, and isFirstChunk is true, queue a task to fire a progress event called loadstart at fr.
        if (chunk_promise->state() == JS::Promise::State::Fulfilled && is_first_chunk) {
            HTML::queue_global_task(HTML::Task::Source::FileReading, realm.global_object(), []() {
                // Fire a progress event here!
            });
        }

        // NOTE: ISSUE 2 We might change loadstart to be dispatched synchronously, to align with XMLHttpRequest behavior. [Issue #119]

        // 3. Set isFirstChunk to false.
        is_first_chunk = false;

        // 4. If chunkPromise is fulfilled with an object whose done property is false and whose value property is a Uint8Array object, run these steps:
        if (chunk_promise->state() == JS::Promise::State::Fulfilled && chunk_promise->result().is_object() && is<JS::Uint8Array>(chunk_promise->result().as_object())) {
            // 1. Let bs be the byte sequence represented by the Uint8Array object.
            auto& byte_sequence = verify_cast<JS::Uint8Array>(chunk_promise->result().as_object());

            // 2. Append bs to bytes.
            bytes.append(byte_sequence.data());

            // FIXME: 3. If roughly 50ms have passed since these steps were last invoked, queue a task to fire a progress event called progress at fr.

            // 4. Set chunkPromise to the result of reading a chunk from stream with reader.
        }
        // 5. Otherwise, if chunkPromise is fulfilled with an object whose done property is true, queue a task to run the following steps and abort this algorithm:
        else if (chunk_promise->state() == JS::Promise::State::Fulfilled && chunk_promise->result().is_object()) {

            HTML::queue_global_task(HTML::Task::Source::FileReading, realm.global_object(), [this]() {
                // 1. Set fr’s state to "done".
                m_state = State::Done;

                // 2. Let result be the result of package data given bytes, type, blob’s type, and encodingName.

                // 3. If package data threw an exception error:
                if (false) {

                    // 1. Set fr’s error to error.

                    // 2. Fire a progress event called error at fr.

                }
                // 4. Else:
                else {

                    // 1. Set fr’s result to result.

                    // 2. Fire a progress event called load at the fr.
                }

                // 5. If fr’s state is not "loading", fire a progress event called loadend at the fr.

                // NOTE: Event handler for the load or error events could have started another load, if that happens the loadend event for this load is not fired.
            });

            return {};
        }
        // 6. Otherwise, if chunkPromise is rejected with an error error, queue a task to run the following steps and abort this algorithm:
        else {

            // 1. Set fr’s state to "done".

            // 2. Set fr’s error to error.

            // 5. Fire a progress event called error at fr.

            // 4. If fr’s state is not "loading", fire a progress event called loadend at fr.

            // 5. Note: Event handler for the error event could have started another load, if that happens the loadend event for this load is not fired.
        }
    }

    return {};
}

void FileReader::read_as_array_buffer(Blob&)
{
}

void FileReader::read_as_binary_string(Blob&)
{
}

void FileReader::read_as_text(Blob&, Optional<String> const&)
{
}

void FileReader::read_as_data_url(Blob&)
{
}

// https://w3c.github.io/FileAPI/#dfn-abort
void FileReader::abort()
{
    // 1. If this's state is "empty" or if this's state is "done" set this's result to null and terminate this algorithm.
    if (m_state == State::Empty || m_state == State::Done) {
        m_result = {};
        return;
    }

    // 2. If this's state is "loading" set this's state to "done" and set this's result to null.
    if (m_state == State::Loading) {
        m_state = State::Done;
        m_result = {};
    }

    // FIXME: 3. If there are any tasks from this on the file reading task source in an affiliated task queue, then remove those tasks from that task queue.

    // FIXME: 4. Terminate the algorithm for the read method being processed.

    // FIXME: 5. Fire a progress event called abort at this.

    // FIXME: 6. If this's state is not "loading", fire a progress event called loadend at this.
}

void FileReader::set_onloadstart(WebIDL::CallbackType* value)
{
    set_event_handler_attribute(HTML::EventNames::loadstart, value);
}

WebIDL::CallbackType* FileReader::onloadstart()
{
    return event_handler_attribute(HTML::EventNames::loadstart);
}

void FileReader::set_onprogress(WebIDL::CallbackType* value)
{
    set_event_handler_attribute(HTML::EventNames::progress, value);
}

WebIDL::CallbackType* FileReader::onprogress()
{
    return event_handler_attribute(HTML::EventNames::progress);
}

void FileReader::set_onload(WebIDL::CallbackType* value)
{
    set_event_handler_attribute(HTML::EventNames::load, value);
}

WebIDL::CallbackType* FileReader::onload()
{
    return event_handler_attribute(HTML::EventNames::load);
}

void FileReader::set_onabort(WebIDL::CallbackType* value)
{
    set_event_handler_attribute(HTML::EventNames::abort, value);
}

WebIDL::CallbackType* FileReader::onabort()
{
    return event_handler_attribute(HTML::EventNames::abort);
}

void FileReader::set_onerror(WebIDL::CallbackType* value)
{
    set_event_handler_attribute(HTML::EventNames::error, value);
}

WebIDL::CallbackType* FileReader::onerror()
{
    return event_handler_attribute(HTML::EventNames::error);
}

void FileReader::set_onloadend(WebIDL::CallbackType* value)
{
    set_event_handler_attribute(HTML::EventNames::loadend, value);
}

WebIDL::CallbackType* FileReader::onloadend()
{
    return event_handler_attribute(HTML::EventNames::loadend);
}

}
