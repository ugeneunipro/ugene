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

#include <QFileInfo>
#include <QTextStream>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleAlignmentInfo.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentWalker.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DocumentFormatUtils.h"
#include "StockholmFormat.h"

/* TRANSLATOR U2::StockholmFormat */
namespace U2 {
const QByteArray StockholmFormat::FILE_ANNOTATION_ID = "#=GF ID";
const QByteArray StockholmFormat::FILE_ANNOTATION_AC = "#=GF AC";
const QByteArray StockholmFormat::FILE_ANNOTATION_DE = "#=GF DE";
const QByteArray StockholmFormat::FILE_ANNOTATION_GA = "#=GF GA";
const QByteArray StockholmFormat::FILE_ANNOTATION_NC = "#=GF NC";
const QByteArray StockholmFormat::FILE_ANNOTATION_TC = "#=GF TC";

const QByteArray StockholmFormat::UNI_ANNOTATION_MARK = "# UNIMARK";

const QByteArray StockholmFormat::COLUMN_ANNOTATION_SS_CONS = "#=GC SS_cons";
const QByteArray StockholmFormat::COLUMN_ANNOTATION_RF      = "#=GC RF";

StockholmFormat::ReadError::ReadError(const GUrl& url) : StockholmBaseException( L10N::errorReadingFile(url) ){}

StockholmFormat::WriteError::WriteError(const GUrl& url) : StockholmBaseException( L10N::errorWritingFile(url) ){}
} // U2

const int  BUF_SZ = 1024;
const int  SMALL_BUF_SZ = 128;
const char TERM_SYM = '\0';
const int  NO_BYTES = 0;
const char NEW_LINE = '\n';

const char* HEADER = "# STOCKHOLM 1.0\n\n";
const char* HEADER_MIN = "# STOCKHOLM 1.";
const int HEADER_SZ_MIN = 15;
const char* EOF_STR = "//";

const char COMMENT_OR_MARKUP_LINE = '#';
const char* EMPTY_STR = "";

const int WRITE_BLOCK_LENGTH = 50;

using namespace U2;

//other not supported
enum AnnotationTag {
    NO_TAG = -1,
    ID,
    AC,
    DE,
    SS_CONS,
    RF,
    GA,
    NC,
    TC
};
//other types not supported
enum AnnotationType {
    FILE_ANNOTATION,
    COLUMN_ANNOTATION,
    UNI_ANNOTATION
};

struct Annotation {
    AnnotationType type;
    AnnotationTag tag;
    QString val;
    Annotation( AnnotationType ty, AnnotationTag t, QString v ) { type = ty, tag = t; val = v; }
    virtual ~Annotation(){}
};
//#=GF annotations
struct FileAnnotation : public Annotation {
    FileAnnotation( AnnotationTag t, QString v ): Annotation( FILE_ANNOTATION, t, v ){}
};
//unipro ugene annotations
struct UniAnnotation : public Annotation {
    UniAnnotation( AnnotationTag t, QString v ): Annotation( UNI_ANNOTATION, t, v ){}
};
//#=GC annotations
struct ColumnAnnotation : public Annotation {
    ColumnAnnotation( AnnotationTag t, QString v ): Annotation( COLUMN_ANNOTATION, t, v ){}
};

static Annotation* findAnnotation( const QList< Annotation* >& annList, AnnotationType t, AnnotationTag tag );

//you should put annotations here after creation
struct AnnotationBank {
    QList<Annotation*> ann_list;

    void addAnnotation( Annotation* ann ) {
        if( NULL == ann ) {
            return;
        }
        if( COLUMN_ANNOTATION == ann->type ) { /* Column annotations usually written as blocks with seqs */
            assert( SS_CONS == ann->tag || RF == ann->tag );
            Annotation* nAnn = findAnnotation( ann_list, COLUMN_ANNOTATION, ann->tag );
            if( NULL != nAnn ) {
                nAnn->val.append( ann->val );
                delete ann;
                return;
            }
        }
        ann_list.append( ann );
    }
    ~AnnotationBank() {
        qDeleteAll( ann_list );
    }
}; // AnnotationBank

static Annotation* findAnnotation( const QList< Annotation* >& annList, AnnotationType t, AnnotationTag tag ) {
    Annotation* ret = NULL;
    foreach( Annotation* a, annList ) {
        assert( NULL != a );
        if( a->type == t && a->tag == tag ) {
            ret = a;
            break;
        }
    }
    return ret;
}

static Annotation* getAnnotation( const QByteArray& l ) {
    QByteArray line = l.trimmed();

    if ( line.startsWith( StockholmFormat::FILE_ANNOTATION_ID ) ) {
        QByteArray val = line.mid( StockholmFormat::FILE_ANNOTATION_ID.size() ).trimmed();
        return ( val.size() )? new FileAnnotation( ID , val ) : NULL;
    }
    else if( line.startsWith( StockholmFormat::FILE_ANNOTATION_AC ) ) {
        QByteArray val = line.mid( StockholmFormat::FILE_ANNOTATION_AC.size() ).trimmed();
        return ( val.size() )? new FileAnnotation( AC, val ) : NULL;
    }
    else if( line.startsWith( StockholmFormat::FILE_ANNOTATION_DE ) ) {
        QByteArray val = line.mid( StockholmFormat::FILE_ANNOTATION_DE.size() ).trimmed();
        return ( val.size() )? new FileAnnotation( DE, val ) : NULL;
    }
    else if( line.startsWith( StockholmFormat::FILE_ANNOTATION_GA ) ) {
        QByteArray val = line.mid( StockholmFormat::FILE_ANNOTATION_GA.size() ).trimmed();
        return ( val.size() )? new FileAnnotation( GA, val ) : NULL;
    }
    else if( line.startsWith( StockholmFormat::FILE_ANNOTATION_NC ) ) {
        QByteArray val = line.mid( StockholmFormat::FILE_ANNOTATION_NC.size() ).trimmed();
        return ( val.size() )? new FileAnnotation( NC, val ) : NULL;
    }
    else if( line.startsWith( StockholmFormat::FILE_ANNOTATION_TC ) ) {
        QByteArray val = line.mid( StockholmFormat::FILE_ANNOTATION_TC.size() ).trimmed();
        return ( val.size() )? new FileAnnotation( TC, val ) : NULL;
    }
    else if ( StockholmFormat::UNI_ANNOTATION_MARK == line ) {
        return new UniAnnotation( NO_TAG, line );
    }
    else if( line.startsWith( StockholmFormat::COLUMN_ANNOTATION_SS_CONS ) ) {
        QByteArray val = line.mid( StockholmFormat::COLUMN_ANNOTATION_SS_CONS.size() ).trimmed();
        return ( val.size() )? new ColumnAnnotation( SS_CONS, val ) : NULL;
    }
    else if( line.startsWith( StockholmFormat::COLUMN_ANNOTATION_RF ) ) {
        QByteArray val = line.mid( StockholmFormat::COLUMN_ANNOTATION_RF.size() ).trimmed();
        return ( val.size() )? new ColumnAnnotation( RF, val ) : NULL;
    }
    return NULL;
}

static QString getMsaName( const AnnotationBank& ann_bank ) {
    foreach( Annotation* ann, ann_bank.ann_list ) {
        assert( NULL != ann );
        if ( FILE_ANNOTATION == ann->type && ID == ann->tag ) {
            return ann->val;
        }
    }
    return QString::null;
}

static bool isUniFile( const AnnotationBank& ann_bank ) {
    foreach( Annotation* ann, ann_bank.ann_list ) {
        assert( NULL != ann );
        if ( UNI_ANNOTATION == ann->type && StockholmFormat::UNI_ANNOTATION_MARK == ann->val ) {
            return true;
        }
    }
    return false;
}

static QString getAnnotationName( Annotation* ann ) {
    assert( NULL != ann );

    AnnotationType t = ann->type;
    switch( t ) {
    case UNI_ANNOTATION:
        return QString( StockholmFormat::UNI_ANNOTATION_MARK );
    case FILE_ANNOTATION:
        {
            AnnotationTag tag = ann->tag;
            switch( tag ) {
            case ID:
                return QString( StockholmFormat::FILE_ANNOTATION_ID );
            case AC:
                return QString( StockholmFormat::FILE_ANNOTATION_AC );
            case DE:
                return QString( StockholmFormat::FILE_ANNOTATION_DE );
            case GA:
                return QString( StockholmFormat::FILE_ANNOTATION_GA );
            case NC:
                return QString( StockholmFormat::FILE_ANNOTATION_NC );
            case TC:
                return QString( StockholmFormat::FILE_ANNOTATION_TC );
            default:
                assert( false );
            }
        }
    case COLUMN_ANNOTATION:
        {
            AnnotationTag tag = ann->tag;
            switch( tag ) {
            case SS_CONS:
                return QString( StockholmFormat::COLUMN_ANNOTATION_SS_CONS );
            case RF:
                return QString( StockholmFormat::COLUMN_ANNOTATION_RF );
            default:
                assert( false );
            }
        }
    default:
        assert( false );
    }
    return QString();
}

static QHash< QString, QString > getAnnotationMap( const AnnotationBank& annBank ) {
    QHash< QString, QString > ret;
    foreach( Annotation* ann, annBank.ann_list ) {
        assert( NULL != ann );
        QString annName = getAnnotationName( ann );
        ret[annName] = QString( ann->val );
    }
    return ret;
}

template<class T>
static void checkValThrowException( bool expected, T val1, T val2,  const StockholmFormat::StockholmBaseException& e ) {
    if ( expected != ( val1 == val2 ) ) {
        throw e;
    }
}

static bool checkHeader( const char* data, int sz ) {
    assert( NULL != data && 0 <= sz );

    if ( HEADER_SZ_MIN > sz ) {
        return false;
    }
    return QByteArray( data, sz ).startsWith( HEADER_MIN );
}

//returns true if the line was skipped
static bool skipCommentOrMarkup(IOAdapter* io, U2OpStatus& os, AnnotationBank& ann_bank) {
    assert(NULL != io);

    QByteArray buf(BUF_SZ, TERM_SYM);
    bool term_there = false;
    int ret = io->readUntil(buf.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Exclude, &term_there);
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), false);

    checkValThrowException<int>(false, -1, ret, StockholmFormat::ReadError(io->getURL()));
    if (COMMENT_OR_MARKUP_LINE == buf[0]) {
        QByteArray line;
        line.append(QByteArray(buf.data(), ret));
        while (!term_there) {
            ret = io->readUntil(buf.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Exclude, &term_there);
            CHECK_EXT(!io->hasError(), os.setError(io->errorString()), false);

            checkValThrowException<int>( false, -1, ret,StockholmFormat::ReadError(io->getURL()) );
            CHECK_BREAK(NO_BYTES != ret);

            line.append(QByteArray(buf.data(), ret));
        }
        ann_bank.addAnnotation(getAnnotation(line));
        return true;
    }
    io->skip( -ret );
    if (io->hasError()) {
        os.setError(io->errorString());
    }
    return false;
}

static void skipBlankLines(IOAdapter* io, U2OpStatus& os, QByteArray* lines = NULL) {
    assert(NULL != io);

    char c = 0;
    bool work = true;
    while ( work ) {
        int ret = io->readBlock( &c, 1 );
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), );

        checkValThrowException<int>(false, -1, ret, StockholmFormat::ReadError(io->getURL()));
        CHECK(ret != NO_BYTES, );

        work = TextUtils::LINE_BREAKS[(uchar)c] || TextUtils::WHITES[(uchar)c];
        if ( lines && TextUtils::LINE_BREAKS[(uchar)c] ) {
            lines->append( c );
        }
    }
    io->skip(-1);
    if (io->hasError()) {
        os.setError(io->errorString());
    }
}

//skips all that it can
static void skipMany(IOAdapter* io, U2OpStatus& os, AnnotationBank& ann_bank) {
    assert(NULL != io);

    char c = 0;
    while ( 1 ) {
        bool ret = io->getChar(&c);
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), );

        checkValThrowException<bool>( false, false, ret, StockholmFormat::ReadError(io->getURL()) );
        if (COMMENT_OR_MARKUP_LINE == c) {
            io->skip( -1 );
            CHECK_EXT(!io->hasError(), os.setError(io->errorString()), );

            skipCommentOrMarkup(io, os, ann_bank);
            CHECK_OP(os, );

            continue;
        }
        else if (TextUtils::LINE_BREAKS[(uchar)c] || TextUtils::WHITES[(uchar)c]) {
            skipBlankLines(io, os);
            CHECK_OP(os, );

            continue;
        }
        io->skip(-1);
        if (io->hasError()) {
            os.setError(io->errorString());
        }
        break;
    }
}

static bool eofMsa(IOAdapter* io, U2OpStatus& os) {
    assert(NULL != io);

    QByteArray buf( SMALL_BUF_SZ, TERM_SYM );
    int ret = io->readUntil( buf.data(), SMALL_BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Include );
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), false);

    checkValThrowException(false, -1, ret, StockholmFormat::ReadError(io->getURL()));
    io->skip(-ret);
    if (io->hasError()) {
        os.setError(io->errorString());
    }
    return EOF_STR == QByteArray( buf.data(), ret ).trimmed();
}

static void readEofMsa(IOAdapter* io, U2OpStatus& os) {
    assert(eofMsa(io, os));
    CHECK_OP(os, );

    QByteArray buf( SMALL_BUF_SZ, TERM_SYM );
    int ret = io->readUntil( buf.data(), SMALL_BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Include );
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), );

    checkValThrowException( false, -1, ret, StockholmFormat::ReadError(io->getURL()) );
}

//returns end of sequence name in line
static int getLine( IOAdapter* io, U2OpStatus& os, QByteArray& to ) {
    assert( NULL != io );

    QByteArray buf( BUF_SZ, TERM_SYM );
    bool there = false;

    while (!there) {
        int ret = io->readUntil( buf.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Exclude, &there );
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), 0);

        checkValThrowException<int>( false, -1, ret, StockholmFormat::ReadError(io->getURL()) );
        CHECK_BREAK(ret != NO_BYTES);

        to.append( QByteArray( buf.data(), ret ) );
    }

    int sz = to.size();
    int i = 0;
    for ( i = 0; i < sz; ++i ) {
        if (TextUtils::WHITES[(uchar)to[i]]) {
            break;
        }
    }
    return i;
}

static bool blockEnded(IOAdapter* io, U2OpStatus& os) {
    assert( NULL != io );

    QByteArray lines;
    skipBlankLines(io, os, &lines);
    CHECK_OP(os, false);

    if (eofMsa(io, os)) {
        CHECK_OP(os, false);

        return true;
    }
    int nl_count = 0;
    int lines_sz = lines.size();

    for (int i = 0; i < lines_sz; ++i) {
        nl_count = (NEW_LINE == lines[i]) ? nl_count + 1 : nl_count;
    }
    return 1 < nl_count;
}

static bool nameWasBefore( const MultipleSequenceAlignment& msa, const QString& name ) {
    int n = msa->getNumRows();
    bool ret = false;

    for( int i = 0; i < n; ++i ) {
        if ( name == msa->getMsaRow(i)->getName()) {
            ret = true;
            break;
        }
    }
    return ret;
}

static void changeGaps( QByteArray& seq ) {
    int seq_sz = seq.size();
    for( int i = 0; i < seq_sz; ++i ) {
        if ( '.' == seq[i] ) {
            seq[i] = '-';
        }
    }
}

// returns true if operation was not canceled
static bool loadOneMsa( IOAdapter* io, U2OpStatus& tsi, MultipleSequenceAlignment& msa, AnnotationBank& ann_bank ) {
    assert( NULL != io );

    QByteArray buf( BUF_SZ, TERM_SYM );
    int ret = 0;

    //skip header
    skipBlankLines(io, tsi);
    CHECK_OP(tsi, false);

    ret = io->readUntil( buf.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Exclude );
    CHECK_EXT(!io->hasError(), tsi.setError(io->errorString()), false);

    checkValThrowException<int>( false, -1, ret, StockholmFormat::ReadError(io->getURL()) );
    if ( !checkHeader( buf.data(), ret ) ) {
        throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: bad header line" ) );
    }
    int currentLen = 0;
    //read blocks
    bool firstBlock = true;
    while ( 1 ) {
        CHECK_OP(tsi, false);

        skipMany(io, tsi, ann_bank);
        CHECK_OP(tsi, false);

        if (eofMsa(io, tsi)) {
            break;
        }
        CHECK_OP(tsi, false);

        bool hasSeqs = true;
        int seq_ind = 0;
        int blockSize = -1;
        while (hasSeqs) {
            QByteArray line;
            int name_end = getLine(io, tsi, line);
            CHECK_OP(tsi, false);

            QByteArray name = line.left(name_end);
            QByteArray seq  = line.mid(name_end + 1).trimmed();

            if (name.startsWith(COMMENT_OR_MARKUP_LINE)) {
                ann_bank.addAnnotation(getAnnotation(line));
                hasSeqs = !blockEnded(io, tsi);
                CHECK_OP(tsi, false);

                tsi.setProgress(io->getProgress());
                continue;
            }
            changeGaps(seq);
            if (firstBlock) {
                if (EMPTY_STR == name) {
                    throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: empty sequence name" ) );
                }
                if (nameWasBefore(msa, QString( name.data()))) {
                    throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: equal sequence names in one block" ) );
                }

                msa->addRow(name.data(), seq);
                if (blockSize == -1) {
                    blockSize = seq.size();
                } else if (blockSize != seq.size() ) {
                    throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: sequences in block are not of equal size" ) );
                }
            }
            else {
                if( name != msa->getMsaRow(seq_ind)->getName()) {
                    throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: sequence names are not equal in blocks" ) );
                }
                msa->appendChars(seq_ind, currentLen, seq.constData(), seq.size());
                if (blockSize == -1) {
                    blockSize = seq.size();
                } else if (blockSize != seq.size() ) {
                    throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: sequences in block are not of equal size" ) );
                }
            }
            seq_ind++;
            hasSeqs = !blockEnded(io, tsi);
            CHECK_OP(tsi, false);

            tsi.setProgress(io->getProgress());
        }
        firstBlock = false;
        currentLen += blockSize;
    }// while( 1 )
    readEofMsa(io, tsi);
    CHECK_OP(tsi, false);

    skipBlankLines(io, tsi);
    CHECK_OP(tsi, false);

    if ( msa->getNumRows() == 0 ) {
        throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: empty sequence alignment" ) );
    }
    U2AlphabetUtils::assignAlphabet(msa);
    if (msa->getAlphabet() == NULL) {
        throw StockholmFormat::BadFileData( StockholmFormat::tr( "invalid file: unknown alphabet" ) );
    }
    return true;
}

static void setMsaInfoCutoffs(QVariantMap& info, const QString& string, MultipleAlignmentInfo::Cutoffs cof1,
                               MultipleAlignmentInfo::Cutoffs cof2) {
    QByteArray str = string.toLatin1();
    QTextStream txtStream( str );
    float val1 = .0f;
    float val2 = .0f;
    txtStream >> val1 >> val2;
    MultipleAlignmentInfo::setCutoff( info, cof1, val1 );
    MultipleAlignmentInfo::setCutoff( info, cof2, val2 );
}

static void setMsaInfo(const QHash< QString, QString>& annMap, MultipleSequenceAlignment& ma) {
    QVariantMap info = ma->getInfo();

    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_AC)) {
        MultipleAlignmentInfo::setAccession( info, annMap[StockholmFormat::FILE_ANNOTATION_AC]);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_DE)) {
        MultipleAlignmentInfo::setDescription( info, annMap[StockholmFormat::FILE_ANNOTATION_DE]);
    }
    if (annMap.contains(StockholmFormat::COLUMN_ANNOTATION_SS_CONS)) {
        MultipleAlignmentInfo::setSSConsensus( info, annMap[StockholmFormat::COLUMN_ANNOTATION_SS_CONS]);
    }
    if (annMap.contains(StockholmFormat::COLUMN_ANNOTATION_RF)) {
        MultipleAlignmentInfo::setReferenceLine( info, annMap[StockholmFormat::COLUMN_ANNOTATION_RF]);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_GA)) {
        setMsaInfoCutoffs(info, annMap[StockholmFormat::FILE_ANNOTATION_GA], MultipleAlignmentInfo::CUTOFF_GA1,
                                                                              MultipleAlignmentInfo::CUTOFF_GA2);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_NC)) {
        setMsaInfoCutoffs(info, annMap[StockholmFormat::FILE_ANNOTATION_NC], MultipleAlignmentInfo::CUTOFF_NC1,
                                                                              MultipleAlignmentInfo::CUTOFF_NC2);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_TC)) {
        setMsaInfoCutoffs(info, annMap[StockholmFormat::FILE_ANNOTATION_TC], MultipleAlignmentInfo::CUTOFF_TC1,
                                                                              MultipleAlignmentInfo::CUTOFF_TC2);
    }
    ma->setInfo(info);
}

static void load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& l, const QVariantMap& fs, U2OpStatus& tsi, bool& uni_file) {
    QStringList names_list;
    QString filename = io->getURL().baseFileName();
    while( !io->isEof() ) {
        MultipleSequenceAlignment msa;
        AnnotationBank ann_bank;
        QString name;
        bool notCanceled = true;
        QHash< QString, QString > annMap;

        notCanceled = loadOneMsa( io, tsi, msa, ann_bank );
        CHECK_OP(tsi, );
        CHECK_BREAK(notCanceled);

        uni_file = uni_file || isUniFile( ann_bank );

        name = getMsaName( ann_bank );
        name = (QString::null == name || names_list.contains(name))?
            filename + "_" + QString::number(l.size()): name;
        names_list.append(name);
        msa->setName(name);

        annMap = getAnnotationMap(ann_bank);
        setMsaInfo(annMap, msa);

        U2OpStatus2Log os;
        const QString folder = fs.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
        MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, msa, os);
        CHECK_OP(os, );

        obj->setIndexInfo(annMap);
        l.append( obj );
    }
}

static int getMaxNameLen( const MultipleSequenceAlignment& msa ) {
    assert( msa->getNumRows() != 0 );
    int sz = msa->getNumRows();
    int max_len = msa->getMsaRow(0)->getName().size();

    for( int i = 0; i < sz; ++i ) {
        int name_len =  msa->getMsaRow(i)->getName().size();
        max_len = ( max_len < name_len )? name_len: max_len;
    }
    return max_len;
}
//returns a gap between name and sequence in block
static QByteArray getNameSeqGap( int diff ) {
    assert( 0 <= diff );
    QByteArray ret = "    ";
    for( int i = 0; i < diff; ++i ) {
        ret.append( " " );
    }
    return ret;
}

static void save( IOAdapter* io, const MultipleSequenceAlignment& msa, QString name, U2OpStatus &os ) {
    assert( NULL != io );
    assert( msa->getNumRows() );
    int ret = 0;

    QByteArray header( HEADER );
    ret = io->writeBlock( header );
    checkValThrowException<int>( true, header.size(), ret, StockholmFormat::WriteError(io->getURL()) );
    QByteArray unimark = StockholmFormat::UNI_ANNOTATION_MARK + "\n\n";
    ret = io->writeBlock( unimark );
    checkValThrowException<int>( true, unimark.size(), ret, StockholmFormat::WriteError(io->getURL()) );
    QByteArray idAnn = StockholmFormat::FILE_ANNOTATION_ID + " " + name.replace(QRegExp("\\s"), "_").toLatin1() + "\n\n";
    ret = io->writeBlock( idAnn );
    checkValThrowException<int>( true, idAnn.size(), ret, StockholmFormat::WriteError(io->getURL()) );

    //write sequences
    int name_max_len = getMaxNameLen( msa );
    int seq_len = msa->getLength();
    int cur_seq_pos = 0;
    MultipleSequenceAlignmentWalker walker(msa);
    CHECK_OP(os, );
    while ( 0 < seq_len ) {
        int block_len = ( WRITE_BLOCK_LENGTH >= seq_len )? seq_len: WRITE_BLOCK_LENGTH;
        QList<QByteArray> seqs = walker.nextData(block_len, os);
        CHECK_OP(os, );

        //write block
        U2OpStatus2Log os;
        QList<QByteArray>::ConstIterator si = seqs.constBegin();
        const QList<MultipleSequenceAlignmentRow> rows = msa->getMsaRows();
        QList<MultipleSequenceAlignmentRow>::ConstIterator ri = rows.constBegin();
        for (; si != seqs.constEnd(); si++, ri++) {
            const MultipleSequenceAlignmentRow &row = *ri;
            QByteArray name = row->getName().toLatin1();
            TextUtils::replace(name.data(), name.length(), TextUtils::WHITES, '_');
            name += getNameSeqGap( name_max_len - row->getName().size() );
            ret = io->writeBlock( name );
            checkValThrowException<int>( true, name.size(), ret, StockholmFormat::WriteError(io->getURL()) );
            QByteArray seq = *si + NEW_LINE;
            ret = io->writeBlock( seq );
            checkValThrowException<int>( true, seq.size(), ret, StockholmFormat::WriteError(io->getURL()) );
        }
        ret = io->writeBlock( QByteArray( "\n\n" ) );
        checkValThrowException<int>( true, 2, ret, StockholmFormat::WriteError(io->getURL()) );
        seq_len -= block_len;
        cur_seq_pos += block_len;
    }
    //write eof
    ret = io->writeBlock( QByteArray( "//\n" ) );
    checkValThrowException<int>( true, 3, ret, StockholmFormat::WriteError(io->getURL()) );
}

namespace U2 {
StockholmFormat::StockholmFormat( QObject *obj ) : TextDocumentFormat(obj, BaseDocumentFormats::STOCKHOLM, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject | DocumentFormatFlag_LockedIfNotCreatedByUGENE, QStringList() << "sto") {
    formatName = tr("Stockholm");
    formatDescription = tr("A multiple sequence alignments file format");
    supportedObjectTypes+=GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}


Document* StockholmFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), NULL);
    QList<GObject*> objects;
    try {
        bool uniFile = false;
        QString lockReason;
        load( io, dbiRef, objects, fs, os, uniFile);
        if ( !uniFile ) {
            lockReason = QObject::tr("The document is not created by UGENE");
        }
        return new Document( this, io->getFactory(), io->getURL(), dbiRef, objects, fs, lockReason );
    }
    catch ( const StockholmBaseException& e ) {
        os.setError(e.msg);
    }
    catch ( ... ) {
        os.setError(tr( "unknown error occurred" ));
    }
    qDeleteAll(objects);
    return NULL;
}

void StockholmFormat::storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os) {
    try {
        foreach( GObject* p_obj, doc->getObjects() ) {
            const MultipleSequenceAlignmentObject* aln_obj = qobject_cast<const MultipleSequenceAlignmentObject*>( p_obj );
            assert( NULL != aln_obj );
            save( io, aln_obj->getMultipleAlignment(), aln_obj->getGObjectName(), os );
            CHECK_OP(os, );
        }
    } catch( const StockholmBaseException& ex ) {
        os.setError( ex.msg );
    } catch(...) {
        os.setError(tr( "unknown error occurred" ));
    }
}

FormatCheckResult StockholmFormat::checkRawTextData(const QByteArray& data, const GUrl&) const {
    bool headerIsOK = checkHeader( data.constData(), data.size());
    return headerIsOK ? FormatDetection_VeryHighSimilarity : FormatDetection_NotMatched;
}

bool StockholmFormat::isObjectOpSupported( const Document *doc, DocObjectOp op, GObjectType t ) const {
    Q_UNUSED( op ); Q_UNUSED( doc );
    assert( NULL != doc );
    if ( GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT != t  ) {
        return false;
    }
    /*if (op == DocumentFormat::DocObjectOp_Add) {
        return doc->getObjects().isEmpty();
    }
    return false;*/
    return true;
}


} // U2
