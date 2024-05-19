/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/HighResolutionTime/EventNames.h>

namespace Web::HighResolutionTime::EventNames {

#define __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT(name) FlyString name;
ENUMERATE_HIGH_RESOLUTION_TIME_EVENTS
#undef __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT

void initialize_strings()
{
    static bool s_initialized = false;
    VERIFY(!s_initialized);

#define __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT(name) \
    name = #name##_fly_string;
    ENUMERATE_HIGH_RESOLUTION_TIME_EVENTS
#undef __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT

    s_initialized = true;
}

}
