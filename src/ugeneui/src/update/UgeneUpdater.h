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

#ifndef _U2_UGENE_UPDATER_H_
#define _U2_UGENE_UPDATER_H_

#include <QMutex>
#include <QObject>
#include <QScopedPointer>

namespace U2 {

class Version;

class UgeneUpdater : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(UgeneUpdater)
public:
    static UgeneUpdater * getInstance();
    static void release();
    static void onClose();

    void update();
    void setUpdateOnClose(bool value);
    bool isUpdateOnClose() const;

    static bool isUpdateSkipped(const Version &version);
    static void skipUpdate(const Version &version);
    static bool isEnabled();
    static void setEnabled(bool value);

signals:
    void si_update();

private:
    QString getMaintenanceToolPath() const;
    bool hasMaintenanceTool() const;
    void startMaintenanceTool() const;

private:
    bool updateOnClose;

private:
    UgeneUpdater();
    static QScopedPointer<UgeneUpdater> instance;
    static QMutex mutex;
};

} // U2

#endif // _U2_UGENE_UPDATER_H_
