/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Vector.h>
#include <LibJS/Forward.h>
#include <LibWeb/Bindings/PlatformObject.h>

namespace Web::WebAudio {

struct PeriodicWaveConstraints {
    bool disable_normalization { false };
};

// FIXME: Relocate

struct PeriodicWaveOptions : public PeriodicWaveConstraints {
    Optional<Vector<float>> real;
    Optional<Vector<float>> imag;
};

class PeriodicWave : public Bindings::PlatformObject {
    WEB_PLATFORM_OBJECT(PeriodicWave, Bindings::PlatformObject);
    JS_DECLARE_ALLOCATOR(PeriodicWave);

public:
    static WebIDL::ExceptionOr<JS::NonnullGCPtr<PeriodicWave>> construct_impl(JS::Realm&, JS::NonnullGCPtr<BaseAudioContext>, PeriodicWaveOptions const&);

    virtual ~PeriodicWave() override;

protected:
    virtual void initialize(JS::Realm&) override;
    virtual void visit_edges(Cell::Visitor&) override;
};

}
