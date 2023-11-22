/*
 * Copyright (c) 2023, Jonah Shafran <jonahshafran@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/CSS/CSSRule.h>

namespace Web::CSS {

class CSSNamespaceRule final : public CSSRule {
    WEB_PLATFORM_OBJECT(CSSNamespaceRule, CSSRule);
    JS_DECLARE_ALLOCATOR(CSSNamespaceRule);

public:
    [[nodiscard]] static JS::NonnullGCPtr<CSSNamespaceRule> create(JS::Realm&, Optional<String> const& prefix, String namespace_uri);

    virtual ~CSSNamespaceRule() = default;

    void set_namespace_uri(String value) { m_namespace_uri = move(value); }
    String namespace_uri() const { return m_namespace_uri; }
    void set_prefix(String value) { m_prefix = move(value); }
    String prefix() const { return m_prefix; }
    virtual Type type() const override { return Type::Namespace; }

private:
    CSSNamespaceRule(JS::Realm&, Optional<String> const& prefix, String namespace_uri);

    virtual void initialize(JS::Realm&) override;

    virtual String serialized() const override;
    String m_namespace_uri;
    String m_prefix;
};

}
