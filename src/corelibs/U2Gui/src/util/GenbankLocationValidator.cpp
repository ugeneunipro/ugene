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

#include <QLineEdit>
#include <QPushButton>

#include <U2Core/AnnotationData.h>
#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2Region.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/GUIUtils.h>

#include "GenbankLocationValidator.h"

namespace U2 {

GenbankLocationValidator::GenbankLocationValidator( QPushButton *_okButton, int length, bool _isCircular, QLineEdit *_le)
:QValidator(),
okButton(_okButton), 
isCircular(_isCircular), 
seqLen(length),
le(_le)
{}

QValidator::State GenbankLocationValidator::validate( QString &str, int &/*ii*/ ) const {
    U2Location loc;
    Genbank::LocationParser::ParsingResult parsingResult = Genbank::LocationParser::Success;
    if (isCircular) {
        parsingResult = Genbank::LocationParser::parseLocation(str.toLatin1().constData(), str.length(), loc, seqLen );
    } else {
        parsingResult = Genbank::LocationParser::parseLocation(str.toLatin1().constData(), str.length(), loc, -1 );
    }

    if (Genbank::LocationParser::Success == parsingResult) {
        if (loc.data()->isEmpty()){
            return failValidate();
        }
        foreach(const U2Region &r, loc.data()->regions){
            if(r.startPos < 0 || r.startPos >seqLen){
                return failValidate();
            }
            if(r.endPos() < 0 || r.endPos() >seqLen){
                return failValidate();
            }
        }
        okButton->setEnabled(true);
        GUIUtils::setWidgetWarning(le, false);
        return QValidator::Acceptable;
    }else{
        return failValidate();
    }
}

QValidator::State GenbankLocationValidator::failValidate() const {
    okButton->setDisabled(true);
    GUIUtils::setWidgetWarning(le, true);
    return QValidator::Intermediate;
}

GenbankLocationValidator::~GenbankLocationValidator(){
    okButton->setEnabled(true);
}

}
