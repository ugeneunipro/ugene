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

#ifndef _U2_DOCUMENT_FORMAT_SELECTOR_CONTROLLER_
#define _U2_DOCUMENT_FORMAT_SELECTOR_CONTROLLER_

#include <ui_DocumentFormatSelectorDialog.h>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>

#include <QRadioButton>
#include <QToolButton>
#include <QComboBox>

namespace U2 {

class LabelClickProvider : public QObject {
    Q_OBJECT
public:
    LabelClickProvider(QLabel *label, QRadioButton *rb);

private:
    bool eventFilter(QObject *object, QEvent *event);

    QLabel *label;
    QRadioButton *rb;
};

class DocumentFormatSelectorController: public QDialog, public Ui_DocumentFormatSelectorDialog {
	Q_OBJECT

    DocumentFormatSelectorController(QList<FormatDetectionResult>& results, QWidget *p);

public:
    static int selectResult(const GUrl& url, QByteArray& rawData, QList<FormatDetectionResult>& results);
    static QString score2Text(int score);

private slots:
    void sl_moreFormatInfo();

private:
    int getSelectedFormatIdx() const;

    QList<QRadioButton*>    radioButtons;
    QList<QToolButton*>     moreButtons;
    QComboBox *userSelectedFormat;
    const QList<FormatDetectionResult>& formatDetectionResults;
};

} //namespace

#endif
