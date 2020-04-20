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

#include <cassert>
#include "Header.h"

namespace U2 {
namespace BAM {

// Header::Reference

Header::Reference::Reference(const QByteArray &name, int length):
        name(name),
        length(length)
{
}

const QByteArray &Header::Reference::getName()const {
    return name;
}

int Header::Reference::getLength()const {
    return length;
}

const QByteArray &Header::Reference::getAssemblyId()const {
    return assemblyId;
}

const QByteArray &Header::Reference::getMd5()const {
    return md5;
}

const QByteArray &Header::Reference::getSpecies()const {
    return species;
}

const QString &Header::Reference::getUri()const {
    return uri;
}

void Header::Reference::setName(const QByteArray &name) {
    this->name = name;
}

void Header::Reference::setLength(int length) {
    this->length = length;
}

void Header::Reference::setAssemblyId(const QByteArray &assemblyId) {
    this->assemblyId = assemblyId;
}

void Header::Reference::setMd5(const QByteArray &md5) {
    this->md5 = md5;
}

void Header::Reference::setSpecies(const QByteArray &species) {
    this->species = species;
}

void Header::Reference::setUri(const QString &uri) {
    this->uri = uri;
}

// Header::ReadGroup

Header::ReadGroup::ReadGroup():
    predictedInsertSize(-1)
{
}

const QByteArray &Header::ReadGroup::getSequencingCenter()const {
    return sequencingCenter;
}

const QByteArray &Header::ReadGroup::getDescription()const {
    return description;
}

const QVariant &Header::ReadGroup::getDate()const {
    return date;
}

const QByteArray &Header::ReadGroup::getLibrary()const {
    return library;
}

const QByteArray &Header::ReadGroup::getPrograms()const {
    return programs;
}

int Header::ReadGroup::getPredictedInsertSize()const {
    return predictedInsertSize;
}

const QByteArray &Header::ReadGroup::getPlatform()const {
    return platform;
}

const QByteArray &Header::ReadGroup::getPlatformUnit()const {
    return platformUnit;
}

const QByteArray &Header::ReadGroup::getSample()const {
    return sample;
}

void Header::ReadGroup::setSequencingCenter(const QByteArray &sequencingCenter) {
    this->sequencingCenter = sequencingCenter;
}

void Header::ReadGroup::setDescription(const QByteArray &description) {
    this->description = description;
}

void Header::ReadGroup::setDate(const QVariant &date) {
    this->date = date;
}

void Header::ReadGroup::setLibrary(const QByteArray &library) {
    this->library = library;
}

void Header::ReadGroup::setPrograms(const QByteArray &programs) {
    this->programs = programs;
}

void Header::ReadGroup::setPredictedInsertSize(int medianInsertSize) {
    this->predictedInsertSize = medianInsertSize;
}

void Header::ReadGroup::setPlatform(const QByteArray &platform) {
    this->platform = platform;
}

void Header::ReadGroup::setPlatformUnit(const QByteArray &platformUnit) {
    this->platformUnit = platformUnit;
}

void Header::ReadGroup::setSample(const QByteArray &sample) {
    this->sample = sample;
}

// Header::Program

Header::Program::Program():
    previousId(-1)
{
}

const QByteArray &Header::Program::getName()const {
    return name;
}

const QByteArray &Header::Program::getCommandLine()const {
    return commandLine;
}

int Header::Program::getPreviousId()const {
    return previousId;
}

const QByteArray &Header::Program::getVersion()const {
    return version;
}

void Header::Program::setName(const QByteArray &name) {
    this->name = name;
}

void Header::Program::setCommandLine(const QByteArray &commandLine) {
    this->commandLine = commandLine;
}

void Header::Program::setPreviousId(int previousId) {
    this->previousId = previousId;
}

void Header::Program::setVersion(const QByteArray &version) {
    this->version = version;
}

// Header

Header::Header() :
    sortingOrder(Unknown)
{

}

const Version &Header::getFormatVersion()const {
    return formatVersion;
}

Header::SortingOrder Header::getSortingOrder()const {
    return sortingOrder;
}

const QList<Header::Reference> &Header::getReferences()const {
    return references;
}

const QList<Header::ReadGroup> &Header::getReadGroups()const {
    return readGroups;
}

const QList<Header::Program> &Header::getPrograms()const {
    return programs;
}

const QString &Header::getText()const {
    return text;
}

void Header::setFormatVersion(const Version &formatVersion) {
    this->formatVersion = formatVersion;
}

void Header::setSortingOrder(SortingOrder sortingOrder) {
    this->sortingOrder = sortingOrder;
}

void Header::setReferences(const QList<Reference> &references) {
    this->references = references;
}

void Header::setReadGroups(const QList<ReadGroup> &readGroups) {
    this->readGroups = readGroups;
}

void Header::setPrograms(const QList<Program> &programs) {
    this->programs = programs;
}

void Header::setText(const QString& text) {
    this->text = text;
}

} // namespace BAM
} // namespace U2
