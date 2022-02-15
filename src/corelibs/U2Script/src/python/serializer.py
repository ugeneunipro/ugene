import os
import imp
import u2py_internals

_BioAvailable = False

try:
    imp.find_module('Bio')
    _BioAvailable = True
except ImportError:
    pass

if _BioAvailable:
    from Bio.SeqRecord import SeqRecord
    from Bio.Align import MultipleSeqAlignment
    from Bio import SeqIO
    from Bio import AlignIO


def getUnixAbsPath(unknownPath):
    return unicode(os.path.abspath(unknownPath).replace('\\', '/'))


class Serializer:
    def bioListToString(self, values, treatStringsAsPaths):
        """
        Get string representation of the input argument.
        The latter may be either string or a list of strings and/or BioPython objects
        """
        tmpList = list()
        tmpList.append(values)
        return self._listToString(tmpList, treatStringsAsPaths)

    def stringListToBioList(self, values):
        """
        Convert list of file paths to list of BioPython object based on the files.
        Only certain formats of sequences and MSA allow such a conversion. Other
        files will stay in string representation.
        """
        if not _BioAvailable:
            return values
        resultList = []
        for path in values:
            resultList.append(self._createObjectBasedOnFile(path))
        return resultList

    def createTmpFileAndGetItsPath(self):
        """
        Create a temporary file and get its absolute path
        """
        tmpFileName = self._generateTmpFile(True)
        tmpFileAbsPath = getUnixAbsPath(tmpFileName)
        self.createdFiles[tmpFileAbsPath] = ''
        return unicode(tmpFileAbsPath)

    def cleanUp(self):
        """
        Removes all created temporary files
        """
        for filePath in self.createdFiles:
            os.remove(filePath)

    def _listToString(self, lst, treatStringsAsPaths):
        """
        Convert the given list containing strings and BioPython objects to string.
        Save BioPython objects' content to files and use file paths in result string.
        """
        stringListSeparator = ';'
        mergedList = ''
        for item in lst:
            itemStringRepresentation = ''
            if str == type(item) or unicode == type(item):
                itemStringRepresentation = item if not treatStringsAsPaths else getUnixAbsPath(item)
            elif list == type(item):
                itemStringRepresentation = self._listToString(item, treatStringsAsPaths)
            elif _BioAvailable:
                tmpFile = self._generateTmpFile()
                absTmpFilePath = getUnixAbsPath(tmpFile.name)
                self.createdFiles[absTmpFilePath] = item
                if SeqRecord == type(item):
                    self._saveSeqRecordToFile(item, tmpFile)
                elif MultipleSeqAlignment == type(item):
                    self._saveMsaToFile(item, tmpFile)
                else:
                    if not tmpFile.closed:
                        tmpFile.close()
                    os.remove(absTmpFilePath)
                    del self.createdFiles[absTmpFilePath]
                    raise TypeError('expected SeqRecord or MultipleSeqAlignment object')
                itemStringRepresentation = absTmpFilePath
            else:
                raise TypeError('expected string, list or an object belonging to Bio package')
            mergedList += itemStringRepresentation + stringListSeparator
        mergedList = mergedList[:-1]
        return unicode(mergedList)

    def _saveMsaToFile(self, msa, file):
        """
        Save the given Bio.Align.MultipleSeqAlignment object to the file
        """
        if not _BioAvailable:
            raise ImportError('Bio package is not available')
        if not MultipleSeqAlignment == type(msa):
            raise TypeError('expected Bio.Align.MultipleSeqAlignment object')
        if file.closed:
            file = open(file.name, file.mode)
        AlignIO.write(msa, file, self._msaSaveFormat)
        file.close()

    def _saveSeqRecordToFile(self, record, file):
        """
        Save the given Bio.SeqRecord object to the file
        """
        if not _BioAvailable:
            raise ImportError('Bio package is not available')
        if not SeqRecord == type(record):
            raise TypeError('expected Bio.SeqRecord object')
        if file.closed:
            file = open(file.name, file.mode)
        SeqIO.write(record, file, self._seqRecordSaveFormat)
        file.close()

    def _generateTmpFile(self, generateNameOnly=False):
        """
        Create a brand new temporary empty file and get its handle
        """
        baseName = 'u2py_scheme_file'
        tmpFileExtension = '.tmp'
        resultName = baseName + tmpFileExtension
        fileCounter = 1
        while os.path.exists(resultName) or getUnixAbsPath(resultName) in self.createdFiles:
            resultName = baseName + '_' + str(fileCounter) + tmpFileExtension
            fileCounter += 1
        if not generateNameOnly:
            return open(resultName, 'w')
        else:
            return resultName

    def _createObjectBasedOnFile(self, filePath):
        if not _BioAvailable:
            return filePath
        conversionResult = []
        file = open(filePath, 'rU')
        fileFormatName = u2py_internals.detectFormat(filePath)
        if fileFormatName in Serializer._seqRecordsFileFormats:
            conversionResult = list(SeqIO.parse(file, fileFormatName))
            self.createdFiles[filePath] = conversionResult
        elif fileFormatName in Serializer._msaFileFormats:
            conversionResult = list(AlignIO.parse(file, fileFormatName))
            self.createdFiles[filePath] = conversionResult
        else:
            conversionResult = filePath
        file.close()
        return conversionResult

    """
    Stores absolute paths to temporary files and BioPython objects based on them
    """
    createdFiles = {}
    _seqRecordSaveFormat = 'genbank'
    _msaSaveFormat = 'clustal'
    _seqRecordsFileFormats = ['abi', 'embl', 'fasta', 'fastq', 'genbank', 'swiss']
    _msaFileFormats = ['ace', 'clustal', 'nexus', 'stockholm']
