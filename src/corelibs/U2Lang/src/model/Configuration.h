/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_WORKFLOW_CONFIGURATION_H_
#define _U2_WORKFLOW_CONFIGURATION_H_

#include "Attribute.h"
#include "ConfigurationValidator.h"

namespace U2 {

class ConfigurationEditor;

/**
 * Set of named attributes
 * additionally, has editor and can be validated
 */
class U2LANG_EXPORT Configuration {
public:
    Configuration();
    virtual ~Configuration();

    virtual QMap<QString, Attribute*> getParameters() const;
    // returns NULL if no parameter identified with 'name'
    virtual Attribute* getParameter(const QString& name) const;

    // returns removed parameter
    // caller takes ownership of the attribute
    virtual Attribute * removeParameter( const QString & name );
    virtual void addParameter( const QString & name, Attribute * attr );

    // sets if only parameter with 'name' exists
    virtual void setParameter(const QString& name, const QVariant& val);
    void setParameters(const QVariantMap&);
    QVariantMap getValues() const;
    virtual bool hasParameter(const QString & name) const;

    // getters/setters
    virtual ConfigurationEditor* getEditor() const;
    void setEditor(ConfigurationEditor* ed);
    virtual ConfigurationValidator* getValidator();
    void setValidator(ConfigurationValidator* v);

    // returns 'false' if validations fails, otherwise 'true'
    // if validation fails, new errors appended to errorList
    virtual bool validate(NotificationsList& notificationList) const;

    // when schema copied, actors in new schema get new id's
    // in this case, we need to remap all links between actors
    // see Port realizations
    virtual void remap(const QMap<ActorId, ActorId>&) = 0;

    virtual QList<Attribute*> getAttributes() const;

    bool isAttributeVisible(Attribute *attribute) const;

protected:
    // controller for ui editor of this configuration
    ConfigurationEditor* editor;
    // standard validations if required attribute is empty are done in validate() function
    // this validator can make any non-trivial validations
    ConfigurationValidator* validator;

private:
    // named attributes
    QMap<QString, Attribute*> params;
    QList<Attribute *> attrs; //For sorting attributes by order of creation

}; // Configuration

} // U2

#endif // _U2_WORKFLOW_CONFIGURATION_H_
