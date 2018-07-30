/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_URL_ATTRIBUTE_H_
#define _U2_URL_ATTRIBUTE_H_

#include <U2Lang/Attribute.h>
#include <U2Lang/Dataset.h>

namespace U2 {

class U2LANG_EXPORT URLAttribute : public Attribute {
public:
    URLAttribute(const Descriptor &d, const DataTypePtr type, bool required = false);

    virtual void setAttributeValue(const QVariant &newVal);
    virtual const QVariant & getAttributePureValue() const;
    virtual const QVariant & getDefaultPureValue() const;
    virtual bool isDefaultValue() const;
    virtual bool isEmpty() const;
    virtual URLAttribute *clone();
    virtual bool validate(ProblemList &problemList);

    virtual const QSet<GObjectType> & getCompatibleObjectTypes() const;
    virtual void setCompatibleObjectTypes(const QSet<GObjectType> &types);

    QList<Dataset> & getDatasets();
    void updateValue();

private:
    URLAttribute(const URLAttribute &other);
    URLAttribute &operator=(const URLAttribute &other);

    QStringList emptyDatasetNames(bool &hasUrls);
    void copy(const URLAttribute &other);

    QList<Dataset> sets;
    QSet<GObjectType> compatibleObjectTypes;
};

} // U2

#endif // _U2_URL_ATTRIBUTE_H_
