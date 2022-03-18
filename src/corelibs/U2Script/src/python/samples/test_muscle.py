from Bio.Align import MultipleSeqAlignment
from Bio import AlignIO
import u2py

u2py.initContext('./')

inputAlignments = []
inputFile = open('../../data/samples/CLUSTALW/COI_copy1.sto', 'rU')
inputAlignments.append(AlignIO.read(inputFile, 'stockholm'))
inputFile.close()
inputFile = open('../../data/samples/CLUSTALW/HIV-1_copy1.sto', 'rU')
inputAlignments.append(AlignIO.read(inputFile, 'stockholm'))
inputFile.close()

scheme = u2py.Scheme('muscle', inputAlignments)
scheme.setElementAttribute('Write Alignment', 'document-format', 'stockholm')

outputAlignments = scheme.launch()
for aln in outputAlignments:
    print
    aln

scheme.cleanUp()
u2py.releaseContext()
