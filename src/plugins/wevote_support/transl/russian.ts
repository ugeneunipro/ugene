<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>QObject</name>
    <message>
        <source>WEVOTE external tool support</source>
        <translation></translation>
    </message>
    <message>
        <source>The plugin supports WEVOTE (WEighted VOting Taxonomic idEntification) - a tool that implements an algorithm to improve the reads classification. (https://github.com/aametwally/WEVOTE)</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::WevotePrompter</name>
    <message>
        <source>Ensemble classification data, produced by other tools.</source>
        <translation></translation>
    </message>
    <message>
        <source>Input URL</source>
        <translation></translation>
    </message>
    <message>
        <source>Input URL.</source>
        <translation></translation>
    </message>
    <message>
        <source>Input classification CSV file</source>
        <translation></translation>
    </message>
    <message>
        <source>Input a CSV file in the following format:
1) a sequence name
2) taxID from the first tool
3) taxID from the second tool
4) etc.</source>
        <translation></translation>
    </message>
    <message>
        <source>WEVOTE Classification</source>
        <translation></translation>
    </message>
    <message>
        <source>A map of sequence names with the associated taxonomy IDs.</source>
        <translation></translation>
    </message>
    <message>
        <source>Penalty</source>
        <translation></translation>
    </message>
    <message>
        <source>Score penalty for disagreements (-k)</source>
        <translation></translation>
    </message>
    <message>
        <source>Number of agreed tools</source>
        <translation></translation>
    </message>
    <message>
        <source>Specify the minimum number of tools agreed on WEVOTE decision (-a).</source>
        <translation></translation>
    </message>
    <message>
        <source>Score threshold</source>
        <translation></translation>
    </message>
    <message>
        <source>Score threshold (-s)</source>
        <translation></translation>
    </message>
    <message>
        <source>Number of threads</source>
        <translation></translation>
    </message>
    <message>
        <source>Use multiple threads (-n).</source>
        <translation></translation>
    </message>
    <message>
        <source>Output file</source>
        <translation></translation>
    </message>
    <message>
        <source>Specify the output text file name.</source>
        <translation></translation>
    </message>
    <message>
        <source>Auto</source>
        <translation></translation>
    </message>
    <message>
        <source>Improve Classification with WEVOTE</source>
        <translation></translation>
    </message>
    <message>
        <source>WEVOTE (WEighted VOting Taxonomic idEntification) is a metagenome shortgun sequencing DNA reads classifier based on an ensemble of other classification methods (Kraken, CLARK, etc.).</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::WevoteWorker</name>
    <message>
        <source>Empty input file URL in the message</source>
        <translation></translation>
    </message>
    <message>
        <source>There were %1 input reads, %2 reads were classified.</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>U2::PrepareWevoteTaxonomyDataTask</name>
    <message>
        <source>Prepare taxonomy data for WEVOTE</source>
        <translation></translation>
    </message>
    <message>
        <source>Taxonomy data are missed</source>
        <translation></translation>
    </message>
    <message>
        <source>Taxonomy file &apos;%1&apos; is not found.</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>U2::WevoteSupport</name>
    <message>
        <source>WEVOTE (WEighted VOting Taxonomic idEntification) is a metagenome shortgun sequencing DNA reads classifier based on an ensemble of other classification methods. In UGENE one can use the following methods: Kraken, CLARK, DIAMOND.</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>U2::WevoteTask</name>
    <message>
        <source>Improve classification with WEVOTE</source>
        <translation></translation>
    </message>
    <message>
        <source>Wevote output file not found</source>
        <translation></translation>
    </message>
    <message>
        <source>Number of agreed tools is less than zero</source>
        <translation></translation>
    </message>
    <message>
        <source>Score threshold is less than zero</source>
        <translation></translation>
    </message>
    <message>
        <source>Number of threads is less or equal zero</source>
        <translation></translation>
    </message>
    <message>
        <source>Input file URL is empty</source>
        <translation></translation>
    </message>
    <message>
        <source>Output file URL is empty</source>
        <translation></translation>
    </message>
    <message>
        <source>Can&apos;t find prepared taxonomy for WEVOTE</source>
        <translation></translation>
    </message>
    <message>
        <source>Can&apos;t overwrite the file &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <source>Too few columns in the result file.</source>
        <translation></translation>
    </message>
    <message>
        <source>Can&apos;t parse the taxID: &quot;%1&quot;</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>WevoteValidator</name>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation></translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not full: file &apos;%1&apos; is missing.</source>
        <translation></translation>
    </message>
</context>
</TS>
