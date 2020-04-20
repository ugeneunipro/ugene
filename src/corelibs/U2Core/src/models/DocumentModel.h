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

#ifndef _U2_DOCUMENT_MODEL_H_
#define _U2_DOCUMENT_MODEL_H_

#include <U2Core/global.h>
#include <U2Core/GUrl.h>
#include <U2Core/DNASequence.h>
#include <U2Core/UnloadedObject.h>
#include <U2Core/U2FormatCheckResult.h>

#include "StateLockableDataModel.h"

#include <QDateTime>
#include <QMimeData>
#include <QPointer>
#include <QScriptValue>

namespace U2 {

class U2OpStatus;

class Document;
class GObject;
class DocumentFormat;
class IOAdapterFactory;
class IOAdapter;
class DocumentFormatConstraints;
class GHints;
class TmpDbiHandle;
class U2SequenceObject;

// Additional info about document format
enum DocumentFormatFlag {
    // Document support reading objects from data stream and can detect object boundaries for all object types correctly
    DocumentFormatFlag_SupportStreaming             = 1<<0,
    // Document support writing
    DocumentFormatFlag_SupportWriting               = 1<<1,
    // Document can only contain 1 object: like text, raw sequence or some formats that do not support streaming
    DocumentFormatFlag_SingleObjectFormat           = 1<<2,
    // Document can't be read from packed stream. Used for database files
    DocumentFormatFlag_NoPack                       = 1<<3,
    // Document is not fully loaded to memory. Used for database files
    DocumentFormatFlag_NoFullMemoryLoad             = 1<<4,
    // Document is not included into format recognition by default
    DocumentFormatFlag_Hidden                       = 1<<5,
    // Document contains only one object of each supported type
    DocumentFormatFlag_OnlyOneObject                = 1<<6,
    // UGENE is unable to create a new document of this format
    // although it can modify existing documents
    DocumentFormatFlag_CannotBeCreated              = 1<<7,
    // Document can contain objects with duplicate names
    DocumentFormatFlag_AllowDuplicateNames          = 1<<8,
    // Document changes are written immedeately, they should'n be saved on closing. Used for databases.

    DocumentFormatFlag_DirectWriteOperations        = 1<<9,
    // Document can be locked if created not by UGENE
    DocumentFormatFlag_LockedIfNotCreatedByUGENE    = 1<<10,

    DocumentFormatFlag_CannotBeCompressed           = 1<<11
};


typedef QFlags<DocumentFormatFlag> DocumentFormatFlags;
#define DocumentFormatFlags_SW (DocumentFormatFlags(DocumentFormatFlag_SupportStreaming) | DocumentFormatFlag_SupportWriting)
#define DocumentFormatFlags_W1 (DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_SingleObjectFormat)


/** Set of hints provided by raw data check routines */

/** 'true' if file contain at least one sequence */
#define RawDataCheckResult_Sequence "sequence"

/** 'true' if at least one sequence in file has gaps */
#define RawDataCheckResult_SequenceWithGaps "sequence-with-gaps"

/** 'true' if multiple sequences were found */
#define RawDataCheckResult_MultipleSequences "multiple-sequences"

/** contains estimation of minimal size of a sequence from document*/
#define RawDataCheckResult_MinSequenceSize "sequence-min-size"

/** contains estimation of maximum size of a sequence from document */
#define RawDataCheckResult_MaxSequenceSize "sequence-max-size"

/*Contains length of sequence that was read from header of sequence file. Used in merge files option*/
#define RawDataCheckResult_HeaderSequenceLength "header-sequence-length"

/** Set of hints that can be processed during document loading */
#define DocumentReadingMode_SequenceMergeGapSize            "merge-gap"
#define DocumentReadingMode_SequenceMergingFinalSizeHint    "merge-size"
#define DocumentReadingMode_SequenceAsAlignmentHint         "sequences-are-msa"
#define DocumentReadingMode_SequenceAsShortReadsHint        "sequences-are-short-reads"
#define DocumentReadingMode_SequenceAsSeparateHint          "sequences-separate-mode"
#define DocumentReadingMode_MaxObjectsInDoc                 "max-objects-in-doc"
#define DocumentReadingMode_DontMakeUniqueNames             "no-unique-names"
#define DocumentReadingMode_LoadAsModified                  "load-as-modified"

/** Set of hints that can be processed during document storing */
#define DocumentWritingMode_SimpleNames                     "simple-names"

/** Set of hints that can affect a document's removal */
#define DocumentRemovalMode_Synchronous                     "synchronous-delete"

/** Hint for splitting variations*/
#define DocumentReadingMode_SplitVariationAlleles           "split-alleles"

/** Set of hints that can be processed during objects conversion */
#define ObjectConvertion_UseGenbankHeader                   "use-genbank-header"

class U2CORE_EXPORT DocumentFormat: public QObject {
    Q_OBJECT
public:
    static const QString DBI_REF_HINT;
    static const QString DBI_FOLDER_HINT;
    static const QString DEEP_COPY_OBJECT;
    static const QString STRONG_FORMAT_ACCORDANCE;

    static const int READ_BUFF_SIZE;

    enum DocObjectOp {
        DocObjectOp_Add,
        DocObjectOp_Remove
    };


    DocumentFormat(QObject* p, const DocumentFormatId& id, DocumentFormatFlags _flags, const QStringList& fileExts = QStringList());

    /* returns unique document format id */
    const DocumentFormatId& getFormatId() const {return id;}

    /* Returns localized format name. */
    const QString& getFormatName() const {return formatName;}

    /* Returns localized format description. */
    const QString& getFormatDescription() const {return formatDescription;}


    /* returns list of usual file extensions for the format
       Example: "fa", "fasta", "gb" ...
    */
    virtual QStringList getSupportedDocumentFileExtensions() const {return fileExtensions;}

    /** Creates new empty document in loaded form.
     * Assigns DBI if needed
     * Hints contain additional information for document loading Common hints:
     * 1) DBI_ALIAS_HINT is used for keeping special DBI-alias
     */
    virtual Document* createNewLoadedDocument(IOAdapterFactory* io, const GUrl& url, U2OpStatus& os, const QVariantMap& hints = QVariantMap());

    /** Creates new document in unloaded state. Assigns DBI if needed */
    virtual Document* createNewUnloadedDocument(IOAdapterFactory* iof, const GUrl& url, U2OpStatus& os,
        const QVariantMap& hints = QVariantMap(),  const QList<UnloadedObjectInfo>& info = QList<UnloadedObjectInfo>(),
        const QString& instanceModLockDesc = QString());

    /** A method for compatibility with old code : creates IO adapter and loads document in DocumentLoadMode_Whole
     * Hints contain additional information for document loading. Common hints:
     * 1) DBI_ALIAS_HINT is used for keeping special DBI-alias
     */
    virtual Document* loadDocument(IOAdapterFactory* iof, const GUrl& url, const QVariantMap& hints, U2OpStatus& os);

    /**
        Loads single dna sequence in streaming mode.
        Note! this function is available only if format supports streaming mode and sequences as an stored data type
    */
    virtual DNASequence* loadSequence( IOAdapter* io, U2OpStatus& ti);

    virtual void storeDocument(Document* d, U2OpStatus& os, IOAdapterFactory* io = NULL, const GUrl& newDocURL = GUrl());

    /* io - opened IOAdapter
     * so you can store many documents to this file
     */
    virtual void storeDocument( Document* d, IOAdapter* io, U2OpStatus& os);

    /** Checks if object can be added/removed to the document */
    virtual bool isObjectOpSupported(const Document* d, DocObjectOp op, GObjectType t) const;

    /*
        Returns score rating that indicates that the data supplied is recognized as a valid document format
        Note: Data can contain only first N (~1024) bytes of the file
        The URL value is optional and provided as supplementary option. URL value here can be empty in some special cases.
    */
    virtual FormatCheckResult checkRawData(const QByteArray& dataPrefix, const GUrl& url = GUrl()) const = 0;

    /* Checks that document format satisfies given constraints */
    virtual bool checkConstraints(const DocumentFormatConstraints& c) const;

    /* Default implementation does nothing */
    virtual void updateFormatSettings(Document* d) const {Q_UNUSED(d);}

    /*
        These object types can be produced by reading documents
        If the format supports write it must support write operation for all the object types it support
    */
    const QSet<GObjectType>& getSupportedObjectTypes() const {return supportedObjectTypes;}

    DocumentFormatFlags getFlags() const {return formatFlags;}

    bool checkFlags(DocumentFormatFlags flagsToCheck) const { return (formatFlags | flagsToCheck) == formatFlags;}

    /**
     * Streaming mode formats implement getSequence() and storeEntry() methods
     */
    virtual bool isStreamingSupport() {return formatFlags.testFlag(DocumentFormatFlag_SupportStreaming);}

    virtual void storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os);

    virtual QString getRadioButtonText() const;

protected:

   /* io - opened IOAdapter.
    * if document format supports streaming reading it must correctly process DocumentLoadMode
    * otherwise, it will load all file from starting position ( default )
    */
    virtual Document* loadDocument(IOAdapter* io, const U2DbiRef& targetDb, const QVariantMap& hints, U2OpStatus& os) = 0;

    DocumentFormatId    id;
    DocumentFormatFlags formatFlags;
    QStringList         fileExtensions;
    QSet<GObjectType>   supportedObjectTypes;
    QString             formatName;
    QString             formatDescription;

private:
    U2DbiRef fetchDbiRef( const QVariantMap &hints, U2OpStatus &os ) const;
};

class DocumentFormatConstraints {
public:
    DocumentFormatConstraints()
        : flagsToSupport(0), flagsToExclude(0), checkRawData(false),
        minDataCheckResult(FormatDetection_VeryLowSimilarity), allowPartialTypeMapping(false)
    {

    }

    void clear() {
        flagsToSupport = 0;
        flagsToExclude = 0;
        checkRawData = false;
        rawData.clear();
        minDataCheckResult = FormatDetection_VeryLowSimilarity;
        allowPartialTypeMapping = false;
        formatsToExclude.clear();
    }
    void addFlagToSupport(DocumentFormatFlag f) {flagsToSupport |= f;}
    void addFlagToExclude(DocumentFormatFlag f) {flagsToExclude |= f;}

    // If 'true' the format supports write operation
    DocumentFormatFlags flagsToSupport;
    DocumentFormatFlags flagsToExclude;
    QSet<GObjectType>   supportedObjectTypes;

    bool                    checkRawData;
    QByteArray              rawData;
    FormatDetectionScore    minDataCheckResult;
    bool                    allowPartialTypeMapping;
    QSet<DocumentFormatId>  formatsToExclude;
};

class DocumentImportersRegistry;
class U2CORE_EXPORT DocumentFormatRegistry  : public QObject {
    Q_OBJECT
public:
    DocumentFormatRegistry(QObject* p = NULL) : QObject(p) {}

    virtual bool registerFormat(DocumentFormat* dfs) = 0;

    virtual bool unregisterFormat(DocumentFormat* dfs) = 0;

    virtual QList<DocumentFormatId> getRegisteredFormats() const = 0;

    virtual DocumentFormat* getFormatById(DocumentFormatId id) const = 0;

    virtual DocumentFormat* selectFormatByFileExtension(const QString& fileExt) const = 0;

    virtual QList<DocumentFormatId> selectFormats(const DocumentFormatConstraints& c) const = 0;

    virtual DocumentImportersRegistry* getImportSupport() = 0;

signals:
    void si_documentFormatRegistered(DocumentFormat*);
    void si_documentFormatUnregistered(DocumentFormat*);
};



enum DocumentModLock {
    DocumentModLock_IO,
    DocumentModLock_USER,
    DocumentModLock_FORMAT_AS_CLASS,
    DocumentModLock_FORMAT_AS_INSTANCE,
    DocumentModLock_UNLOADED_STATE,
    DocumentModLock_NUM_LOCKS
};

enum DocumentObjectRemovalMode {
    DocumentObjectRemovalMode_Deallocate, // ordinary removal: both object and its DB representation are deallocated
    DocumentObjectRemovalMode_OnlyNotify, // fake removal: neither object nor its DB representation are deallocated.
                                          // Only signals about removal are emitted. The object itself remains to belong to the document.
    DocumentObjectRemovalMode_Release     // fake removal: the same as DocumentObjectRemovalMode_OnlyNotify.
                                          // Additionally, the object is excluded from the document's child objects list.
                                          // External code has to handle a proper object deallocation.
};

class DocumentChildEventsHelper;

class U2CORE_EXPORT Document : public  StateLockableTreeItem {
    Q_OBJECT
    Q_PROPERTY( QString name WRITE setName READ getName )
    Q_PROPERTY( GUrl url WRITE setURL READ getURL )

    friend class DocumentChildEventsHelper;

public:
    class Constraints {
    public:
        Constraints() : stateLocked(TriState_Unknown) {}
        TriState                stateLocked;
        QList<DocumentModLock>  notAllowedStateLocks; // if document contains one of these locks -> it's not matched
        QList<DocumentFormatId> formats;              // document format must be in list to match
        GObjectType             objectTypeToAdd;      // document must be ready to add objects of the specified type
    };
    static const QString UNLOAD_LOCK_NAME;


    //Creates document in unloaded state. Populates it with unloaded objects
    Document(DocumentFormat* _df, IOAdapterFactory* _io, const GUrl& _url,
                    const U2DbiRef& _dbiRef,
                    const QList<UnloadedObjectInfo>& unloadedObjects = QList<UnloadedObjectInfo>(),
                    const QVariantMap& hints = QVariantMap(),
                    const QString& instanceModLockDesc = QString());

    //Creates document in loaded state.
    Document(DocumentFormat* _df, IOAdapterFactory* _io, const GUrl& _url,
                    const U2DbiRef& _dbiRef,
                    const QList<GObject*>& objects,
                    const QVariantMap& hints = QVariantMap(),
                    const QString& instanceModLockDesc = QString());

    virtual ~Document();

    DocumentFormat* getDocumentFormat() const {return df;}

    DocumentFormatId getDocumentFormatId() const {return df->getFormatId();}

    IOAdapterFactory* getIOAdapterFactory() const {return io;}

    void setIOAdapterFactory(IOAdapterFactory* iof);

    const QList<GObject*>& getObjects() const {return objects;}

    GObject * getObjectById(const U2DataId &id) const;

    void addObject(GObject* ref);

    bool removeObject(GObject* o, DocumentObjectRemovalMode removalMode = DocumentObjectRemovalMode_Deallocate);

    void setObjectsInUse(const QSet<U2DataId> &objs);

    const QString& getName() const {return name;}

    void setName(const QString& newName);

    const GUrl& getURL() const {return url;}

    const QString& getURLString() const {return url.getURLString();}

    void setURL(const GUrl& newUrl);

    void makeClean();

    void setModificationTrack(bool track);

    // avoid using this method against shared databases documents,
    // since databases allow many objects with the same name. The method returns
    // first matched GObject. Use `getObjectById` instead.
    GObject * findGObjectByName(const QString &name) const;

    QList<GObject*> findGObjectByType(GObjectType t, UnloadedObjectFilter f = UOF_LoadedOnly) const;

    bool isLoaded() const {return modLocks[DocumentModLock_UNLOADED_STATE] == 0;}

    void setLoaded(bool v);

    /** Takes content from sourceDoc. Source doc becomes unloaded after this op is completed! */
    void loadFrom(Document* sourceDoc);

    /** If deleteObjects is false -> does not delete objects and leaves them without parent */
    bool unload(bool deleteObjects = true);

    bool checkConstraints(const Constraints& c) const;

    GHints* getGHints() const {return ctxState;}

    void setGHints(GHints* state);

    QVariantMap getGHintsMap() const;

    StateLock* getDocumentModLock(DocumentModLock type) const {return modLocks[type];}

    void propagateModLocks(Document* doc) const;

    bool hasUserModLock() const {return modLocks[DocumentModLock_USER]!=NULL;}

    void setUserModLock(bool v);

    virtual void setModified(bool modified, const QString& modType = QString());

    virtual bool isModificationAllowed(const QString& modType);

    bool isModified() const { return isTreeItemModified(); }

    void setLastUpdateTime();

    const QDateTime& getLastUpdateTime() const { return lastUpdateTime; }

    const U2DbiRef& getDbiRef() const;

    inline bool isDocumentOwnsDbiResources() const { return documentOwnsDbiResources; }

    inline void setDocumentOwnsDbiResources(bool value) { documentOwnsDbiResources = value; }

    virtual bool isDatabaseConnection() const;

    static void setupToEngine(QScriptEngine *engine);

    Document *getSimpleCopy(DocumentFormat *df, IOAdapterFactory *io, const GUrl &url) const;

private:
    static QScriptValue toScriptValue(QScriptEngine *engine, Document* const &in);
    static void fromScriptValue(const QScriptValue &object, Document* &out);

protected:
    void removeObjectsDataFromDbi(QList<GObject*>objects);
    bool _removeObject(GObject* o, bool deleteObjects = true);
    void _addObject(GObject* obj);
    void _addObjectToHierarchy(GObject* obj);

    void initModLocks(const QString& instanceModLockDesc, bool loaded);

    void checkUnloadedState() const;
    void checkLoadedState() const;
    void checkUniqueObjectNames() const;
    void addUnloadedObjects(const QList<UnloadedObjectInfo>& info);

    GObject * findGObjectByNameInDb(const QString &name) const;
    GObject * findGObjectByNameInMem(const QString &name) const;

    DocumentFormat* const       df;
    IOAdapterFactory*           io;
    GUrl                        url;
    U2DbiRef                    dbiRef; // Default dbi ref for the document

    QString                     name; /* display name == short pathname, excluding the path */
    QList<GObject*>             objects;
    QHash<U2DataId, GObject *>  id2Object;
    QSet<U2DataId>              objectsInUse;
    GHints*                     ctxState;
    QDateTime                   lastUpdateTime;
    bool                        documentOwnsDbiResources;

    StateLock*                  modLocks[DocumentModLock_NUM_LOCKS];
    bool                        loadStateChangeMode;

signals:
    void si_urlChanged();
    void si_nameChanged();
    void si_objectAdded(GObject* o);
    void si_objectRemoved(GObject* o);
    void si_loadedStateChanged();
};

//TODO: decide if to use filters or constraints. May be it worth to remove Document::Constraints at all..

class U2CORE_EXPORT DocumentFilter {
public:
    virtual ~DocumentFilter(){}
    virtual bool matches(Document* doc) const = 0;
};

class U2CORE_EXPORT DocumentConstraintsFilter : public DocumentFilter {
public:
    DocumentConstraintsFilter(const Document::Constraints& _c) : constraints(_c){}

    virtual bool matches(Document* doc) const {
        return doc->checkConstraints(constraints);
    }

protected:
    Document::Constraints constraints;
};

class U2CORE_EXPORT DocumentMimeData : public QMimeData {
    Q_OBJECT
public:
    static const QString MIME_TYPE;
    DocumentMimeData(Document* obj);
    QPointer<Document> objPtr;
    bool hasFormat ( const QString & mimeType ) const { return (mimeType == MIME_TYPE);}
    QStringList formats () const {return (QStringList() << MIME_TYPE);}
};

} //namespace

Q_DECLARE_METATYPE(U2::Document*)
Q_DECLARE_OPERATORS_FOR_FLAGS(U2::DocumentFormatFlags)

#endif
