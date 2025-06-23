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

#include <U2Core/AppContext.h>

#include <U2Gui/MainWindow.h>

#include <U2Lang/ActorDocument.h>
#include <U2Lang/IntegralBusModel.h>

namespace U2 {

namespace Workflow {

/**
 * provides utility functions for ActorDocument purposes
 */
class U2DESIGNER_EXPORT PrompterBaseImpl : public ActorDocument, public Prompter {
    Q_OBJECT
public:
    PrompterBaseImpl(Actor* p = 0)
        : ActorDocument(p) {
    }

    static bool isWildcardURL(const QString& url) {
        return url.indexOf(QRegExp("[*?\\[\\]]")) >= 0;
    }

    ActorDocument* createDescription(Actor*) override = 0;

    QString getURL(const QString& id, bool* empty = nullptr, const QString& onEmpty = "", const QString& defaultValue = "");
    QString getScreenedURL(IntegralBusPort* input, const QString& id, const QString& slot, const QString& onEmpty = "");
    QString getRequiredParam(const QString& id);
    QVariant getParameter(const QString& id);
    QString getProducers(const QString& port, const QString& slot);
    QString getProducersOrUnset(const QString& port, const QString& slot);
    static QString getHyperlink(const QString& id, const QString& val);
    static QString getHyperlink(const QString& id, int val);
    static QString getHyperlink(const QString& id, qreal val);

    virtual QString composeRichDoc() = 0;
    void update(const QVariantMap& cfg) override {
        map = cfg;
        sl_actorModified();
    }

protected slots:
    virtual void sl_actorModified();

protected:
    QVariantMap map;

};  // PrompterBaseImpl

/**
 * template realization of Prompter and ActorDocument in one entity
 * represents creating description, updating description and displaying description facilities
 *
 * only classes that inherit ActorDocument can be used as a template argument
 * provides
 */
template<typename T>
class PrompterBase : public PrompterBaseImpl {
public:
    PrompterBase(Actor* p = 0, bool listenInputs = true)
        : PrompterBaseImpl(p), listenInputs(listenInputs) {
    }
    virtual ~PrompterBase() = default;
    ActorDocument* createDescription(Actor* a) override {
        auto doc = new T(a);
        doc->connect(a, SIGNAL(si_labelChanged()), SLOT(sl_actorModified()));
        doc->connect(a, SIGNAL(si_modified()), SLOT(sl_actorModified()));
        if (listenInputs) {
            foreach (Workflow::Port* input, a->getInputPorts()) {
                doc->connect(input, SIGNAL(bindingChanged()), SLOT(sl_actorModified()));
            }
        }

        foreach (Workflow::Port* input, a->getOutputPorts()) {
            doc->connect(input, SIGNAL(bindingChanged()), SLOT(sl_actorModified()));
        }
        //}
        return doc;
    }

protected:
    bool listenInputs;

};  // PrompterBase

}  // namespace Workflow

}  // namespace U2

