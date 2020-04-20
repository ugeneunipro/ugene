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

#ifndef _U2_COLOR_SCHEMA_DIALOG_CONTROLLER_H_
#define _U2_COLOR_SCHEMA_DIALOG_CONTROLLER_H_

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Gui/AppSettingsGUI.h>
#include <U2View/ColorSchemaSettingsController.h>

#include "ui_ColorSchemaDialog.h"
#include <ui_ColorSchemaSettingsWidget.h>
#include <ui_CreateMSAScheme.h>

#include <QMap>
#include <QPixmap>
#include <QMouseEvent>
#include <QDialog>

namespace U2{

class ColorSchemaDialogController : public QDialog, public Ui_ColorSchemaDialog{
    Q_OBJECT
public:
    ColorSchemaDialogController(QMap<char, QColor>& colors);
    ~ColorSchemaDialogController();
    int adjustAlphabetColors();
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent*);
private slots:
    void sl_onClear();
    void sl_onRestore();
private:
    QPixmap* alphabetColorsView;
    QMap<char, QColor>& newColors;
    QMap<char, QColor> storedColors;
    QMap<char, QRect> charsPlacement;

};

class ColorSchemaSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    QString colorsDir;
    QList<ColorSchemeData> customSchemas;
    QList<ColorSchemeData> removedCustomSchemas;
};

class ColorSchemaSettingsPageWidget: public AppSettingsGUIPageWidget, public Ui_ColorSchemaSettingsWidget {
    Q_OBJECT
public:
    ColorSchemaSettingsPageWidget(ColorSchemaSettingsPageController* ctrl);

    virtual void setState(AppSettingsGUIPageState* state);

    virtual AppSettingsGUIPageState* getState(QString& err) const;

private slots:
    void sl_onColorsDirButton();
    void sl_onChangeColorSchema();
    void sl_onAddColorSchema();
    void sl_onDeleteColorSchema();
    void sl_schemaChanged(int);

private:
    QList<ColorSchemeData> customSchemas;
    QList<ColorSchemeData> removedCustomSchemas;
};


class CreateColorSchemaDialog: public QDialog, public Ui_CreateMSAScheme{
    Q_OBJECT
public:
    CreateColorSchemaDialog(ColorSchemeData*, QStringList usedNames);
    int createNewScheme();

private slots:
    void sl_createSchema();
    void sl_cancel();
    void sl_schemaNameEdited(const QString&);
    void sl_alphabetChanged(int);

private:
    bool isNameExist(const QString&);
    bool isSchemaNameValid(const QString&, QString&);

    QStringList usedNames;
    ColorSchemeData* newSchema;
};


}

#endif // _U2_COLOR_SCHEMA_DIALOG_CONTROLLER_H_
