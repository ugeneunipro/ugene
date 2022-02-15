import u2py

u2py.initContext('./')

scheme = u2py.Scheme()
seqReader = scheme.addReader('read-sequence', '../../data/samples/FASTA/chrUn_gl000231.fa')
seqWriter = scheme.addWriter('write-sequence', '../../data/samples/FASTA/ann.gb')
repeats = scheme.addElement('repeats-search')
scheme.addActorsBinding(seqReader, 'sequence', repeats, 'in-sequence.sequence')
scheme.addActorsBinding(repeats, 'annotations', seqWriter, 'in-sequence.annotations')
scheme.addActorsBinding(seqReader, 'sequence', seqWriter, 'in-sequence.sequence')
scheme.setElementAttribute(seqWriter, 'document-format', 'genbank')
result = scheme.launch()
print
result

scheme.cleanUp()
u2py.releaseContext()
