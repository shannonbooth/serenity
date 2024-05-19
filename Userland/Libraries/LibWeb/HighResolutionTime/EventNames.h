/*
 * Copyright (c) 2024, Shannon Booth <shannon@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/FlyString.h>

namespace Web::HighResolutionTime::EventNames {

#define ENUMERATE_HIGH_RESOLUTION_TIME_EVENTS \
    __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT(resourcetimingbufferfull)

#define __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT(name) extern FlyString name;
ENUMERATE_HIGH_RESOLUTION_TIME_EVENTS
#undef __ENUMERATE_HIGH_RESOLUTION_TIME_EVENT

void initialize_strings();

}
