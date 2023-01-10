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

#ifndef _U2_DIALOG_UTILS_H_
#define _U2_DIALOG_UTILS_H_

#include <QLineEdit>
#include <QWidget>

#include <U2Core/global.h>

class QWizard;

namespace U2 {

class DocumentFormatConstraints;
class Logger;
class TaskStateInfo;

class U2GUI_EXPORT WizardUtils : public QObject {
    Q_OBJECT
public:
    static void setWizardMinimumSize(QWizard* wizard, const QSize& minimumSize = QSize());
};

class U2GUI_EXPORT FileLineEdit : public QLineEdit {
    Q_OBJECT
public:
    FileLineEdit(const QString& filter, const QString& type, bool multi, QWidget* parent)
        : QLineEdit(parent), FileFilter(filter), type(type), multi(multi) {
    }
private slots:
    void sl_onBrowse();

private:
    QString FileFilter;
    QString type;
    bool multi;
};

}  // namespace U2

#endif
