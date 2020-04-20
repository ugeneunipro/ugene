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

#ifndef _U2_ACTORPROTOTYPE_H_
#define _U2_ACTORPROTOTYPE_H_

#include <U2Lang/Descriptor.h>
#include <U2Lang/Port.h>

#include <QMimeData>
#include <QObject>

namespace U2 {
    class Attribute;
    class AttributeScript;
    class ConfigurationEditor;
    class ConfigurationValidator;
namespace Workflow {

class Prompter;

/**
 * Actor represents particular semantic template
 * and can be configured via set of template-specific parameters
 * ActorPrototype is such a template
 */
class U2LANG_EXPORT ActorPrototype : public QObject, public VisualDescriptor {
    Q_OBJECT
public:
    ActorPrototype(const Descriptor& desc,
        const QList<PortDescriptor*>& ports = QList<PortDescriptor*>(),
        const QList<Attribute*>& attrs = QList<Attribute*>());
    virtual ~ActorPrototype();

    QList<PortDescriptor*> getPortDesciptors() const;

    QList<Attribute*> getAttributes() const;
    Attribute * getAttribute( const QString & id ) const;
    void addAttribute( Attribute * a );
    int removeAttribute( Attribute * attr );

    void setEditor(ConfigurationEditor* e);
    ConfigurationEditor * getEditor() const;

    // validator has setter but no getter
    void setValidator(ConfigurationValidator* v);

    // prompter has no getter
    void setPrompter(Prompter* p);

    // port validators has no getters
    // all validators will be used as validator in corresponding port
    // see createInstance()
    void setPortValidator(const QString& id, ConfigurationValidator* v);

    // for drag'n'drop purposes
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap* = NULL) const;

    virtual Actor* createInstance(const ActorId &actorId, AttributeScript *script = NULL, const QVariantMap& params = QVariantMap());

    void setDisplayName(const QString& n) override;

    void setDocumentation(const QString& d) override;

    void setScriptFlag(bool flag = true);
    bool isScriptFlagSet() {return isScript;}
    void setSchema(const QString &path);
    void setNonStandard(const QString &path);
    bool isStandardFlagSet() {return isStandard;}
    bool isSchemaFlagSet() {return isSchema;}
    bool isExternalTool() {return !isStandard && !isSchema && !isScript;}
    QString getFilePath() {return actorFilePath;}

    bool isAllowsEmptyPorts() const {return allowsEmptyPorts;}
    void setAllowsEmptyPorts(bool value) {allowsEmptyPorts = value;}

    bool getInfluenceOnPathFlag() const {return influenceOnPathFlag;}
    void setInfluenceOnPathFlag(bool value) {influenceOnPathFlag = value;}

    void addExternalTool(const QString &toolId, const QString &paramId = "");
    const StrStrMap & getExternalTools() const;
    void clearExternalTools();
    int getUsageCounter() const;

signals:
    void si_nameChanged();
    void si_descriptionChanged();

protected:
    // create port and sets p as owner of new port
    // caller should add created port to actor's ports see createInstance
    virtual Port* createPort(const PortDescriptor& d, Actor* p);

protected:
    // list of attributes
    // can be changed via addAttribute and removeAttribute
    QList<Attribute*> attrs;
    // list of port's
    // real Port's are created via createPort
    QList<PortDescriptor*> ports;
    // controller for actor's configuration editor
    ConfigurationEditor* ed;
    // makes any non-trivial validations
    ConfigurationValidator* val;
    // some realization of Prompter (e.g. PrompterBaseImpl)
    Prompter* prompter;
    // as if each port is configuration
    // we need port validators
    // QString here - id of corresponding PortDescriptor
    QMap<QString, ConfigurationValidator*> portValidators;
    // actor can be written on a script by user
    // in such case user can define attributes and input, output ports of actor
    bool isScript;
    // actor can be a standard actor or external tool or script actor included from file
    bool isStandard;
    // if actor is non standard then its meta is kept in actorFilePath
    QString actorFilePath;
    // actor can implement some big schema
    bool isSchema;

    bool allowsEmptyPorts;
    bool influenceOnPathFlag;

    // toolId <-> parameterId (optional)
    // The actor could use external tools. The map shows what tools are used.
    // Also the path to a tool can be set in a parameter. In this case the value of map is the parameter's id;
    // otherwise the value is empty string.
    StrStrMap externalTools;

private slots:
    void sl_onActorDestruction();
private:
    int usageCounter;
}; // ActorPrototype

} // Workflow
} // U2

#endif // _U2_ACTORPROTOTYPE_H_
