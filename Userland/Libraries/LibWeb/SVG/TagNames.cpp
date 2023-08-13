/*
 * Copyright (c) 2020, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/SVG/TagNames.h>

namespace Web::SVG::TagNames {

#define TO_STRING_IMPL(s) #s
#define TO_STRING(x) TO_STRING_IMPL(x)

#define __ENUMERATE_SVG_TAG(name) FlyString name;
ENUMERATE_SVG_TAGS
#undef __ENUMERATE_SVG_TAG

void initialize_strings()
{
    static bool s_initialized = false;
    VERIFY(!s_initialized);

#define __ENUMERATE_SVG_TAG(name) name = TO_STRING(name) ""_fly_string;
    ENUMERATE_SVG_TAGS
#undef __ENUMERATE_SVG_TAG

    s_initialized = true;
}

}
