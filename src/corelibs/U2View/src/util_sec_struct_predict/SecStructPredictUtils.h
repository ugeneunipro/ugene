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

#ifndef _U2_SEC_STRUCT_PREDICT_UTILS_H_
#define _U2_SEC_STRUCT_PREDICT_UTILS_H_

#include <U2View/ADVUtils.h>
#include <U2Core/AnnotationData.h>

namespace U2 {

class AnnotatedDNAView;

class SecStructPredictViewAction: public ADVGlobalAction {
    Q_OBJECT
public:
    SecStructPredictViewAction(AnnotatedDNAView* v);
    ~SecStructPredictViewAction();
    static ADVGlobalAction* createAction(AnnotatedDNAView* view);
protected slots:
    void sl_execute();
};

class U2VIEW_EXPORT SecStructPredictUtils {
public:
    static QString getStructNameForCharTag(char tag);
    static QList<SharedAnnotationData> saveAlgorithmResultsAsAnnotations(const QByteArray& predicted, const QString& annotationName);
};



} //namespace

#endif // _U2_SEC_STRUCT_PREDICT_UTILS_H_
