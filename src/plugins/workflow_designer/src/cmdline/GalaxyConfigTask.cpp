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

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentImport.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Core/U2SafePoints.h>
#include "GalaxyConfigTask.h"
#include <QApplication>

namespace U2 {
using namespace WorkflowSerialize;

/*******************************************
* GalaxyConfigTask
*******************************************/

#define TOOL                    "tool"
#define NAME                    "name"
#define VALUE                   "value"
#define LABEL                   "label"
#define ID                      "id"
#define COMMAND                 "command"
#define INPUT                   "input"
#define INPUTS                  "inputs"
#define OUTPUTS                 "outputs"
#define PARAM                   "param"
#define FORMAT                  "format"
#define OPTION                  "option"
#define OPTIONS                 "options"
#define SELECTED                "selected"
#define MAX                     "max"
#define MIN                     "min"
#define DISPLAY                 "display"
#define HIERARCHY               "hierarchy"
#define MULTIPLE                "multiple"
#define SEPARATOR               "separator"
#define DATA                    "data"
#define CHANGE_FORMAT           "change-format"
#define WHEN                    "when"
#define HELP                    "help"
#define WORKFLOW_RUN_LOG        "ugene_workflow_run_log"
#define STDERR_TO_STDOUT        "2>&1"
#define SUBSTRING_NOT_FOUND     -1

const QString GalaxyConfigTask::GALAXY_CONFIG_OPTION   = "galaxy-config";
const QString GalaxyConfigTask::UGENE_PATH_OPTION      = "ugene-path";
const QString GalaxyConfigTask::GALAXY_PATH_OPTION     = "galaxy-path";


GalaxyConfigTask::GalaxyConfigTask( const QString &_schemePath, const QString &_ugenePath,
                                    const QString &_galaxyPath, const QString &_destinationPath ) :
Task( tr( "Create Galaxy config from existing workflow" ), TaskFlag_None ), schemePath(_schemePath), ugenePath(_ugenePath),
                                                                                   galaxyPath(_galaxyPath), destinationPath(_destinationPath)
{
}

GalaxyConfigTask::~GalaxyConfigTask() {}

void tryToAppendSlash( QString &path ) {
    if( !path.endsWith("/") && !path.endsWith("\\") && path.length() > 0 ) {
        path.append("/");
    }
}

void GalaxyConfigTask::fillGObjectTypeMap() {
    portGObjectTypeMap[BasePorts::OUT_MSA_PORT_ID()]             = GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    portGObjectTypeMap[BasePorts::IN_MSA_PORT_ID()]              = GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    portGObjectTypeMap[BasePorts::OUT_SEQ_PORT_ID()]             = GObjectTypes::SEQUENCE;
    portGObjectTypeMap[BasePorts::IN_SEQ_PORT_ID()]              = GObjectTypes::SEQUENCE;
    portGObjectTypeMap[BasePorts::OUT_ANNOTATIONS_PORT_ID()]     = GObjectTypes::ANNOTATION_TABLE;
    portGObjectTypeMap[BasePorts::IN_ANNOTATIONS_PORT_ID()]      = GObjectTypes::ANNOTATION_TABLE;
    portGObjectTypeMap[BasePorts::OUT_TEXT_PORT_ID()]            = GObjectTypes::TEXT;
    portGObjectTypeMap[BasePorts::IN_TEXT_PORT_ID()]             = GObjectTypes::TEXT;
    portGObjectTypeMap[BasePorts::OUT_VARIATION_TRACK_PORT_ID()] = GObjectTypes::VARIANT_TRACK;
    portGObjectTypeMap[BasePorts::IN_VARIATION_TRACK_PORT_ID()]  = GObjectTypes::VARIANT_TRACK;
    portGObjectTypeMap[BasePorts::OUT_ASSEMBLY_PORT_ID()]        = GObjectTypes::ASSEMBLY;
    portGObjectTypeMap[BasePorts::IN_ASSEMBLY_PORT_ID()]         = GObjectTypes::ASSEMBLY;
    portGObjectTypeMap["in-url"]                                 = GObjectTypes::TEXT;
    portGObjectTypeMap["out-url"]                                = GObjectTypes::TEXT;
    portGObjectTypeMap["unknown"]                                = GObjectTypes::UNKNOWN;
}

void GalaxyConfigTask::prepare() {
    schemeName.clear();
    schemeContent.clear();
    galaxyToolName.clear();
    galaxyHelpMessage.clear();
    schemeConfigName.clear();
    schemeConfigPath.clear();

    elemAliases.clear();
    inputElementsPositions.clear();
    outputElementsPositions.clear();
    optionElementsPositions.clear();

    appDirPath = QApplication::applicationDirPath();

    tryToAppendSlash( appDirPath );
    tryToAppendSlash( ugenePath );
    tryToAppendSlash( galaxyPath );
    tryToAppendSlash( destinationPath );

    if( galaxyPath.isEmpty() && !getGalaxyPath() ) {
        coreLog.info( "Galaxy folder is not found" );
    }

    fillGObjectTypeMap();
}

void GalaxyConfigTask::run() {
    CHECK( getSchemeName(),             );
    CHECK( getSchemeContent(),          );
    CHECK( getHelpMessage(),            );
    CHECK( getWorkflowName(),           );
    CHECK( defineAliases(),             );
    CHECK( createConfigForGalaxy(),     );
    if( tryToCopySchemeConfigFile() ) {
       addToolToGalaxy();
    }
}

void GalaxyConfigTask::tryToFindInPath( const QString &objectName, QString &objectPath ) {
    QString envVariable = qgetenv( "GALAXY_DIR" );
    if( envVariable.isEmpty() ) {
        coreLog.info( QString("Environment variable GALAXY_DIR is not found") );
    }

    QString pathVariable = qgetenv("PATH").constData();
    const int objectNamePosition = pathVariable.indexOf( objectName );
    if( objectNamePosition == SUBSTRING_NOT_FOUND ) {
        coreLog.info( QString("Path to %1 folder is not found in PATH variable").arg(objectName) );
        return;
    }
    int currPos = objectNamePosition;
    for( ; currPos >= 0; currPos-- ) {
        if( QString(pathVariable[currPos]) == Constants::COLON ) {
            break;
        }
    }
    const int pathStartPosition = currPos + 1,
              pathEndPosition = pathVariable.indexOf( Constants::COLON, objectNamePosition ),
              pathLength = pathEndPosition - pathStartPosition;
    objectPath = pathVariable.mid( pathStartPosition, pathLength );

    tryToAppendSlash( objectPath );
}

void GalaxyConfigTask::tryToFindByLocate( const QString &objectName, QString &objectPath ) {
    if( !objectPath.isEmpty() ) {
        return;
    }
    QString fileName = objectName + "_path.txt";
    QString locateCommand = QString("locate %1 -l 1 > %2").arg(objectName).arg(fileName);
    system( locateCommand.toLocal8Bit().constData() );

    QFile file( fileName );
    if( !file.open(QIODevice::ReadOnly ) ) {
        coreLog.info( QString("Can not read %1_path.txt file to get path to %1 folder. Check user privileges").arg(objectName) );
        return;
    }
    QTextStream inFile(&file);
    inFile >> objectPath;
    file.close();

    QFile::remove( fileName );
    if( !objectPath.length() ) {
        coreLog.info( QString("Path to %1 folder is not found by \"locate\" command").arg(objectName) );
        return;
    }

    tryToAppendSlash( objectPath );
}

bool GalaxyConfigTask::fileExists( const QString &objectPath, const QString &suffix ) {
    if( !objectPath.isEmpty() ) {
        const QString fullPath = objectPath + suffix;
        if( QFile::exists( fullPath ) ) {
            return true;
        }
        coreLog.info( QString("Galaxy folder does not contain %1 file").arg(suffix) );
    }
    return false;
}

bool GalaxyConfigTask::findPathToObject( const QString &objectName, QString &objectPath ) {
    const QString suffix = "tool_conf.xml";
    tryToFindInPath( objectName, objectPath );

#ifdef __linux__
    tryToFindByLocate( objectName, objectPath );
#endif
    return fileExists( objectPath, suffix );
}

bool GalaxyConfigTask::getGalaxyPath() {
    if( galaxyPath.isEmpty() ) {
        return findPathToObject( "galaxy", galaxyPath );
    }
    return true;
}

bool GalaxyConfigTask::getSchemeName() {
    if( !schemePath.length() || !QFile::exists( schemePath ) ) {
        stateInfo.setError( "Workflow file is incorrect. Check it exists" );
        return false;
    }
    schemeName = schemePath.split("/").last();
    return true;
}

bool GalaxyConfigTask::getSchemeContent() {
    QFile schemeFile( schemePath );
    QFileInfo schemeFileInfo( schemePath );
    if( !schemeFileInfo.isReadable() ) {
        stateInfo.setError( QString("Workflow file %1 is not readable by this user").arg( schemePath) );
        return false;
    }
    if( !schemeFile.open(QIODevice::ReadOnly ) ) {
        stateInfo.setError( QString("Can not open %1 file. Check user privileges ").arg(schemePath) );
        return false;
    }
    QTextStream input(&schemeFile);
    schemeContent = input.readAll();
    schemeFile.close();
    return true;
}

void GalaxyConfigTask::setError( const QString &keyword ) {
    stateInfo.setError( QString("Workflow file is corrupted. It does not contain %1 keyword").arg(keyword) );
}

bool GalaxyConfigTask::getHelpMessage() {
    galaxyHelpMessage = "\n**Description**\n";
    int commentStartPosition = schemeContent.indexOf( Constants::HEADER_LINE );
    if( commentStartPosition == SUBSTRING_NOT_FOUND ) {
        setError( Constants::HEADER_LINE );
        return false;
    }
    commentStartPosition += Constants::HEADER_LINE.length();
    const int commentEndPosition = schemeContent.lastIndexOf( Constants::BODY_START );
    if( commentEndPosition == SUBSTRING_NOT_FOUND ) {
        setError( Constants::BODY_START );
        return false;
    }
    const int commentLength = commentEndPosition - commentStartPosition;

    QString comment;
    comment = schemeContent.mid( commentStartPosition, commentLength );
    comment.replace( Constants::SERVICE_SYM, "\n" );
    galaxyHelpMessage += comment;
    return true;
}

bool GalaxyConfigTask::getWorkflowName() {
    int nameStartPosition = schemeContent.lastIndexOf( Constants::BODY_START );
    nameStartPosition += Constants::BODY_START.length() + 1;
    const int nameEndPosition = schemeContent.indexOf( Constants::BLOCK_START, nameStartPosition );
    CHECK_OPERATIONS( nameEndPosition != SUBSTRING_NOT_FOUND,
                      stateInfo.setError( "Workflow file is corrupted. It does not contain start of body block" ),
                      return false;
    );
    const int nameLength = nameEndPosition - nameStartPosition;

    galaxyToolName = schemeContent.mid( nameStartPosition, nameLength );
    galaxyToolName.replace( QRegExp("^\""),"" );
    galaxyToolName.replace( QRegExp("\"$"),"" );
    return true;
}

bool GalaxyConfigTask::getParameterValue( const QString &keyword, const int searchFrom, QString &parameterValue, int &nextSearchFrom ) {
    const int keywordPosition = schemeContent.indexOf( keyword, searchFrom );
    const int blockEndPosition = schemeContent.indexOf( Constants::BLOCK_END, searchFrom );
    if( keyword == Constants::DESCRIPTION && ( keywordPosition == - 1 || blockEndPosition < keywordPosition ) ) {
        nextSearchFrom = searchFrom;
        return true;
    }

    const int parameterStartPosition = schemeContent.indexOf( Constants::COLON, keywordPosition ) + 1,
              parameterEndPosition = schemeContent.indexOf( Constants::SEMICOLON, parameterStartPosition );
    CHECK_OPERATIONS( parameterStartPosition != SUBSTRING_NOT_FOUND,
                      stateInfo.setError( "Workflow file is corrupted. Begin of alias value is not found" ),
                      return false;
                    );
    CHECK_OPERATIONS( parameterEndPosition != SUBSTRING_NOT_FOUND,
                      stateInfo.setError( "Worklow file is corrupted. End of alias value is not found" ),
                      return false;
                    );
    const int parameterLength = parameterEndPosition - parameterStartPosition;
    parameterValue = schemeContent.mid( parameterStartPosition, parameterLength );

    nextSearchFrom = parameterEndPosition;
    return true;
}

bool GalaxyConfigTask::defineAliases() {
    int aliasesStartPosition = schemeContent.indexOf( Constants::PARAM_ALIASES_START );
    CHECK_OPERATIONS( aliasesStartPosition != SUBSTRING_NOT_FOUND,
        setError( Constants::PARAM_ALIASES_START ),
        return false;
    );
    aliasesStartPosition += Constants::PARAM_ALIASES_START.length();
    const int visualKeywordPosition = schemeContent.indexOf( Constants::VISUAL_START, aliasesStartPosition );
    CHECK_OPERATIONS( visualKeywordPosition != SUBSTRING_NOT_FOUND,
        setError( Constants::VISUAL_START ),
        return false;
    );

    int elementNameStartPosition = schemeContent.indexOf( QRegExp("[a-z]"), aliasesStartPosition );
    while ( elementNameStartPosition < visualKeywordPosition ) {
        const int elementNameEndPosition = schemeContent.indexOf( Constants::DOT, elementNameStartPosition );
        CHECK_OPERATIONS( elementNameEndPosition != SUBSTRING_NOT_FOUND,
                          stateInfo.setError( "Workflow file contains wrong alias" ),
                          return false;
                        );
        const int elementNameLength = elementNameEndPosition - elementNameStartPosition;
        QString elementName = schemeContent.mid( elementNameStartPosition, elementNameLength );
        elementName.replace(QRegExp("[0-9]$"),"");

        const int elementAliasStartPosition = elementNameEndPosition + 1,
                  elementAliasEndPosition = schemeContent.indexOf( Constants::BLOCK_START, elementAliasStartPosition );
        const int elementAliasLength = elementAliasEndPosition - elementAliasStartPosition;
        QString elementAlias = schemeContent.mid( elementAliasStartPosition, elementAliasLength );
        elementAlias.replace(" ","");

        QString aliasName = QString(),
                aliasDescription = QString();
        int aliasNameEndPosition = -1,
            aliasDescriptionEndPosition = -1;

        if( !getParameterValue( Constants::ALIAS, elementAliasEndPosition, aliasName, aliasNameEndPosition ) ) {
            return false;
        }
        if( !getParameterValue( Constants::DESCRIPTION, aliasNameEndPosition, aliasDescription, aliasDescriptionEndPosition ) ) {
            return false;
        }

        QMap< QString, QStringList > elementProperties;
        QStringList elementAliasParameters;
        elementAliasParameters << elementAlias << aliasName << aliasDescription;
        elementProperties[elementName] = elementAliasParameters;
        elemAliases.push_back(elementProperties);

        elementNameStartPosition = schemeContent.indexOf( QRegExp("[a-z]"), aliasDescriptionEndPosition );
    }
    return true;
}

void GalaxyConfigTask::writeToolUnit() {
    galaxyConfigOutput.writeStartElement( TOOL );  //tool unit begin
    galaxyConfigOutput.writeAttribute( ID, galaxyToolName + "_tool" );
    QString toolName = galaxyToolName;
    toolName.replace(" ", "_");
    galaxyConfigOutput.writeAttribute( NAME, toolName );
}

ActorPrototype* GalaxyConfigTask::getElementFromActorPrototypeRegistry( const QString &elementName ) {
    U2::Workflow::ActorPrototypeRegistry *prototypeRegistry =
                                          U2::Workflow::WorkflowEnv::getProtoRegistry();
    assert( NULL != prototypeRegistry );
    return prototypeRegistry->getProto( elementName );
}

void GalaxyConfigTask::fillPositionsList( const QString &elementAttribute, const int elementPosition ) {
    if( BaseAttributes::URL_IN_ATTRIBUTE().getId() == elementAttribute ) {
        inputElementsPositions.push_back( elementPosition );
    } else if ( BaseAttributes::URL_OUT_ATTRIBUTE().getId() == elementAttribute ){
        outputElementsPositions.push_back( elementPosition );
    } else {
        optionElementsPositions.push_back( elementPosition );
    }
}

bool GalaxyConfigTask::divideElementsByType() {
    QList < QMap < QString, QStringList > > ::iterator elemAliasesIterator;
    elemAliasesIterator = elemAliases.begin();
    while( elemAliasesIterator != elemAliases.end() ) {
        const QMap < QString, QStringList > ::iterator elementProperties = (*elemAliasesIterator).begin();
        const QString elementName = elementProperties.key();
        const QString attributeName = elementProperties.value().at(0);

        ActorPrototype *currElement = getElementFromActorPrototypeRegistry( elementName );
        assert( NULL != currElement );

        const QList < Attribute* > elementAttributes = currElement->getAttributes();
        if( elementAttributes.isEmpty() ) {
            stateInfo.setError( QString("Config generation error: can not get attributes from \"%1\" element").arg( currElement->getId() ) );
            return false;
        }

        foreach( Attribute *elementAttribute, elementAttributes ) {
            if( elementAttribute->getId() == attributeName ) {
                fillPositionsList( attributeName, std::distance( elemAliases.begin(), elemAliasesIterator ) );
            }
        }
        elemAliasesIterator++;
    }
    return true;
}

void GalaxyConfigTask::writeRunUgeneCommand( const QString &ugeneExecutable ) {
    if( ugenePath.isEmpty() ) {
        ugenePath = appDirPath;
    }
    QString runUgene = ugenePath + ugeneExecutable + " --task=" + schemePath + " ";
    QList < QMap < QString, QStringList > > ::iterator elemAliasesIterator;
    elemAliasesIterator = elemAliases.begin();
    while( elemAliasesIterator != elemAliases.end() ) {
        QMap <QString, QStringList> ::iterator elementParameters = (*elemAliasesIterator).begin();
        const QString aliasName = elementParameters.value().at(1);
        runUgene += "--" + aliasName + "=$" + aliasName + " ";
        elemAliasesIterator++;
    }
    runUgene += QString("  >> $") + WORKFLOW_RUN_LOG + " " + STDERR_TO_STDOUT;
    galaxyConfigOutput.writeCharacters( runUgene );
}

void GalaxyConfigTask::writeOutputFilesChecks() {
    QList <int> ::iterator outputElementsPositionsIterator;
    outputElementsPositionsIterator = outputElementsPositions.begin();
    while( outputElementsPositionsIterator != outputElementsPositions.end() ) {
        QMap <QString, QStringList> ::iterator elementParameters = elemAliases[*outputElementsPositionsIterator].begin();
        const QString aliasName = elementParameters.value().at(1);
        galaxyConfigOutput.writeDTD( QString("\nif [ ! -s $" + aliasName + " ]; then") );
        galaxyConfigOutput.writeDTD( QString("\necho \"EMPTY RESULT FILE\" > $" + aliasName + ";") );
        galaxyConfigOutput.writeDTD( QString("\nfi;\n") );
        outputElementsPositionsIterator++;
    }
}

bool GalaxyConfigTask::writeCommandUnit() {
    galaxyConfigOutput.writeStartElement ( COMMAND ); //command unit begin
    CHECK( divideElementsByType(), false );

    QString ugeneExecutable;
#ifdef QT_DEBUG
    ugeneExecutable = "ugened";
#else
    ugeneExecutable = "ugene";
#endif
    writeRunUgeneCommand( ugeneExecutable );
    writeOutputFilesChecks( );

    galaxyConfigOutput.writeEndElement();            //command unit end
    return true;
}

void GalaxyConfigTask::getConstraint( const QString &typeName, QString &resultType ) {
    resultType = portGObjectTypeMap[typeName];
    if( resultType.isEmpty() ) {
        resultType = portGObjectTypeMap["unknown"];
    }
}

bool GalaxyConfigTask::getResultType( const ActorPrototype &currElement, QString &resultType ) {
    PortDescriptor *port = currElement.getPortDesciptors().first();
    assert( NULL != port );

    const QString formatType = port->getId();
    getConstraint( formatType, resultType );
    if ( resultType == GObjectTypes::UNKNOWN ) {
        stateInfo.setError( QString("Config generation error: element \"%1\" has unknown type").arg(currElement.getId()) );
        return false;
    }
    return true;
}

void GalaxyConfigTask::writeFormatAttribute( const QString &resultType ) {
    DocumentFormatRegistry *docFormatRegistry = AppContext::getDocumentFormatRegistry();
    assert( NULL != docFormatRegistry );

    DocumentFormatConstraints constraint;
    constraint.supportedObjectTypes.insert( resultType );
    constraint.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList <QString> selectedFormats = docFormatRegistry->selectFormats( constraint );

    QString resultFormatString = QString();
    QList <QString> ::iterator selectedFormatsIterator;
    selectedFormatsIterator = selectedFormats.begin();
    while( selectedFormatsIterator != selectedFormats.end() - 1 ) {
        resultFormatString += *selectedFormatsIterator;
        resultFormatString += Constants::COMMA;
        selectedFormatsIterator++;
    }
    resultFormatString += (*selectedFormatsIterator);
    galaxyConfigOutput.writeAttribute( FORMAT, resultFormatString );
}

void GalaxyConfigTask::writeLabelAttribute( const QStringList &elementParameters, const ActorPrototype &element ) {
    const QString attributeName = elementParameters.at(0);
    QString aliasDescription = elementParameters.at(2);
    QString copyStr = aliasDescription;
    if( aliasDescription.length() == 0 || (!copyStr.contains("[a-zA-Z0-9]") ) ) {
        aliasDescription.clear();
        aliasDescription += element.getDisplayName();
        aliasDescription += ".";
        aliasDescription += element.getAttribute( attributeName )->getDocumentation();
    }
    aliasDescription = aliasDescription.trimmed();
    if( aliasDescription.startsWith( Constants::QUOTE ) ) {
        aliasDescription.remove( 0, 1 );
    }
    if( aliasDescription.endsWith( Constants::QUOTE ) ) {
        aliasDescription.remove( aliasDescription.length() - 1, 1 );
    }
    galaxyConfigOutput.writeAttribute( LABEL, aliasDescription );
}

bool GalaxyConfigTask::writeInputElements() {
    QList <int> ::iterator inputElementsIterator;
    inputElementsIterator = inputElementsPositions.begin();
    while( inputElementsIterator != inputElementsPositions.end() ) {
        galaxyConfigOutput.writeStartElement( PARAM );
        const QMap < QString, QStringList > currAlias = elemAliases[ *inputElementsIterator ];
        QMap < QString, QStringList > ::const_iterator currAliasIterator = currAlias.begin();
        const QString elementName = (currAliasIterator).key(),
                      aliasName = (currAliasIterator).value().at(1);

        galaxyConfigOutput.writeAttribute( Constants::NAME_ATTR, aliasName );
        galaxyConfigOutput.writeAttribute( Constants::TYPE_ATTR, DATA );

        ActorPrototype *currElement = getElementFromActorPrototypeRegistry( elementName );
        assert( NULL != currElement );

        QString resultType = QString();
        CHECK( getResultType( *currElement, resultType ), false );

        writeFormatAttribute( resultType );
        writeLabelAttribute( (currAliasIterator).value(), *currElement );
        galaxyConfigOutput.writeEndElement();
        inputElementsIterator++;
    }
    return true;
}

bool GalaxyConfigTask::isDelegateComboBox( PropertyDelegate *pd ) {
    ComboBoxDelegate *cbd = dynamic_cast <ComboBoxDelegate *>(pd);
    if( cbd != NULL ) {
        return true;
    }
    return false;
}

bool GalaxyConfigTask::isDelegateComboBoxWithChecks( PropertyDelegate *pd ){
    ComboBoxWithChecksDelegate *cbwcd = dynamic_cast <ComboBoxWithChecksDelegate *>(pd);
    if( cbwcd != NULL ) {
        return true;
    }
    return false;
}

bool GalaxyConfigTask::isDelegateSpinBox( PropertyDelegate *pd ) {
    SpinBoxDelegate *sbd = dynamic_cast <SpinBoxDelegate *>(pd);
    if( sbd != NULL ) {
        return true;
    }
    DoubleSpinBoxDelegate *dsbd = dynamic_cast <DoubleSpinBoxDelegate *>(pd);
    if( dsbd != NULL ) {
        return true;
    }
    return false;
}

bool GalaxyConfigTask::isDelegateStringList( PropertyDelegate *pd ) {
    StringListDelegate *sld = dynamic_cast <StringListDelegate *>(pd);
    if( sld != NULL ) {
        return true;
    }
    return false;
}

bool GalaxyConfigTask::tryToWriteSimpleType( const PropertyDelegate * pd, QString &attributeType ) {
    if( pd != NULL ) {
        return false;
    }
    if( attributeType == BaseTypes::BOOL_TYPE()->getId() ) {
        attributeType = "boolean";
    } else if( attributeType == BaseTypes::STRING_TYPE()->getId() ) {
        attributeType = "text";
    } else if( attributeType == BaseTypes::NUM_TYPE()->getId() ) {
        attributeType = "integer";
    } else {
        return false;
    }
    galaxyConfigOutput.writeAttribute( Constants::TYPE_ATTR, attributeType );
    return true;
}

void GalaxyConfigTask::writeSelectAttribute( const PropertyDelegate &pd ) {
    QVariantMap items;
    pd.getItems(items);
    QVariantMap ::iterator itemsIterator;
    itemsIterator = items.begin();
    while( itemsIterator != items.end() ) {
        galaxyConfigOutput.writeStartElement( OPTION );
        galaxyConfigOutput.writeAttribute( VALUE, itemsIterator.value().toString() );
        if( itemsIterator == items.begin() ) {
            galaxyConfigOutput.writeAttribute( SELECTED, "true" );
        }
        galaxyConfigOutput.writeDTD( itemsIterator.key() );
        galaxyConfigOutput.writeEndElement();
        itemsIterator++;
    }
}

void GalaxyConfigTask::writeDrillDownAttribute( const PropertyDelegate &pd ) {
    QVariantMap items;
    pd.getItems(items);
    galaxyConfigOutput.writeStartElement( OPTIONS );
    QVariantMap ::iterator itemsIterator;
    itemsIterator = items.begin();
    while( itemsIterator != items.end() ) {
        galaxyConfigOutput.writeStartElement( OPTION );
        galaxyConfigOutput.writeAttribute( NAME, itemsIterator.key() );
        galaxyConfigOutput.writeAttribute( VALUE, itemsIterator.value().toString() );
        galaxyConfigOutput.writeEndElement();
        itemsIterator++;
    }
    galaxyConfigOutput.writeEndElement();
}

void GalaxyConfigTask::writeMinAndMaxAttributes( const PropertyDelegate &pd ) {
    QVariantMap items;
    pd.getItems(items);
    QString minValue = items.value("minimum").toString();
    QString maxValue = items.value("maximum").toString();
    galaxyConfigOutput.writeAttribute( MIN, minValue );
    galaxyConfigOutput.writeAttribute( MAX, maxValue );
}

bool GalaxyConfigTask::tryToWriteComplexType( PropertyDelegate *pd, const QString &/*attributeName*/ ) {
    QString attributeType = QString();
    assert( pd != NULL );
    if( isDelegateComboBox(pd) ) {
        attributeType = "select";
        galaxyConfigOutput.writeAttribute( Constants::TYPE_ATTR, attributeType );
        writeSelectAttribute( *pd );
    } else if( isDelegateComboBoxWithChecks(pd) ) {
        attributeType = "drill_down";
        galaxyConfigOutput.writeAttribute( Constants::TYPE_ATTR, attributeType );
        galaxyConfigOutput.writeAttribute( DISPLAY, "checkbox" );
        galaxyConfigOutput.writeAttribute( HIERARCHY, "recurse" );
        galaxyConfigOutput.writeAttribute( MULTIPLE, "true" );
        galaxyConfigOutput.writeAttribute( SEPARATOR, Constants::COMMA );
        writeDrillDownAttribute( *pd );
    }
    else if( isDelegateSpinBox(pd) ) {
        QVariantMap items;
        pd->getItems( items );
        const QString typeName1 = items.value("minimum").typeName();
        const QString typeName2 = items.value("maximum").typeName();
        if( typeName1 == "double" || typeName2 == "double" ) {
            attributeType = "float";
        } else {
            attributeType = "integer";
        }
        galaxyConfigOutput.writeAttribute( Constants::TYPE_ATTR, attributeType );
        writeMinAndMaxAttributes( *pd );
    }
    else if( isDelegateStringList(pd) ) {
        attributeType = "text";
        galaxyConfigOutput.writeAttribute( Constants::TYPE_ATTR, attributeType );
    }
    return true;
}

//FIXME
//look at tryToWriteSimpleType and tryToWriteComplexType functions
bool GalaxyConfigTask::writeTypeForOptionElement( const QStringList &elementParameters, const ActorPrototype &element ) {
    const QString attributeName = elementParameters.at(0);
    Attribute *elementAttribute = element.getAttribute(attributeName);
    assert( elementAttribute != NULL );

    ConfigurationEditor *editor = element.getEditor();
    PropertyDelegate *pd = NULL;
    if( editor != NULL ) {
        pd = editor->getDelegate(attributeName);
    }

    QString attributeType = elementAttribute->getAttributeType()->getId();
    if( tryToWriteSimpleType( pd, attributeType ) ) {
        if( attributeType == "integer" ) {
            galaxyConfigOutput.writeAttribute( VALUE, "1" );
        }
    } else if( !tryToWriteComplexType( pd, attributeName ) ) {
        stateInfo.setError( "Config generation error: unknown attribute type: " + attributeType );
        return false;
    }

    return true;
}

bool GalaxyConfigTask::writeOptionElements() {
    QList <int> ::iterator optionElementsIterator;
    optionElementsIterator = optionElementsPositions.begin();
    while( optionElementsIterator != optionElementsPositions.end() ) {
        galaxyConfigOutput.writeStartElement( PARAM );
        const QMap < QString, QStringList > currAlias = elemAliases[ *optionElementsIterator ];
        QMap < QString, QStringList > ::const_iterator currAliasIterator = currAlias.begin();

        const QString elementName = (currAliasIterator).key(),
                      aliasName = (currAliasIterator).value().at(1);
        galaxyConfigOutput.writeAttribute( Constants::NAME_ATTR, aliasName );

        ActorPrototype *currElement = getElementFromActorPrototypeRegistry( elementName );
        assert( NULL != currElement );

        writeLabelAttribute( (currAliasIterator).value(), *currElement );
        CHECK( writeTypeForOptionElement( (currAliasIterator).value(), *currElement ), false );

        galaxyConfigOutput.writeEndElement();
        optionElementsIterator++;
    }
    return true;
}

bool GalaxyConfigTask::writeInputsUnit() {
    galaxyConfigOutput.writeStartElement( INPUTS );  //inputs unit begin
    CHECK( writeInputElements(), false );
    CHECK( writeOptionElements(), false );
    galaxyConfigOutput.writeEndElement();            //inputs unit end
    return true;
}

void GalaxyConfigTask::writeFormatAttributeForOutputElement( const QString &resultType ) {
    DocumentFormatRegistry *docFormatRegistry = AppContext::getDocumentFormatRegistry();
    assert( NULL != docFormatRegistry );

    DocumentFormatConstraints constraint;
    constraint.supportedObjectTypes.insert( resultType );
    constraint.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    const QList <QString> selectedFormats = docFormatRegistry->selectFormats( constraint );
    galaxyConfigOutput.writeAttribute( FORMAT, selectedFormats.first() );
}

bool GalaxyConfigTask::checkDocumentFormatAttribute( const ActorPrototype &element ) {
    const QList <Attribute *> elementAttibutes = element.getAttributes();
    foreach( Attribute *elementAttribute, elementAttibutes ) {
        if( elementAttribute->getId() == BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId() ) {
            return true;
        }
    }
    return false;
}

void GalaxyConfigTask::writeChangeFormatAttribute( const QString &aliasName, const ActorPrototype &element ) {
    galaxyConfigOutput.writeStartElement( CHANGE_FORMAT );

    CHECK(NULL != element.getEditor(), );
    PropertyDelegate *pd = element.getEditor()->getDelegate( BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId() );
    assert( NULL != pd );

    QVariantMap items;
    pd->getItems(items);
    QVariantMap ::iterator itemsIterator;
    itemsIterator = items.begin();
    while( itemsIterator != items.end() ) {
        galaxyConfigOutput.writeStartElement( WHEN );
        galaxyConfigOutput.writeAttribute( INPUT, aliasName );
        galaxyConfigOutput.writeAttribute( VALUE, itemsIterator.value().toString() );
        galaxyConfigOutput.writeAttribute( FORMAT, itemsIterator.value().toString() );
        galaxyConfigOutput.writeEndElement();
        itemsIterator++;
    }
    galaxyConfigOutput.writeEndElement();
}

void GalaxyConfigTask::tryToWriteChangeFormatAttribute( const ActorPrototype &element, QList <int> &usedOptionElements ) {
    if( !checkDocumentFormatAttribute( element ) ) {
        return;
    }
    QList <int> ::iterator optionElementsIterator;
    optionElementsIterator = optionElementsPositions.begin();
    while( optionElementsIterator != optionElementsPositions.end() ) {
        QMap < QString, QStringList > ::iterator elementProperties = elemAliases[ *optionElementsIterator ].begin();

        const QString elementName = elementProperties.key(),
                      attributeName = elementProperties.value().at(0),
                      aliasName = elementProperties.value().at(1);

        if( elementName == element.getId() &&
            attributeName == BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId() &&
            !usedOptionElements.count( *optionElementsIterator ) )
        {
            usedOptionElements.push_back( *optionElementsIterator );
            writeChangeFormatAttribute( aliasName, element );
            break;
        }
        optionElementsIterator++;
    }
}

bool GalaxyConfigTask::writeOutputsUnit() {
    galaxyConfigOutput.writeStartElement( OUTPUTS );  //outputs unit begin
    QList <int>  usedOptionElements;

    QList <int> ::iterator outputElementsIterator;
    outputElementsIterator = outputElementsPositions.begin();
    while( outputElementsIterator != outputElementsPositions.end() ) {
        const QMap < QString, QStringList > currAlias = elemAliases[ *outputElementsIterator ];
        QMap < QString, QStringList > ::const_iterator currAliasIterator = currAlias.begin();

        const QString elementName = (currAliasIterator).key(),
                      aliasName = (currAliasIterator).value().at(1);

        ActorPrototype *currElement = getElementFromActorPrototypeRegistry( elementName );
        assert( NULL != currElement );

        QString resultType = QString();
        CHECK( getResultType( *currElement, resultType ), false );

        galaxyConfigOutput.writeStartElement( DATA );
        writeFormatAttributeForOutputElement( resultType );
        galaxyConfigOutput.writeAttribute( Constants::NAME_ATTR, aliasName );
        tryToWriteChangeFormatAttribute( *currElement, usedOptionElements );
        galaxyConfigOutput.writeEndElement();
        outputElementsIterator++;
    }

    galaxyConfigOutput.writeStartElement( DATA );
    galaxyConfigOutput.writeAttribute( FORMAT, "txt" );
    galaxyConfigOutput.writeAttribute( Constants::NAME_ATTR, WORKFLOW_RUN_LOG );
    galaxyConfigOutput.writeAttribute( "label", WORKFLOW_RUN_LOG );
    galaxyConfigOutput.writeEndElement();

    galaxyConfigOutput.writeEndElement();            //outputs unit end
    return true;
}

void GalaxyConfigTask::writeHelpUnit() {
    galaxyConfigOutput.writeStartElement( HELP );
    galaxyConfigOutput.writeDTD( galaxyHelpMessage );
    galaxyConfigOutput.writeEndElement();
}

bool GalaxyConfigTask::createConfigForGalaxy() {
    schemeConfigPath = schemePath;
    schemeConfigPath.replace( ".uwl", ".xml" );
    QFile galaxyConfigFile( schemeConfigPath );
    galaxyConfigFile.open( QIODevice::WriteOnly );
    if( !galaxyConfigFile.isOpen() ) {
        this->stateInfo.setError( "Config generation error: can not open " + schemeConfigPath );
        return false;
    }
    galaxyConfigOutput.setDevice(&galaxyConfigFile);

    writeToolUnit();
    CHECK( writeCommandUnit(), false );
    CHECK( writeInputsUnit(),  false );
    CHECK( writeOutputsUnit(), false );
    writeHelpUnit();
    galaxyConfigOutput.writeEndElement(); //tool unit end

    galaxyConfigFile.close();
    coreLog.info("Tool config was created");
    coreLog.info("Workflow config path is " + schemeConfigPath);
    return true;
}

bool GalaxyConfigTask::tryToCopySchemeConfigFile() {
    if( destinationPath.isEmpty() ) {
        return true;
    }
    QFileInfo destinationDirInfo( destinationPath );
    if( !destinationDirInfo.isWritable() ) {
        stateInfo.setError( QString("Folder %1 is not writable by this user").arg(destinationPath) );
        return false;
    }

    schemeConfigName = schemeName;
    schemeConfigName.replace( ".uwl", ".xml" );
    QString destinationFilePath = destinationPath + schemeConfigName;
    QString schemeConfigPathCopy = schemeConfigPath;
    destinationFilePath.replace( "\\", "/" );
    schemeConfigPathCopy.replace( "\\", "/" );

    if( !QString::compare( destinationFilePath, schemeConfigPath ) ) {
        return true;
    }
    bool copied = QFile::copy( schemeConfigPath, destinationFilePath );
    if( !copied ) {
        stateInfo.setError( QString("Can not copy %1 to %2").arg(schemeConfigPath).arg(destinationPath) );
        return false;
    }
    return true;
}

bool GalaxyConfigTask::rewriteFile( const QString &sourceFileName, const QString &targetFileName ) {
    if( QFile::exists( targetFileName ) ) {
        if( !QFile::remove( targetFileName ) ) {
            stateInfo.setError( QString("Can not remove %1").arg(targetFileName) );
            return false;
        }
    }
    if( !QFile::copy( sourceFileName, targetFileName ) ) {
        stateInfo.setError( QString("Can not copy %1 to %2").arg(sourceFileName).arg(targetFileName) );
        return false;
    }
    return true;
}

bool GalaxyConfigTask::doCopyCommands( const QString &pathToCopy ) {
    CHECK( rewriteFile( schemeConfigPath, pathToCopy + schemeConfigName ), false );
    CHECK( rewriteFile( schemePath, pathToCopy + schemeName ), false );
    return true;
}
void GalaxyConfigTask::doDeleteCommands() {
    if( !QFile::remove( schemeConfigPath ) ) {
        coreLog.info( QString("Can not delete %1").arg(schemeConfigPath) );
    }
}

bool GalaxyConfigTask::prepareToolDirectory() {
    if( galaxyPath.isEmpty() ) {
        return true;
    }
    const QString truncatedSchemeName = schemeName.left( schemeName.length() - 4 );
    QString pathToCopy = galaxyPath + "tools/" + truncatedSchemeName;
    QDir directory( pathToCopy );

    if( !directory.exists() ) {
        bool created = directory.mkdir( pathToCopy );
        if( !created ) {
            stateInfo.setError( QString("Can not create %1 folder. Check user privileges").arg(pathToCopy) );
            return false;
        }
    }
    QFileInfo copyPathDirectory( pathToCopy );
    if( !copyPathDirectory.isWritable() ) {
        stateInfo.setError( QString("Folder %1 is not writable by this user").arg(pathToCopy) );
        return false;
    }

    tryToAppendSlash( pathToCopy );
    CHECK( doCopyCommands( pathToCopy ), false );
    doDeleteCommands( );
    return true;
}

bool GalaxyConfigTask::makeCopyOfGalaxyToolConfig() {
    const QString toolConfigurationPath = galaxyPath + "tool_conf.xml";
    QString backupFile = toolConfigurationPath;
    backupFile.replace( ".xml", ".bak" );
    if( !QFile::exists( backupFile ) ) {
        bool copied = QFile::copy( toolConfigurationPath, backupFile );
        if( !copied ) {
            stateInfo.setError( QString("Can not copy %1 to %2").arg(toolConfigurationPath).arg(backupFile) );
            return false;
        }
    }
    return true;
}

void GalaxyConfigTask::writeNewSection( const QString &config ) {
    const int toolboxPosition = config.indexOf( "<toolbox>" );
    const int beginLength = toolboxPosition + QString("<toolbox>").length();
    const QString begin = config.mid(0, beginLength );
    const QString end = config.mid( beginLength, config.length() - begin.length() );

    const QString toolsConfigurationPath = galaxyPath + "tool_conf.xml";
    QFile configFile( toolsConfigurationPath );
    if( !configFile.open( QIODevice::WriteOnly ) ) {
        return;
    }
    QXmlStreamWriter galaxyToolsConfigOutput;
    galaxyToolsConfigOutput.setDevice( &configFile );

    QString idStrCopy = galaxyToolName;
    idStrCopy.replace(" ","_");
    const QString truncatedSchemeName = schemeName.left( schemeName.length() - 4 );

    galaxyToolsConfigOutput.writeDTD( begin );
    galaxyToolsConfigOutput.writeDTD( "\n" );
    galaxyToolsConfigOutput.writeStartElement( "section" );
    galaxyToolsConfigOutput.writeAttribute( "name", galaxyToolName + "-tool" );

    galaxyToolsConfigOutput.writeAttribute( "id", idStrCopy );
    galaxyToolsConfigOutput.writeDTD( "\n" );
    galaxyToolsConfigOutput.writeStartElement( "tool" );

    galaxyToolsConfigOutput.writeAttribute( "file", QString( "%1/%1.xml" ).arg(truncatedSchemeName) );
    galaxyToolsConfigOutput.writeEndElement();
    galaxyToolsConfigOutput.writeDTD( "\n" );
    galaxyToolsConfigOutput.writeEndElement();
    galaxyToolsConfigOutput.writeDTD( "\n" );
    galaxyToolsConfigOutput.writeDTD( end );
}

void GalaxyConfigTask::addNewTool() {
    const QString toolConfigurationPath = galaxyPath + "tool_conf.xml";

    QFile configFile( toolConfigurationPath );
    if( !configFile.open( QIODevice::ReadOnly ) ) {
        coreLog.info( QString("Can not open %1").arg( toolConfigurationPath ) );
        return;
    }
    QTextStream input( &configFile );
    QString config = input.readAll();
    configFile.close();
    if( config.indexOf( galaxyToolName ) != SUBSTRING_NOT_FOUND ) {
        return;
    }
    writeNewSection( config );
}

void GalaxyConfigTask::modifyToolConfig() {
    CHECK( makeCopyOfGalaxyToolConfig(), );
    addNewTool();
}

void GalaxyConfigTask::addToolToGalaxy() {
    CHECK( prepareToolDirectory(), );
    modifyToolConfig();
}

} // U2
