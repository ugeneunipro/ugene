import sys
import u2py_internals
import serializer

setattr(sys.modules[__name__], 'releaseContext', u2py_internals.releaseContext)


def initContext(workingDirectory):
    return u2py_internals.initContext(serializer.getUnixAbsPath(workingDirectory))


class Scheme(u2py_internals.Scheme):
    def __init__(self, *args):
        argCount = len(args)
        if 1 == argCount:
            schemeFilePath = serializer.getUnixAbsPath(args[0])
            super(Scheme, self).__init__(unicode(schemeFilePath))
        elif 1 < argCount:
            algorithmName = args[0]
            inputData = self._serializer.bioListToString(args[1], True)
            outputData = self._serializer.createTmpFileAndGetItsPath()
            if 3 == argCount:
                outputData = serializer.getUnixAbsPath(args[2])
            elif 3 < argCount:
                raise TypeError('expected 3 or less arguments')
            super(Scheme, self).__init__(unicode(algorithmName), unicode(inputData), unicode(outputData))
        else:
            super(Scheme, self).__init__()

    def addReader(self, readerName, inputData):
        """
        Add to the computational scheme a new read element of the supplied type,
        set its input data and get the element's name
        """
        inputString = self._serializer.bioListToString(inputData, True)
        return super(Scheme, self).addReader(unicode(readerName), inputString)

    def addWriter(self, writerName, outputPath):
        """
        Add to the computational scheme a new write element of the supplied type,
        set its output file path and get the element's name
        """
        absPath = serializer.getUnixAbsPath(outputPath)
        return super(Scheme, self).addWriter(unicode(writerName), absPath)

    def setElementAttribute(self, elementName, attributeName, attributeValue):
        """"
        Specify the given attribute of the supplied element
        """
        treatStringsAsPaths = True
        if not ('url-in' == attributeName or 'url-out' == attributeName):
            treatStringsAsPaths = False
        stringValue = self._serializer.bioListToString(attributeValue, treatStringsAsPaths)
        super(Scheme, self).setElementAttribute(unicode(elementName), unicode(attributeName), stringValue)

    def getElementAttribute(self, elementName, attributeName):
        """"
        Get the value of the given element's attribute
        """
        multivalueAttributeDelimeter = ';'
        attributeValue = super(Scheme, self).getElementAttribute(unicode(elementName), unicode(attributeName))
        if multivalueAttributeDelimeter in attributeValue:
            valuesList = attributeValue.split(multivalueAttributeDelimeter)
            for index, value in enumerate(valuesList):
                if value in self._serializer.createdFiles:
                    valuesList[index] = self._serializer.createdFiles[value]
            return valuesList
        elif attributeValue in self._serializer.createdFiles:
            return self._serializer.createdFiles[attributeValue]
        else:
            return attributeValue

    def launch(self):
        """
        Run the scheme synchronously by the Workflow Designer
        """
        resultList = super(Scheme, self).launch()
        return self._serializer.stringListToBioList(resultList)

    @staticmethod
    def launchSas(*args):
        """
        Create and run the Single Algorithm Scheme synchronously by the Workflow Designer
        """
        tempSerializer = serializer.Serializer()
        argumentCount = len(args)
        if not 2 <= argumentCount <= 3:
            raise RuntimeError('expected 2 or 3 arguments')
        algorithmName = args[0]
        outputString = unicode('')
        if 2 == argumentCount:
            outputString = tempSerializer.createTmpFileAndGetItsPath()
        elif 3 == argumentCount:
            outputString = serializer.getUnixAbsPath(args[2])
        inputString = tempSerializer.bioListToString(args[1], True)
        resultList = u2py_internals.Scheme.launchSas(unicode(algorithmName), inputString, outputString)
        if 2 == argumentCount:
            resultList = tempSerializer.stringListToBioList(resultList)
        tempSerializer.cleanUp()
        return resultList

    def cleanUp(self):
        """
        Removes all temporary files created during the scheme execution
        """
        self._serializer.cleanUp()

    _serializer = serializer.Serializer()
