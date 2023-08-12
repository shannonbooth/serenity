/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/DeprecatedFlyString.h>
#include <AK/String.h>
#include <LibWeb/DOM/ChildNode.h>
#include <LibWeb/DOM/Node.h>

namespace Web::DOM {

class DocumentType final
    : public Node
    , public ChildNode<DocumentType> {
    WEB_PLATFORM_OBJECT(DocumentType, Node);

public:
    static WebIDL::ExceptionOr<JS::NonnullGCPtr<DocumentType>> create(Document&);

    virtual ~DocumentType() override = default;

    virtual DeprecatedFlyString node_name() const override { return "#doctype"; }

    String const& name() const { return m_name; }
    void set_name(String const& name) { m_name = name; }

    String const& public_id() const { return m_public_id; }
    void set_public_id(String const& public_id) { m_public_id = public_id; }

    String const& system_id() const { return m_system_id; }
    void set_system_id(String const& system_id) { m_system_id = system_id; }

private:
    explicit DocumentType(Document&);

    virtual void initialize(JS::Realm&) override;

    String m_name;
    String m_public_id;
    String m_system_id;
};

template<>
inline bool Node::fast_is<DocumentType>() const { return is_document_type(); }

}
