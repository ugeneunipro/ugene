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

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/Log.h>

#include "BAMDbiPlugin.h"
#include "InvalidFormatException.h"
#include "SamReader.h"
#include "CigarValidator.h"

#include <SamtoolsAdapter.h>

namespace U2 {
namespace BAM {

const int SamReader::LOCAL_READ_BUFFER_SIZE = 100000;

Alignment SamReader::parseAlignmentString(QByteArray line) {
    Alignment alignment;

    if(line.isEmpty()) {
        assert(0);
        throw InvalidFormatException(BAMDbiPlugin::tr("Unexpected empty string"));
    }
    QByteArray recordTag;
    QHash<QByteArray, QByteArray> fields;
    QList<QByteArray> tokens = line.split('\t');

    if (tokens.length() < 11) {
        throw InvalidFormatException(BAMDbiPlugin::tr("Not enough fields in one of alignments"));
    }

    {
        QByteArray &qname = tokens[0];
        // workaround for malformed SAMs
        if(!QRegExp("[ -~]{1,255}").exactMatch(qname)) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid query template name: %1").arg(QString(qname)));
        }
        alignment.setName(qname);
    }
    {
        bool ok = false;
        int flagsValue = tokens[1].toInt(&ok);
        if(!ok) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid FLAG value: %1").arg(QString(tokens[1])));
        }
        qint64 flags = 0;
        if(flagsValue & 0x1) {
            flags |= Fragmented;
        }
        if(flagsValue & 0x2) {
            flags |= FragmentsAligned;
        }
        if(flagsValue & 0x4) {
            flags |= Unmapped;
        }
        if(flagsValue & 0x8) {
            flags |= NextUnmapped;
        }
        if(flagsValue & 0x10) {
            flags |= Reverse;
        }
        if(flagsValue & 0x20) {
            flags |= NextReverse;
        }
        if(flagsValue & 0x40) {
            flags |= FirstInTemplate;
        }
        if(flagsValue & 0x80) {
            flags |= LastInTemplate;
        }
        if(flagsValue & 0x100) {
            flags |= SecondaryAlignment;
        }
        if(flagsValue & 0x200) {
            flags |= FailsChecks;
        }
        if(flagsValue & 0x400) {
            flags |= Duplicate;
        }
        alignment.setFlags(flags);
    }
    {
        QByteArray &rname = tokens[2];
        // workaround for malformed SAMs
        if(!QRegExp("[*]|[!-()+-<>-~][ -~]*").exactMatch(rname)) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid reference name: %1").arg(QString(rname)));
        }
        if ("*" == rname) {
            alignment.setReferenceId(-1);
        } else {
            if (!referencesMap.contains(rname)) {
                throw InvalidFormatException(BAMDbiPlugin::tr("Undefined reference name: %1").arg(QString(rname)));
            }
            alignment.setReferenceId(referencesMap[rname]);
        }
    }
    {
        bool ok = false;
        int position = tokens[3].toInt(&ok);
        if(!ok) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid read position value: %1").arg(QString(tokens[3])));
        }
        if(position < 0) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid read position: %1").arg(position));
        }
        alignment.setPosition(position - 1); //to 0-based mapping
    }
    {
        bool ok = false;
        int mapQuality = tokens[4].toInt(&ok);
        if(!ok) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid mapping quality value: %1").arg(QString(tokens[4])));
        }
        if(mapQuality < 0 || mapQuality > 255) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid mapping quality: %1").arg(mapQuality));
        }
        alignment.setMapQuality(mapQuality);
    }
    {
        QByteArray &cigarString = tokens[5];
        if(!QRegExp("[*]|([0-9]+[MIDNSHPX=])+").exactMatch(cigarString)) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid cigar value: %1").arg(QString(cigarString)));
        }
        if ("*" != cigarString) {
            QString err;
            QList<U2CigarToken> res = U2AssemblyUtils::parseCigar(cigarString, err);
            if (!err.isEmpty()) {
                throw InvalidFormatException(BAMDbiPlugin::tr("Invalid cigar value: %1").arg(QString(cigarString)));
            }

            QList<Alignment::CigarOperation> cigar;
            for(int i = 0; i < res.length(); i++) {
                Alignment::CigarOperation::Operation operation;
                switch(res[i].op) {
                case U2CigarOp_M:
                    operation = Alignment::CigarOperation::AlignmentMatch;
                    break;
                case U2CigarOp_I:
                    operation = Alignment::CigarOperation::Insertion;
                    break;
                case U2CigarOp_D:
                    operation = Alignment::CigarOperation::Deletion;
                    break;
                case U2CigarOp_N:
                    operation = Alignment::CigarOperation::Skipped;
                    break;
                case U2CigarOp_S:
                    operation = Alignment::CigarOperation::SoftClip;
                    break;
                case U2CigarOp_H:
                    operation = Alignment::CigarOperation::HardClip;
                    break;
                case U2CigarOp_P:
                    operation = Alignment::CigarOperation::Padding;
                    break;
                case U2CigarOp_EQ:
                    operation = Alignment::CigarOperation::SequenceMatch;
                    break;
                case U2CigarOp_X:
                    operation = Alignment::CigarOperation::SequenceMismatch;
                    break;
                default:
                    throw InvalidFormatException(BAMDbiPlugin::tr("Invalid cigar operation code: %1").arg(res[i].op));
                }
                int operatonLength = res[i].count;
                cigar.append(Alignment::CigarOperation(operatonLength, operation));
            }
            alignment.setCigar(cigar);
        }
    }
    {
        QByteArray nextReference = tokens[6];
        // workaround for malformed SAMs
        if(!QRegExp("[*]|[=]|[!-()+-<>-~][ -~]*").exactMatch(nextReference)) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid mate reference name: %1").arg(QString(nextReference)));
        }
        if ("*" == nextReference) {
            alignment.setNextReferenceId(-1);
        } else if ("=" == nextReference) {
            alignment.setNextReferenceId(alignment.getReferenceId());
        } else {
            if (!referencesMap.contains(nextReference)) {
                throw InvalidFormatException(BAMDbiPlugin::tr("Undefined mate reference name: %1").arg(QString(nextReference)));
            }
            alignment.setNextReferenceId(referencesMap[nextReference]);
        }
        alignment.setNextReferenceName(nextReference);
    }
    {
        bool ok = false;
        int nextPosition = tokens[7].toInt(&ok);
        if(!ok) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid mate position value: %1").arg(QString(tokens[7])));
        }
        if(nextPosition < 0) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid mate position: %1").arg(nextPosition));
        }
        alignment.setNextPosition(nextPosition - 1); //to 0-based mapping
    }
    {
        bool ok = false;
        int templateLength = tokens[8].toInt(&ok);
        if(!ok) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid template length value: %1").arg(QString(tokens[8])));
        }
        if(!(alignment.getFlags() & Fragmented)) {
            if(0 != templateLength) {
                throw InvalidFormatException(BAMDbiPlugin::tr("Invalid template length of a single-fragment template: %1").arg(templateLength));
            }
        }
        alignment.setTemplateLength(templateLength);
    }
    {
        QByteArray sequence = tokens[9];
        if(!QRegExp("[*]|[A-Za-z=.]+").exactMatch(sequence)) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid sequence: %1").arg(QString(sequence)));
        }
        alignment.setSequence(sequence);
    }
    {
        QByteArray quality = tokens[10];
        if ("*" != quality) {
            if(QRegExp("[!-~]+").exactMatch(quality)) {
                alignment.setQuality(quality);
            }
        }
    }
    {
        QByteArray samAuxString;
        bool first = true;
        QMap<QByteArray, QVariant> optionalFields;
        for (int i = 11; i < tokens.length(); i++) {
            if (!first) {
                samAuxString += '\t';
            }
            samAuxString += tokens[i];
            first = false;
        }
        alignment.setAuxData(SamtoolsAdapter::samString2aux(samAuxString));
    }
    {
        // Validation of the CIGAR string.
        int totalLength = 0;
        int length = alignment.getSequence().length();
        const QList<Alignment::CigarOperation> &cigar = alignment.getCigar();
        CigarValidator validator(cigar);
        validator.validate(&totalLength);
        if(!cigar.isEmpty() && length != totalLength) {
            const_cast<QList<Alignment::CigarOperation>&>(cigar).clear(); //Ignore invalid cigar
        }
    }
    return alignment;
}

SamReader::SamReader(IOAdapter &ioAdapter):
        Reader(ioAdapter),
        readBuffer(LOCAL_READ_BUFFER_SIZE, '\0')
{
    readHeader();
}

const Header &SamReader::getHeader()const {
    return header;
}

Alignment SamReader::readAlignment(bool &eof) {
    QByteArray alignmentString = readString(eof);

    return parseAlignmentString(alignmentString);
}

bool SamReader::isEof()const {
    return ioAdapter.isEof();
}

QByteArray SamReader::readString(bool &eof) {
    char* buff  = readBuffer.data();
    bool lineOk = false;
    int len = 0;
    QByteArray result;
    while((len = ioAdapter.readLine(buff, LOCAL_READ_BUFFER_SIZE, &lineOk)) == 0) {}
    if (len == -1) {
        eof = true;
    } else {
        result = QByteArray(buff, len);
    }

    return result;
}

void SamReader::readHeader() {
    char* buff  = readBuffer.data();
    bool lineOk = false;
    int len = 0;
    qint64 bRead = ioAdapter.bytesRead();

    QList<Header::Reference> references;
    {
        QList<Header::ReadGroup> readGroups;
        QList<Header::Program> programs;
        QList<QByteArray> previousProgramIds;
        while ((len = ioAdapter.readLine(buff, LOCAL_READ_BUFFER_SIZE, &lineOk)) >= 0) {
            if(isEof()){
                break;
            }

            QByteArray line = QByteArray(buff, len);
            if(line.isEmpty()) {
                continue;
            }
            if(line.startsWith("@CO")) {
                continue;
            }
            if(!line.startsWith('@')) {
                ioAdapter.skip(bRead - ioAdapter.bytesRead());
                break;
            }
            bRead = ioAdapter.bytesRead();
            QByteArray recordTag;
            QHash<QByteArray, QByteArray> fields;
            {
                QList<QByteArray> tokens = line.split('\t');
                recordTag = tokens[0].mid(1);
                if(!QRegExp("[A-Za-z][A-Za-z]").exactMatch(recordTag)) {
                    throw InvalidFormatException(BAMDbiPlugin::tr("Invalid header record tag: %1").arg(QString(recordTag)));
                }
                for(int index = 1;index < tokens.size();index++) {
                    QByteArray fieldTag;
                    QByteArray fieldValue;
                    {
                        int colonIndex = tokens[index].indexOf(':');
                        if(-1 != colonIndex) {
                            fieldTag = tokens[index].mid(0, colonIndex);
                            fieldValue = tokens[index].mid(colonIndex + 1);
                        } else if("PG" == recordTag) { // workaround for invalid headers produced by some programs
                            continue;
                        } else {
                            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid header field: %1").arg(QString(tokens[index])));
                        }
                    }
                    if(!QRegExp("[A-Za-z][A-Za-z]").exactMatch(fieldTag) && "M5" != fieldTag) { //workaround for bug in the spec
                        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid header field tag: %1").arg(QString(fieldTag)));
                    }
                    // CL and PN tags of can contain any string
                    if(fieldTag!="CL" && fieldTag!="PN" && !QRegExp("[ -~]+").exactMatch(fieldValue)) {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid %1-%2 value: %3").arg(QString(recordTag)).arg(QString(fieldTag)).arg(QString(fieldValue)));
                    }
                    if(!fields.contains(fieldTag)) {
                        fields.insert(fieldTag, fieldValue);
                    } else {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Duplicate header field: %1").arg(QString(fieldTag)));
                    }
                }
            }
            if("HD" == recordTag) {
                if(fields.contains("VN")) {
                    QByteArray value = fields["VN"];
                    if(!QRegExp("[0-9]+\\.[0-9]+").exactMatch(value)) {
                        //Do nothing to support malformed BAMs
                        //throw InvalidFormatException(BAMDbiPlugin::tr("Invalid HD-VN value: %1").arg(QString(value)));
                    }
                    header.setFormatVersion(Version::parseVersion(value));
                } else {
                    throw InvalidFormatException(BAMDbiPlugin::tr("HD record without VN field"));
                }
                if(fields.contains("SO")) {
                    QByteArray value = fields["SO"];
                    if("unknown" == value) {
                        header.setSortingOrder(Header::Unknown);
                    } else if("unsorted" == value) {
                        header.setSortingOrder(Header::Unsorted);
                    } else if("queryname" == value) {
                        header.setSortingOrder(Header::QueryName);
                    } else if("coordinate" == value) {
                        header.setSortingOrder(Header::Coordinate);
                    } else if("sorted" == value) { // workaround for invalid headers produced by some programs
                        header.setSortingOrder(Header::Coordinate);
                    } else {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid HD-SO value: %1").arg(QString(value)));
                    }
                } else {
                    header.setSortingOrder(Header::Unknown);
                }
            } else if("SQ" == recordTag) {
                Header::Reference *reference = NULL;
                QByteArray name;
                if(fields.contains("SN")) {
                    name = fields["SN"];
                } else {
                    throw InvalidFormatException(BAMDbiPlugin::tr("SQ record without SN field"));
                }
                if(fields.contains("LN")) {
                    QByteArray value = fields["LN"];
                    bool ok = false;
                    int length = value.toInt(&ok);
                    if(ok) {
                        Header::Reference newRef(name, length);
                        referencesMap.insert(name, references.size());
                        references.append(newRef);
                        reference = &(references.last());
                    } else {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid SQ-LN value: %1").arg(QString(value)));
                    }
                } else {
                    throw InvalidFormatException(BAMDbiPlugin::tr("SQ record without LN field"));
                }
                assert(NULL != reference);
                if(fields.contains("AS")) {
                    reference->setAssemblyId(fields["AS"]);
                }
                if(fields.contains("M5")) {
                    QByteArray value = fields["M5"];
                    //[a-f] is a workaround (not matching to SAM-1.3 spec) to open 1000 Genomes project BAMs
                    if(!QRegExp("[0-9A-Fa-f]+").exactMatch(value)) {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid SQ-M5 value: %1").arg(QString(value)));
                    }
                    reference->setMd5(fields["M5"]);
                }
                if(fields.contains("SP")) {
                    reference->setSpecies(fields["SP"]);
                }
                if(fields.contains("UR")) {
                    reference->setUri(fields["UR"]);
                }
            } else if("RG" == recordTag) {
                Header::ReadGroup readGroup;
                if(fields.contains("ID")) {
                    QByteArray value = fields["SN"];
                    readGroupsMap.insert(value, readGroups.size());
                } else {
                    fields.insert("ID", "-1");
                }
                if(fields.contains("CN")) {
                    readGroup.setSequencingCenter(fields["CN"]);
                }
                if(fields.contains("DS")) {
                    readGroup.setDescription(fields["DS"]);
                }
                if(fields.contains("DT")) {
                    QByteArray value = fields["DT"];
                    QDateTime dateTime = QDateTime::fromString(value, Qt::ISODate);
                    if(dateTime.isValid()) {
                        readGroup.setDate(dateTime);
                    } else {
                        QDate date = QDate::fromString(value, Qt::ISODate);
                        if(date.isValid()) {
                            readGroup.setDate(date);
                        } else {
                            //Allow anything.
                            //throw InvalidFormatException(BAMDbiPlugin::tr("Invalid RG-DT field value: %1").arg(QString(value)));
                        }
                    }
                }
                if(fields.contains("LB")) {
                    readGroup.setLibrary(fields["LB"]);
                }
                if(fields.contains("PG")) {
                    readGroup.setPrograms(fields["PG"]);
                }
                if(fields.contains("PI")) {
                    QByteArray value = fields["PI"];
                    bool ok = false;
                    int predictedInsertSize = value.toInt(&ok);
                    if(ok) {
                        readGroup.setPredictedInsertSize(predictedInsertSize);
                    } else {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid RG-PI field value: %1").arg(QString(value)));
                    }
                }
                if(fields.contains("PL")) {
                    readGroup.setPlatform(fields["PL"]);
                }
                if(fields.contains("PU")) {
                    readGroup.setPlatformUnit(fields["PU"]);
                }
                if(fields.contains("SM")) {
                    readGroup.setSample(fields["SM"]);
                }
                readGroups.append(readGroup);
            } else if("PG" == recordTag) {
                Header::Program program;
                if(!fields.contains("ID")) {
                    fields.insert("ID", QByteArray::number(programs.size()));
                }
                programsMap.insert(fields["ID"], programs.size());
                if(fields.contains("PN")) {
                    program.setName(fields["PN"]);
                }
                if(fields.contains("CL")) {
                    program.setCommandLine(fields["CL"]);
                }
                if(fields.contains("PP")) {
                    previousProgramIds.append(fields["PP"]);
                } else {
                    previousProgramIds.append(QByteArray());
                }
                if(fields.contains("VN")) {
                    program.setVersion(fields["VN"]);
                }
                programs.append(program);
            }
        }
        for(int index = 0;index < programs.size();index++) {
            const QByteArray &previousProgramId = previousProgramIds[index];
            if(!previousProgramId.isEmpty()) {
                if(programsMap.contains(previousProgramId)) {
                    programs[index].setPreviousId(programsMap[previousProgramId]);
                } else {
                    throw InvalidFormatException(BAMDbiPlugin::tr("Invalid PG-PP field value: %1").arg(QString(previousProgramId)));
                }
            }
        }
        header.setReferences(references);
        header.setReadGroups(readGroups);
        header.setPrograms(programs);
    }
}

} // namespace BAM
} // namespace U2
