/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GenecutHttpFileAdapter.h"
#include "GenecutHttpFileAdapterFactory.h"

#include <U2Core/U2SafePoints.h>
#include <U2Core/Log.h>

#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace U2 {

const IOAdapterId GenecutHttpFileAdapterFactory::GENECUT_ADAPTER_ID = "genecut_http";

GenecutHttpFileAdapterFactory::GenecutHttpFileAdapterFactory(QObject* parent)
    : HttpFileAdapterFactory(parent) {
    name = tr("Genecut HTTP");
}

IOAdapter* GenecutHttpFileAdapterFactory::createIOAdapter() {
    return new GenecutHttpFileAdapter(this);
}

IOAdapterId GenecutHttpFileAdapterFactory::getAdapterId() const {
    return GENECUT_ADAPTER_ID;
}

}