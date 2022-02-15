import u2py

u2py.initContext('./')

inputFile = '../../data/samples/FASTA/chrUn_gl000231.fa'
outputFile = '../../data/samples/FASTA/ann.gb'
u2py.Scheme.launchSas('repeats-search', inputFile, outputFile)

u2py.releaseContext()
