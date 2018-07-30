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

#ifndef _U2_SAMPLE_ACTION_H_
#define _U2_SAMPLE_ACTION_H_

#include <QStringList>

#include <U2Core/U2OpStatus.h>

namespace U2 {

class SampleAction {
public:
    SampleAction(const QString &actionName, const QString &toolsMenu, const QString &samplePath, const QString &actionText);

    QString actionText;
    QString actionName;
    QString toolsMenu;
    QString samplePath;
    QStringList requiredPlugins;
};

class SampleActionsManager : public QObject {
    Q_OBJECT
public:
    SampleActionsManager(QObject *parent);
    void registerAction(const SampleAction &action);

signals:
    void si_clicked(const SampleAction &action);

private slots:
    void sl_clicked();

private:
    int getValidClickedActionId(U2OpStatus &os) const;
    SampleAction getClickedAction(U2OpStatus &os) const;
    QStringList getAbsentPlugins(const QStringList &requiredPlugins) const;

private:
    QList<SampleAction> actions;
};

} // U2

#endif // _U2_SAMPLE_ACTION_H_
