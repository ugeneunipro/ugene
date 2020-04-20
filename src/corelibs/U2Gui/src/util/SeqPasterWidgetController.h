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

#ifndef _U2_SEQ_PASTER_WIDGET_CONTROLLER_H_
#define _U2_SEQ_PASTER_WIDGET_CONTROLLER_H_

#include <QWidget>

#include <U2Core/DNASequence.h>

class Ui_SeqPasterWidget;

namespace U2{


class U2GUI_EXPORT SeqPasterWidgetController : public QWidget {
    Q_OBJECT
public:
    SeqPasterWidgetController(QWidget *p = NULL, const QByteArray& initText = QByteArray(), bool needWarning = false);
    ~SeqPasterWidgetController();

    QString validate(); 
    QList<DNASequence> getSequences() const;
    void disableCustomSettings();
    void setPreferredAlphabet(const DNAAlphabet *alp);
    void selectText();
    void setEventFilter(QObject* evFilter);
    void allowFastaFormat(bool allow);

    static QByteArray getNormSequence(const DNAAlphabet * alph, const QByteArray & seq, bool replace, QChar replaceChar);
    
private slots:
    void sl_currentIndexChanged(const QString &newText);

private:
    QString addSequence(const QString &name, QString data);
    static bool isFastaFormat(const QString &data);

    const DNAAlphabet *preferred;
    QList<DNASequence> resultSequences;
    Ui_SeqPasterWidget* ui;
    bool additionalWarning;
    bool allowFastaFormatMode;
};

}//ns

#endif
