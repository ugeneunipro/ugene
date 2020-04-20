<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>KrakenClassifyValidator</name>
    <message>
        <source>The database folder &quot;%1&quot; doesn&apos;t exist.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The mandatory database file &quot;%1&quot; doesn&apos;t exist.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>MinimizerLengthValidator</name>
    <message>
        <source>Minimizer length has to be less than K-mer length</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not full: file &apos;%1&apos; is missing.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Kraken external tool support</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The plugin supports Kraken: taxonomic sequence classification system (https://ccb.jhu.edu/software/kraken/)</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::KrakenBuildTask</name>
    <message>
        <source>%1 Kraken database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Build</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shrink</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Sequential execution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input database URL is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input database doesn&apos;t exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>New database URL is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Genomes URLs list to build database from is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Cannot find taxonomy data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Can&apos;t create a symbolic link to the taxonomy file: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Taxonomy data are not set</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::KrakenClassifyTask</name>
    <message>
        <source>Classify reads with Kraken</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Cannot open classification report: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Duplicate sequence name &apos;%1&apos; have been detected in the classification output.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Broken Kraken report : %1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::KrakenSupport</name>
    <message>
        <source>The tool is used to build a Kraken database.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The tool is used to classify a set of sequences. It does this by examining the k-mers within a read and querying a database with those k-mers.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Build</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shrink</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::KrakenSupportPlugin</name>
    <message>
        <source>Kraken is a taxonomic sequence classifier that assigns taxonomic labels to short DNA reads.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::KrakenBuildPrompter</name>
    <message>
        <source>Use custom data to build %1 Kraken database.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shrink Kraken database %1 to %2.</source>
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
        <source>Output Kraken database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>URL to the folder with the Kraken database.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Select &quot;Build&quot; to create a new database from a genomic library (--build).&lt;br&gt;&lt;br&gt;Select &quot;Shrink&quot; to shrink an existing database to have only specified number of k-mers (--shrink).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Name of the input database that should be shrunk (corresponds to --db that is used with --shrink).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Name of the output Kraken database (corresponds to --db that is used with --build, and to --new-db that is used with --shrink).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Genomic library</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Genomes that should be used to build the database.&lt;br&gt;&lt;br&gt;The genomes should be specified in FASTA format. The sequence IDs must contain either a GI number or a taxonomy ID (see documentation for details).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Number of k-mers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The new database will contain the specified number of k-mers selected from across the input database.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>K-mer length</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>K-mer length in bp (--kmer-len).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Minimizer length</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Minimizer length in bp (--minimizer-len).&lt;br&gt;&lt;br&gt;The minimizers serve to keep k-mers that are adjacent in query sequences close to each other in the database, which allows Kraken to exploit the CPU cache.&lt;br&gt;&lt;br&gt;Changing the value of the parameter can significantly affect the speed of Kraken, and neither increasing nor decreasing of the value will guarantee faster or slower speed.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Maximum database size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>By default, a full database build is done.&lt;br&gt;&lt;br&gt;To shrink the database before the full build, input the size of the database in Mb (this corresponds to the --max-db-size parameter, but Mb is used instead of Gb). The size is specified together for the database and the index.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shrink block offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>When shrinking, select the k-mer that is NUM positions from the end of a block of k-mers (--shrink-block-offset).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Clean</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Remove unneeded files from a built database to reduce the disk usage (--clean).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Work on disk</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Perform most operations on disk rather than in RAM (this will slow down build in most cases).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Jellyfish hash size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The &quot;kraken-build&quot; tool uses the &quot;jellyfish&quot; tool. This parameter specifies the hash size for Jellyfish.&lt;br&gt;&lt;br&gt;Supply a smaller hash size to Jellyfish, if you encounter problems with allocating enough memory during the build process (--jellyfish-hash-size).&lt;br&gt;&lt;br&gt;By default, the parameter is not used.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Number of threads</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Use multiple threads (--threads).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No limit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Skip</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Build Kraken Database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Build a Kraken database from a genomic library or shrink a Kraken database.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::KrakenClassifyPrompter</name>
    <message>
        <source>Classify sequences from &lt;u&gt;%1&lt;/u&gt; with Kraken, use %2 database.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Classify paired-end reads from &lt;u&gt;%1&lt;/u&gt; with Kraken, use %2 database.</source>
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
        <source>Input sequences</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>URL(s) to FASTQ or FASTA file(s) should be provided.

In case of SE reads or contigs use the &quot;Input URL 1&quot; slot only.

In case of PE reads input &quot;left&quot; reads to &quot;Input URL 1&quot;, &quot;right&quot; reads to &quot;Input URL 2&quot;.

See also the &quot;Input data&quot; parameter of the element.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Kraken Classification</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A map of sequence names with the associated taxonomy IDs, classified by Kraken.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Input data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To classify single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to &quot;SE reads or contigs&quot;.&lt;br&gt;&lt;br&gt;To classify paired-end (PE) reads, set the value to &quot;PE reads&quot;.&lt;br&gt;&lt;br&gt;One or two slots of the input port are used depending on the value of the parameter. Pass URL(s) to data to these slots.&lt;br&gt;&lt;br&gt;The input files should be in FASTA or FASTQ formats.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Database</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>A path to the folder with the Kraken database files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Output file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Specify the output file name.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Quick operation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop classification of an input read after the certain number of hits.&lt;br&gt;&lt;br&gt;The value can be specified in the &quot;Minimum number of hits&quot; parameter.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Minimum number of hits</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The number of hits that are required to declare an input sequence classified.&lt;br&gt;&lt;br&gt;This can be especially useful with custom databases when testing to see if sequences either do or do not belong to a particular genome.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Number of threads</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Use multiple threads (--threads).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Load database into memory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Load the Kraken database into RAM (--preload).&lt;br&gt;&lt;br&gt;This can be useful to improve the speed. The database size should be less than the RAM size.&lt;br&gt;&lt;br&gt;The other option to improve the speed is to store the database on ramdisk. Set this parameter to &quot;False&quot; in this case.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Classify Sequences with Kraken</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Kraken is a taxonomic sequence classifier that assigns taxonomic labels to short DNA reads. It does this by examining the k-mers within a read and querying a database with those.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::KrakenClassifyWorker</name>
    <message>
        <source>There were %1 input reads, %2 reads were classified.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
