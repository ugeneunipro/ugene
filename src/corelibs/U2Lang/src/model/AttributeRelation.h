/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <QVariant>

#include <U2Core/global.h>

namespace U2 {

class DelegateTags;

enum RelationType {
    VISIBILITY,
    FILE_EXTENSION,
    CUSTOM_VALUE_CHANGER,
    PROPERTY_CHANGER
};

/**
 * An abstract class describing any relations between influencing and dependent attributes
 */
class U2LANG_EXPORT AttributeRelation {
public:
    AttributeRelation(const QString& relatedAttrId)
        : relatedAttrId(relatedAttrId) {
    }
    virtual ~AttributeRelation() {
    }

    /**
     * Updates tags of delegates
     */
    virtual QVariant getAffectResult(const QVariant& influencingValue, const QVariant& dependentValue, DelegateTags* infTags = nullptr, DelegateTags* depTags = nullptr) const = 0;
    virtual RelationType getType() const = 0;
    virtual void updateDelegateTags(const QVariant& influencingValue, DelegateTags* dependentTags) const;
    QString getRelatedAttrId() const {
        return relatedAttrId;
    }

    /**
     * Some relations changes value of the dependent attribute,
     * other relations changes only some properties of attributes (e.g. visibility)
     */
    virtual bool valueChangingRelation() const {
        return true;
    }

    virtual AttributeRelation* clone() const = 0;

protected:
    QString relatedAttrId;
};

/**
 * It describes how visibility of some attribute depends on a value of the related attribute
 */
class U2LANG_EXPORT VisibilityRelation : public AttributeRelation {
public:
    VisibilityRelation(const QString& relatedAttrId, const QVariantList& visibilityValues, bool invertVisibilityRules = false);
    VisibilityRelation(const QString& relatedAttrId, const QVariant& visibilityValue, bool invertVisibilityRules = false);

    QVariant getAffectResult(const QVariant& influencingValue, const QVariant& dependentValue, DelegateTags* infTags, DelegateTags* depTags) const override;
    RelationType getType() const override {
        return VISIBILITY;
    }
    bool valueChangingRelation() const override {
        return false;
    }

    VisibilityRelation* clone() const override;

private:
    QVariantList visibilityValues;
    bool invertAffectResult;
};

/**
 * A value of URL_OUT_ATTRIBUTE depends on a value of DOCUMENT_FORMAT_ATTRIBUTE
 */
class U2LANG_EXPORT FileExtensionRelation : public AttributeRelation {
public:
    FileExtensionRelation(const QString& relatedAttrId)
        : AttributeRelation(relatedAttrId) {
    }

    QVariant getAffectResult(const QVariant& influencingValue, const QVariant& dependentValue, DelegateTags* infTags, DelegateTags* depTags) const override;
    void updateDelegateTags(const QVariant& influencingValue, DelegateTags* dependentTags) const override;
    RelationType getType() const override {
        return FILE_EXTENSION;
    }

    FileExtensionRelation* clone() const override;
};

/**
 * Possible values of one attribute depends on current value of another
 */
class U2LANG_EXPORT ValuesRelation : public AttributeRelation {
public:
    ValuesRelation(const QString& relatedAttrId, const QVariantMap& _dependencies)
        : AttributeRelation(relatedAttrId), dependencies(_dependencies) {
    }
    RelationType getType() const override {
        return CUSTOM_VALUE_CHANGER;
    }

    QVariant getAffectResult(const QVariant& influencingValue, const QVariant& dependentValue, DelegateTags* infTags, DelegateTags* depTags) const override;
    void updateDelegateTags(const QVariant& influencingValue, DelegateTags* dependentTags) const override;

    ValuesRelation* clone() const override;

private:
    QVariantMap dependencies;
};

}  // namespace U2
