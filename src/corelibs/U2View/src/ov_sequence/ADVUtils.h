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

#ifndef _U2_ADV_UTILS_H_
#define _U2_ADV_UTILS_H_

#include <U2Core/global.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Gui/ObjectViewModel.h>

#include <QSet>


namespace U2 {

class ADVSequenceWidget;
class AnnotatedDNAView;
class Annotation;

enum ADVGlobalActionFlag {
    ADVGlobalActionFlag_AddToToolbar        = 1 << 1,
    ADVGlobalActionFlag_AddToAnalyseMenu    = 1 << 2,
    ADVGlobalActionFlag_SingleSequenceOnly  = 1 << 3
};

typedef QFlags<ADVGlobalActionFlag> ADVGlobalActionFlags;

class U2VIEW_EXPORT ADVGlobalAction: public GObjectViewAction {
    Q_OBJECT
public:
    ADVGlobalAction(AnnotatedDNAView* v, const QIcon& icon, const QString& text, int pos = 1000*1000,
        ADVGlobalActionFlags flags = ADVGlobalActionFlags(ADVGlobalActionFlag_AddToToolbar)
                                    | ADVGlobalActionFlag_AddToAnalyseMenu
                                    | ADVGlobalActionFlag_SingleSequenceOnly);

    void addAlphabetFilter(DNAAlphabetType t) {alphabetFilter.insert(t); updateState();}

    ADVGlobalActionFlags getFlags() const {return flags;}

    int getPosition() const {return pos;}

private slots:
    void sl_focusChanged(ADVSequenceWidget*, ADVSequenceWidget*);

private:
    void updateState();

    QSet<DNAAlphabetType>   alphabetFilter;
    int                     pos;
    ADVGlobalActionFlags    flags;
};

class U2VIEW_EXPORT ADVSelectionUtils {
public:
    static QString getSequenceIdsFromSelection(const QList<Annotation*> &selection, bool localBase = false);
};

} //namespace
#endif
