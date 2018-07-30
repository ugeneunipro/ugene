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

#ifndef _U2_DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_H_
#define _U2_DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_H_

#include <QButtonGroup>

#include <U2Core/DocumentUtils.h>

#include "ui_DocumentProviderSelectorDialog.h"

class QRadioButton;
class QToolButton;

namespace U2 {

class ImportWidget;

class DocumentProviderSelectorController : public QDialog, private Ui_DocumentProviderSelectorDialog {
    Q_OBJECT
public:
    static int selectResult(const GUrl& url, QList<FormatDetectionResult>& results);

private slots:
    void accept();
    void sl_enableConvertInfo(int state);


private:
    DocumentProviderSelectorController(const GUrl& url, QList<FormatDetectionResult>& results, QWidget *parent);
    ImportWidget* getRadioButtonWgt(const FormatDetectionResult& result, QString& radioButtonName, const GUrl& url, int it);
    int getSelectedFormatIdx() const;
    void addFormatRadioButton(const GUrl& url, QList<FormatDetectionResult> &results, QButtonGroup* bg, int it);
    void fillTitle(const FormatDetectionResult &result);

    static QString getButtonName(const GObjectType &objectType);

    QList<QRadioButton*> formatsRadioButtons;
    QList<ImportWidget*> radioButtonConnectedWidget;
    QList<FormatDetectionResult>& formatDetectionResults;
    QList<QString> formatInfo;
    QString title;
    QString selectedFormat;
    int selectedRadioButton;

    static const QString DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_ROOT;
    static const QString SELECTION;

};

}   // namespace U2

#endif // _U2_DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_H_
