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

#ifndef _U2_ALIGNMENT_ALGORITHM_GUI_EXTENSION_H_
#define _U2_ALIGNMENT_ALGORITHM_GUI_EXTENSION_H_

#include <U2Core/global.h>

#include <QObject>
#include <QMap>
#include <QVariantMap>
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QWidget>

namespace U2 {

class AbstractAlignmentTaskSettings;

// These classes are intended for extending alignment GUIs
// with options specific to the align algorithm

class U2VIEW_EXPORT AlignmentAlgorithmMainWidget : public QWidget
{
    Q_OBJECT

public:
    AlignmentAlgorithmMainWidget(QWidget* parent, QVariantMap* s);
    virtual ~AlignmentAlgorithmMainWidget();

    virtual QVariantMap getAlignmentAlgorithmCustomSettings(bool append);

    virtual void updateWidget(){}

public slots:
    void sl_externSettingsInvalide();

protected:
    virtual void fillInnerSettings();

protected:
    QVariantMap innerSettings;        //inner settings
    QVariantMap* externSettings;      //extern settings (from msa)
    bool externSettingsExists;                  //msa editor is alive, pointer externSettings is valid
};

class U2VIEW_EXPORT AlignmentAlgorithmGUIExtensionFactory : public QObject {
    Q_OBJECT

public:
    virtual AlignmentAlgorithmMainWidget* createMainWidget(QWidget* parent, QVariantMap* s) = 0;
    virtual bool hasMainWidget(const QWidget* parent);

protected slots:
    virtual void sl_widgetDestroyed(QObject * obj);

protected:
    QMap<const QWidget*, AlignmentAlgorithmMainWidget*> mainWidgets;
};

}   //namespace

#endif  //_U2_ALIGNMENT_ALGORITHM_GUI_EXTENSION_H_
