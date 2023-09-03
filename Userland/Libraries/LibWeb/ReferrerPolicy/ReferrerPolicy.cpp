/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/ReferrerPolicy/ReferrerPolicy.h>

namespace Web::ReferrerPolicy {

String to_string(ReferrerPolicy referrer_policy)
{
    switch (referrer_policy) {
    case ReferrerPolicy::EmptyString:
        return ""_string;
    case ReferrerPolicy::NoReferrer:
        return "no-referrer"_string;
    case ReferrerPolicy::NoReferrerWhenDowngrade:
        return "no-referrer-when-downgrade"_string;
    case ReferrerPolicy::SameOrigin:
        return "same-origin"_string;
    case ReferrerPolicy::Origin:
        return "origin"_string;
    case ReferrerPolicy::StrictOrigin:
        return "strict-origin"_string;
    case ReferrerPolicy::OriginWhenCrossOrigin:
        return "origin-when-cross-origin"_string;
    case ReferrerPolicy::StrictOriginWhenCrossOrigin:
        return "strict-origin-when-cross-origin"_string;
    case ReferrerPolicy::UnsafeURL:
        return "unsafe-url"_string;
    }
    VERIFY_NOT_REACHED();
}

Optional<ReferrerPolicy> from_string(StringView string)
{
    if (string.equals_ignoring_ascii_case("no-referrer"sv))
        return ReferrerPolicy::NoReferrer;
    if (string.equals_ignoring_ascii_case("no-referrer-when-downgrade"sv))
        return ReferrerPolicy::NoReferrerWhenDowngrade;
    if (string.equals_ignoring_ascii_case("same-origin"sv))
        return ReferrerPolicy::SameOrigin;
    if (string.equals_ignoring_ascii_case("origin"sv))
        return ReferrerPolicy::Origin;
    if (string.equals_ignoring_ascii_case("strict-origin"sv))
        return ReferrerPolicy::StrictOrigin;
    if (string.equals_ignoring_ascii_case("origin-when-cross-origin"sv))
        return ReferrerPolicy::OriginWhenCrossOrigin;
    if (string.equals_ignoring_ascii_case("strict-origin-when-cross-origin"sv))
        return ReferrerPolicy::StrictOriginWhenCrossOrigin;
    if (string.equals_ignoring_ascii_case("unsafe-url"sv))
        return ReferrerPolicy::UnsafeURL;
    return {};
}

}
