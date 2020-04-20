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

#include "DirectoryItem.h"

namespace U2 {

DirectoryItem::DirectoryItem(const QString &url, QListWidget *parent)
: UrlItem(url, parent)
{
    options = new DirectoryOptions();
    connect(options, SIGNAL(si_dataChanged()), SIGNAL(si_dataChanged()));

    QIcon dirIcon = QIcon(QString(":U2Designer/images/directory.png"));
    this->setIcon(dirIcon);

    setToolTip(
        "<p>" + url + "</p><p>" +
        tr("Use <i>right click</i> to set advanced options")
        + "</p>");
}

DirectoryItem::~DirectoryItem() {
    options->setParent(NULL);
    delete options;
}

QWidget * DirectoryItem::getOptionsWidget() {
    return options;
}

void DirectoryItem::accept(UrlItemVisitor *visitor) {
    visitor->visit(this);
}

void DirectoryItem::setIncludeFilter(const QString &value) {
    options->setIncludeFilter(value);
}

void DirectoryItem::setExcludeFilter(const QString &value) {
    options->setExcludeFilter(value);
}

void DirectoryItem::setRecursive(bool value) {
    options->setRecursive(value);
}

QString DirectoryItem::getIncludeFilter() const {
    return options->getIncludeFilter();
}

QString DirectoryItem::getExcludeFilter() const {
    return options->getExcludeFilter();
}

bool DirectoryItem::isRecursive() const {
    return options->isRecursive();
}

/************************************************************************/
/* DirectoryOptions */
/************************************************************************/
DirectoryOptions::DirectoryOptions(QWidget *parent)
: QWidget(parent)
{
    setupUi(this);
    connect(includeMaskEdit, SIGNAL(textChanged(const QString &)), SIGNAL(si_dataChanged()));
    connect(excludeMaskEdit, SIGNAL(textChanged(const QString &)), SIGNAL(si_dataChanged()));
    connect(recursiveBox, SIGNAL(clicked(bool)), SIGNAL(si_dataChanged()));
}

void DirectoryOptions::setIncludeFilter(const QString &value) {
    includeMaskEdit->setText(value);
}

void DirectoryOptions::setExcludeFilter(const QString &value) {
    excludeMaskEdit->setText(value);
}

void DirectoryOptions::setRecursive(bool value) {
    recursiveBox->setChecked(value);
}

QString DirectoryOptions::getIncludeFilter() const {
    return includeMaskEdit->text();
}

QString DirectoryOptions::getExcludeFilter() const {
    return excludeMaskEdit->text();
}

bool DirectoryOptions::isRecursive() const {
    return recursiveBox->isChecked();
}

} // U2
