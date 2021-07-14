# Associate file extensions
!macro AssociateExtSection un Ext Str IcoNum
Section "${un}${Ext}"
  SectionIn 1
  Push $1
  Push $2
  Push $3
  Push "${Ext}"
  Push "${Str}"
  Push "${IcoNum}"
  Call "${un}AssociateExtSectionGroup"
  Pop $3
  Pop $2
  Pop $1
SectionEnd
!macroend

!macro AssociateExtSectionGroup un
SectionGroup "${un}Associate file extensions"

;Project file
  !insertmacro AssociateExtSection "${un}" ".uprj"   "Unipro UGENE project file"    "0"

;ABIF format
  !insertmacro AssociateExtSection "${un}" ".ab1"    "ABIF file"                    "1"
  !insertmacro AssociateExtSection "${un}" ".abi"    "ABIF file"                    "1"
  !insertmacro AssociateExtSection "${un}" ".abif"   "ABIF file"                    "1"

;CLUSTAL format
  !insertmacro AssociateExtSection "${un}" ".aln"    "Clustal alignment file"       "1"

;EMBL format
  !insertmacro AssociateExtSection "${un}" ".em"     "EMBL file"                    "1"
  !insertmacro AssociateExtSection "${un}" ".emb"    "EMBL file"                    "1"
  !insertmacro AssociateExtSection "${un}" ".embl"   "EMBL file"                    "1"
;Swiss-Prot format
  !insertmacro AssociateExtSection "${un}" ".sw"     "Swiss-Prot file"              "1"

;FASTA format
  !insertmacro AssociateExtSection "${un}" ".fa"     "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".mpfa"   "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".fna"    "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".fsa"    "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".fas"    "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".fasta"  "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".seq"    "FASTA sequence file"          "1"
  !insertmacro AssociateExtSection "${un}" ".seqs"   "FASTA sequence file"          "1"

;FASTQ format
  !insertmacro AssociateExtSection "${un}" ".fastq"   "FASTQ file"                  "1"

;Genbank format
  !insertmacro AssociateExtSection "${un}" ".gb"      "Genbank plain text file"     "1"
  !insertmacro AssociateExtSection "${un}" ".gbk"     "Genbank plain text file"     "1"
  !insertmacro AssociateExtSection "${un}" ".gen"     "Genbank plain text file"     "1"
  !insertmacro AssociateExtSection "${un}" ".genbank" "Genbank plain text file"     "1"

;GFF format
  !insertmacro AssociateExtSection "${un}" ".gff"     "GFF format"                  "1"

;MSF format
  !insertmacro AssociateExtSection "${un}" ".msf"     "MSF multiple sequence file"  "1"

;NEWICK format
  !insertmacro AssociateExtSection "${un}" ".newick"  "NEWICK tree file"            "1"

;PDB format
  !insertmacro AssociateExtSection "${un}" ".pdb"     "Protein Data Bank file"      "1"

;SAM/BAM format
  !insertmacro AssociateExtSection "${un}" ".sam"   "SAM genome assembly"           "1"
  !insertmacro AssociateExtSection "${un}" ".bam"   "BAM genome assembly"           "1"

;SCF format -> DISABLED: overrides show desktop icon!!
;  !insertmacro AssociateExtSection "${un}" ".scf"    "SCF file"                    "1"

;Short Reads FASTA format
  !insertmacro AssociateExtSection "${un}" ".srfa"   "FASTA short reads file"       "1"
  !insertmacro AssociateExtSection "${un}" ".srfasta" "FASTA short reads file"      "1"

;STOCKHOLM format
  !insertmacro AssociateExtSection "${un}" ".sto"    "Stockholm alignment file"     "1"

;UGENE Database format
  !insertmacro AssociateExtSection "${un}" ".ugenedb" "UGENE Database"              "1"

;UGENE Query format
  !insertmacro AssociateExtSection "${un}" ".uql"    "UGENE Query Language"         "1"

;UGENE Workflow format
  !insertmacro AssociateExtSection "${un}" ".uwl"    "UGENE Workflow Language"         "1"

SectionGroupEnd
!MACROEND

Function AssociateExtSectionGroup ; (ext name)
  Pop $4 ; icon num in exe
  Pop $3 ; name
  Pop $2 ; ext
  ReadRegStr $1 HKCR $2 ""
  StrCmp $1 "" NoBackup
  StrCmp $1 $3 NoBackup
  WriteRegStr HKCR $2 "backup_val" $1
NoBackup:
  WriteRegStr HKCR $2 "" $3
  ReadRegStr $0 HKCR $3 ""
  StrCmp $0 "" 0 Skip
  WriteRegStr HKCR $3 "" $3
  WriteRegStr HKCR "$3\shell" "" "open"
  WriteRegStr HKCR "$3\DefaultIcon" "" "$INSTDIR\ugeneui.exe,$4"
Skip:
  WriteRegStr HKCR "$3\shell\open\command" "" \
    '$INSTDIR\ugeneui.exe "%1"'
FunctionEnd

Function un.AssociateExtSectionGroup ; ext name
  Pop $4 ; icon num in exe
  Pop $3 ; name
  Pop $2 ; ext
  ReadRegStr $1 HKCR $2 ""
  StrCmp $1 $3 0 NoOwn ; only do this if we own it
  ReadRegStr $1 HKCR $2 "backup_val"
  StrCmp $1 "" 0 Restore ; if backup="" then delete the whole k
  DeleteRegKey HKCR $2
  DeleteRegKey HKCR $3 ;Delete key with association settings
  Goto NoOwn
Restore:
  WriteRegStr HKCR $2 "" $1
  DeleteRegValue HKCR $2 "backup_val"
  DeleteRegKey HKCR $3 ;Delete key with association settings
NoOwn:
FunctionEnd
