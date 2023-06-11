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

#include <base_dialogs/GTFileDialog.h>

namespace U2 {
using namespace HI;

class AppSettingsDialogFiller : public Filler {
public:
    enum style { minimal,
                 extended,
                 none };
    enum Tabs {
        General,
        Resources,
        Network,
        FileFormat,
        Directories,
        Logging,
        AlignmentColorScheme,
        GenomeAligner,
        WorkflowDesigner,
        ExternalTools
    };
    AppSettingsDialogFiller(style _itemStyle = extended)
        : Filler("AppSettingsDialog"),
          itemStyle(_itemStyle), r(-1), g(-1), b(-1) {
    }
    AppSettingsDialogFiller(int _r, int _g, int _b)
        : Filler("AppSettingsDialog"),
          itemStyle(none), r(_r), g(_g), b(_b) {
    }
    AppSettingsDialogFiller(CustomScenario* customScenario);
    void commonScenario();

    static void openTab(Tabs tab);
    static void clickOnTool(const QString& toolName);
    static void setExternalToolsDir(const QString& dirPath);
    static void setExternalToolPath(const QString& toolName, const QString& toolPath);
    static void setExternalToolPath(const QString& toolName, const QString& path, const QString& name);
    static QString getExternalToolPath(const QString& toolName);
    static bool isExternalToolValid(const QString& toolName);
    static void clearToolPath(const QString& toolName);
    static bool isToolDescriptionContainsString(const QString& toolName, const QString& checkIfContains);
    static void setTemporaryDirPath(const QString& path);
    static void setDocumentsDirPath(const QString& path);
    static void setWorkflowOutputDirPath(const QString& path);

private:
    style itemStyle;
    int r, g, b;
    static const QMap<Tabs, QString> tabMap;
    static QMap<Tabs, QString> initMap();
};

class NewColorSchemeCreator : public Filler {
public:
    enum alphabet { amino,
                    nucl };
    enum Action { Create,
                  Delete,
                  Change };
    NewColorSchemeCreator(QString _schemeName, alphabet _al, Action _act = Create, bool cancel = false);
    NewColorSchemeCreator(CustomScenario* c);
    virtual void commonScenario();

private:
    QString schemeName;
    alphabet al;
    Action act;
    bool cancel;
};

class CreateAlignmentColorSchemeDialogFiller : public Filler {
public:
    CreateAlignmentColorSchemeDialogFiller(QString _schemeName, NewColorSchemeCreator::alphabet _al)
        : Filler("CreateMSAScheme"), schemeName(_schemeName), al(_al) {
    }
    CreateAlignmentColorSchemeDialogFiller(CustomScenario* c)
        : Filler("CreateMSAScheme", c), al(NewColorSchemeCreator::nucl) {
    }
    virtual void commonScenario();

private:
    QString schemeName;
    NewColorSchemeCreator::alphabet al;
};

class ColorSchemeDialogFiller : public Filler {
public:
    ColorSchemeDialogFiller()
        : Filler("ColorSchemaDialog") {
    }
    ColorSchemeDialogFiller(CustomScenario* c)
        : Filler("ColorSchemaDialog", c) {
    }
    virtual void commonScenario();
};
}  // namespace U2
