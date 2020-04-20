<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>ClassificationFilterValidator</name>
    <message>
        <source>Invalid taxon ID: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Set &quot;%1&quot; to &quot;True&quot; or select a taxon in &quot;%2&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ClassificationReportPrompter</name>
    <message>
        <source>Generate a detailed classification report.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Auto</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ClassificationReportValidator</name>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ClassificationReportWorkerFactory</name>
    <message>
        <source>Number of reads</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Tax ID</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>GenomicLibraryDialog</name>
    <message>
        <source>Select Genomes for Kraken Database</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Select genomes...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Custom genomes</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>TaxonSelectionDialog</name>
    <message>
        <source>Select Taxa</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationFilterPrompter</name>
    <message>
        <source>Put input sequences that belong to the specified taxonomic group(s) to separate file(s).</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationFilterTask</name>
    <message>
        <source>Filter classified reads</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Missing pair read for &apos;%1&apos;, input files: %2 and %3.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Format %1 is not supported by this task.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Warning: classification result for the ‘%1’ (from &apos;%2&apos;) hasn’t been found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Failed writing sequence to ‘%1’.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationFilterWorker</name>
    <message>
        <source>Filter by Classification</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The filter takes files with NGS reads or contigs, classified by one of the tools: Kraken, CLARK, DIAMOND, WEVOTE. For each input file it outputs a file with unspecific sequences (i.e. sequences not classified by the tools, taxID = 0) and/or one or several files with sequences that belong to specific taxonomic group(s).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input sequences and tax IDs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The following input should be provided: &lt;ul&gt;&lt;li&gt;URL(s) to FASTQ or FASTA file(s).&lt;li&gt;Corresponding taxonomy classification of sequences in the files.&lt;/ul&gt;To process single-end reads or contigs, pass the URL(s) to  the &quot;Input URL 1&quot; slot.&lt;br&gt;&lt;br&gt;To process paired-end reads, pass the URL(s) to files with the &quot;left&quot; and &quot;right&quot; reads to the &quot;Input URL 1&quot; and &quot;Input URL 2&quot; slots correspondingly.&lt;br&gt;&lt;br&gt;The taxonomy classification data are received by one of the classification tools (Kraken, CLARK, or DIAMOND) and should correspond to the input files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output File(s)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The port outputs URLs to files with NGS reads, classified by taxon IDs: one file per each specified taxon ID per each input file (or pair of files in case of PE reads).

Either one (for SE reads or contigs) or two (for PE reads) output slots are used depending on the input data.

See also the &quot;Input data&quot; parameter of the element.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input URL 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input URL 1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input URL 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input URL 2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output URL 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output URL 1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output URL 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output URL 2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Save unspecific sequences</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select &quot;True&quot; to put all unspecific input sequences (i. e. sequences with tax ID = 0) into a separate file.&lt;br&gt;Select &quot;False&quot; to skip unspecific sequences. At least one specific taxon should be selected in the &quot;Save sequences with taxID&quot; parameter in this case.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To filter single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to &quot;SE reads or contigs&quot;. Use the &quot;Input URL 1&quot; slot of the input port.&lt;br&gt;&lt;br&gt;To filter paired-end (PE) reads, set the value to &quot;PE reads&quot;. Use the &quot;&quot;Input URL 1&quot; and &quot;Input URL 2&quot; slots of the input port to input the NGS reads data.&lt;br&gt;&lt;br&gt;Also, input the classification data, received from Kraken, CLARK, or DIAMOND, to the &quot;Taxonomy classification data&quot; input slot.&lt;br&gt;&lt;br&gt;Either one or two slots of the output port are used depending on the input data.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Save sequences with taxID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select a taxID to put all sequences that belong to this taxonomic group (i. e. the specified taxID and all children in the taxonomy tree) into a separate file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Invalid taxon ID: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Set &quot;%1&quot; to &quot;True&quot; or select a taxon in &quot;%2&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No paired read provided</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Some input sequences have been skipped, as there was no classification data for them. See log for details.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>SE reads or contigs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PE reads</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>There are no sequences that belong to taxon ‘%1 (ID: %2)’ in the input ‘%3’ and ‘%4’ files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>There are no sequences that belong to taxon ‘%1 (ID: %2)’ in the input ‘%3’ file.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationReportTask</name>
    <message>
        <source>Compose classification report</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationReportWorker</name>
    <message>
        <source>Classification Report</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Based on the input taxonomy classification data the element generates a detailed report and saves it in a tab-delimited text format.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input taxonomy data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input taxonomy data from one of the classification elements (Kraken, CLARK, etc.).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Specify the output text file name.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>All taxa</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>By default, taxa with no sequences (reads or scaffolds) assigned are not included into the output. This option specifies to include all taxa.                                           &lt;br&gt;&lt;br&gt;This may be useful when output from several samples is compared.Set &quot;Sort by&quot; to &quot;Tax ID&quot; in this case.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sort by</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>It is possible to sort rows in the output file in two ways:             &lt;ul&gt;&lt;li&gt;by the number of reads, covered by the clade rooted at the taxon(i.e. &quot;clade_num&quot; for this taxID)&lt;/li&gt;             &lt;li&gt;by taxIDs&lt;/li&gt;&lt;/ul&gt;             The second option may be useful when output from different samples is compared.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::EnsembleClassificationPrompter</name>
    <message>
        <source>Ensemble classification data from other elements into %1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::EnsembleClassificationTask</name>
    <message>
        <source>Taxonomy classification for &apos;%1&apos; is missing from %2 slot</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Ensemble different classifications</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::EnsembleClassificationWorker</name>
    <message>
        <source>Ensemble Classification Data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The element ensembles data, produced by classification tools (Kraken, CLARK, DIAMOND), into a single file in CSV format. This file can be used as input for the WEVOTE classifier.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input taxonomy data 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>An input slot for taxonomy classification data.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input taxonomy data 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input taxonomy data 3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Ensembled classification</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>URL to the CSV file with ensembled classification data.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input tax data 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input tax data 1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input tax data 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input tax data 2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input tax data 3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input tax data 3.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output URL</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output URL.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Number of tools</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Specify the number of classification tools. The corresponding data should be provided using the input ports.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Specify the output file. The classification data are stored in CSV format with the following columns:&lt;ol&gt;&lt;li&gt; a sequence name&lt;li&gt;taxID from the first tool&lt;li&gt;taxID from the second tool&lt;li&gt;optionally, taxID from the third tool&lt;/ol&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Different taxonomy data do not match. Some sequence names were skipped.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Not enough classified data in the ports &apos;%1&apos; and &apos;%2&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Not enough classified data in the port &apos;%1&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Auto</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::GenomicLibraryDialog</name>
    <message>
        <source>Select</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::TaxonSelectionDialog</name>
    <message>
        <source>Select</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::TaxonomySupport</name>
    <message>
        <source>Taxonomy classification data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Taxon name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Rank</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Taxon ID</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::NgsReadsClassificationPlugin</name>
    <message>
        <source>Loading NCBI taxonomy data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>NCBI taxonomy classification data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>CLARK viral database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Minikraken 4Gb database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>DIAMOND database built from UniProt50</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>DIAMOND database built from UniProt90</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>RefSeq release human data from NCBI</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>RefSeq release viral data from NCBI</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Found the %1 at %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>CLARK bacterial and viral database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>RefSeq release bacterial data from NCBI</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>MetaPhlAn2 database</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
