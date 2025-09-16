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

#include "PrompterBase.h"

#include <U2Core/Folder.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/Theme.h>

#include <U2Lang/Dataset.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

namespace Workflow {

/*****************************
 * PrompterBaseImpl
 *****************************/
QVariant PrompterBaseImpl::getParameter(const QString& id) {
    if (map.contains(id)) {
        return map.value(id);
    } else {
        return target->getParameter(id)->getAttributePureValue();
    }
}

QString PrompterBaseImpl::getURL(const QString& id, bool* empty, const QString& onEmpty, const QString& defaultValue) {
    QVariant urlVar = getParameter(id);
    QString url;
    if (urlVar.canConvert<QList<Dataset>>()) {
        QStringList urls = WorkflowUtils::getDatasetsUrls(urlVar.value<QList<Dataset>>());
        url = urls.join(";");
    } else {
        url = getParameter(id).toString();
    }
    if (empty != nullptr) {
        *empty = false;
    }
    if (!target->getParameter(id)->getAttributeScript().isEmpty()) {
        url = "got from user script";
    } else if (url.isEmpty()) {
        if (!onEmpty.isEmpty()) {
            return onEmpty;
        }
        if (!defaultValue.isEmpty()) {
            url = defaultValue;
        } else {
            url = QString("<font color='%1'>").arg(Theme::wdParameterLabelStr()) + tr("unset") + "</font>";
        }
        if (empty != nullptr) {
            *empty = true;
        }
    } else if (url.indexOf(";") != -1) {
        url = tr("the list of files");
    } else if (SharedDbUrlUtils::isDbObjectUrl(url)) {
        url = SharedDbUrlUtils::getDbObjectNameByUrl(url);
    } else if (SharedDbUrlUtils::isDbFolderUrl(url)) {
        url = Folder::getFolderName(SharedDbUrlUtils::getDbFolderPathByUrl(url));
    } else {
        QString name = QFileInfo(url).fileName();
        if (!name.isEmpty()) {
            url = name;
        }
    }
    return url;
}

QString PrompterBaseImpl::getRequiredParam(const QString& id) {
    QString url = getParameter(id).toString();
    if (url.isEmpty()) {
        url = QString("<font color='%1'>").arg(Theme::wdParameterLabelStr()) + tr("unset") + "</font>";
    }
    return url;
}

QString PrompterBaseImpl::getScreenedURL(IntegralBusPort* input, const QString& id, const QString& slot, const QString& onEmpty) {
    bool empty = false;
    QString attrUrl = QString("<u>%1</u>").arg(getURL(id, &empty, onEmpty));
    if (!empty) {
        return attrUrl;
    }

    Actor* origin = input->getProducer(slot);
    QString slotUrl;
    if (origin != nullptr) {
        slotUrl = tr("file(s) alongside of input sources of <u>%1</u>").arg(origin->getLabel());
        return slotUrl;
    }

    assert(!attrUrl.isEmpty());
    return attrUrl;
}

QString PrompterBaseImpl::getProducers(const QString& port, const QString& slot) {
    auto input = qobject_cast<IntegralBusPort*>(target->getPort(port));
    CHECK(input != nullptr, "");
    QList<Actor*> producers = input->getProducers(slot);

    QStringList labels;
    foreach (Actor* a, producers) {
        labels << a->getLabel();
    }
    return labels.join(", ");
}

QString PrompterBaseImpl::getProducersOrUnset(const QString& port, const QString& slot) {
    static const QString unsetStr = QString("<font color='%1'>") + tr("unset") + "</font>";
    QString prods = getProducers(port, slot);
    return prods.isEmpty() ? unsetStr.arg(Theme::wdParameterLabelStr()) : prods;
}

QString PrompterBaseImpl::getHyperlink(const QString& id, const QString& val) {
    return QString("<a href=%1:%2>%3</a>").arg(WorkflowUtils::HREF_PARAM_ID).arg(id).arg(val);
}

QString PrompterBaseImpl::getHyperlink(const QString& id, int val) {
    return getHyperlink(id, QString::number(val));
}

QString PrompterBaseImpl::getHyperlink(const QString& id, qreal val) {
    return getHyperlink(id, QString::number(val));
}

void PrompterBaseImpl::sl_actorModified() {
    if (AppContext::isGUIMode() && target != nullptr) {
        setHtml(QString("<center><b>%1</b></center><hr>%2").arg(target->getLabel()).arg(composeRichDoc()));
    }
}

}  // namespace Workflow

}  // namespace U2
