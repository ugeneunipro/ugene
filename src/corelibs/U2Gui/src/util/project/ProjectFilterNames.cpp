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

#include <QApplication>

#include "ProjectFilterNames.h"

namespace U2 {

namespace ProjectFilterNames {

const QString OBJ_NAME_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Object name");
const QString FEATURE_KEY_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Annotation feature key");

const QString MSA_CONTENT_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Multiple alignment content");
const QString MSA_SEQ_NAME_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Multiple alignment sequence name");

const QString MCA_READ_NAME_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Sanger read name");
const QString MCA_REFERENCE_NAME_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Sanger reference name");
const QString MCA_READ_CONTENT_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Sanger read content");
const QString MCA_REFERENCE_CONTENT_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Sanger reference content");

const QString SEQUENCE_ACC_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Sequence accession number");
const QString TEXT_CONTENT_FILTER_NAME = QApplication::translate("AbstractProjectFilterTask", "Text content");

}

} // namespace U2
