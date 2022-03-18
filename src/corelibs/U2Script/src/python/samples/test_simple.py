from Bio import SeqIO
from Bio.Alphabet import generic_dna
import u2py

u2py.initContext('./')

inputFile = open('../../data/samples/FASTA/chrUn_gl000231.fa', 'rU')
inputSequence = SeqIO.read(inputFile, 'fasta')
inputFile.close()
inputSequence.seq.alphabet = generic_dna

scheme = u2py.Scheme('../../../Ugene samples/repeat_search_scheme.uwl')

scheme.setElementAttribute('Read Sequence', 'url-in', inputSequence)
scheme.setElementAttribute('Write Sequence', 'url-out', 'output.gb')
scheme.setElementAttribute('Write Sequence', 'document-format', 'genbank')
resultList = scheme.launch()
for recordList in resultList:
    for record in recordList:
        print
        'Sequence name: ' + record.name + '; count of features: ' + str(len(record.features))

scheme.cleanUp()
u2py.releaseContext()
