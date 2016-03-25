<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>HMMBuildDialog</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="32"/>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="49"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="14"/>
        <source>HMM Build</source>
        <translation>HMM Build</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="22"/>
        <source>Multiple alignment file:</source>
        <translation>Multiple alignment file:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="39"/>
        <source>File to save HMM profile:</source>
        <translation>File to save HMM profile:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="58"/>
        <source>Expert options</source>
        <translation>Expert options</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="70"/>
        <source>Name can be any string of non-whitespace characters (e.g. one ”word”).</source>
        <translation>Name can be any string of non-whitespace characters (e.g. one ”word”).</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="73"/>
        <source>Name this HMM:</source>
        <translation>Name this HMM:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="84"/>
        <source>
             By default, the model is configured to find one or more nonoverlapping alignments to the complete model:
             multiple global alignments with respect to the model, and local with respect to the sequence
         </source>
        <translation>By default, the model is configured to find one or more nonoverlapping alignments to the complete model:
multiple global alignments with respect to the model, and local with respect to the sequence</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="90"/>
        <source>Default (hmmls) behaviour:</source>
        <translation>Default (hmmls) behaviour:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="120"/>
        <source>
             Configure the model for finding multiple domains per sequence, where each domain can be a local (fragmentary) alignment.
             This is analogous to the old hmmfs program of HMMER 1.
         </source>
        <translation>Configure the model for finding multiple domains per sequence, where each domain can be a local (fragmentary) alignment.
This is analogous to the old hmmfs program of HMMER 1.
</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="126"/>
        <source>Emulate hmmfs behaviour:</source>
        <translation>Emulate hmmfs behaviour:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="153"/>
        <source>
             Configure the model for finding a single global alignment to a target sequence,
             analogous to the old hmms program of HMMER 1.
         </source>
        <translation>Configure the model for finding a single global alignment to a target sequence,
analogous to the old hmms program of HMMER 1.
</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="159"/>
        <source>Emulate hmms behaviour:</source>
        <translation>Emulate hmms behaviour:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="186"/>
        <source>
             Configure the model for finding a single local alignment per target sequence.
             This is analogous to the standard Smith/Waterman algorithm or the hmmsw program of HMMER 1.
         </source>
        <translation>Configure the model for finding a single local alignment per target sequence.
This is analogous to the standard Smith/Waterman algorithm or the hmmsw program of HMMER 1.
</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="192"/>
        <source>Emulate hmmsw behaviour:</source>
        <translation>Emulate hmmsw behaviour:</translation>
    </message>
</context>
<context>
    <name>HMMBuildWorker</name>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="89"/>
        <source>Fix the length of the random sequences to &lt;n&gt;, where &lt;n&gt; is a positive (and reasonably sized) integer. &lt;p&gt;The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.</source>
        <translation>Fix the length of the random sequences to &lt;n&gt;, where &lt;n&gt; is a positive (and reasonably sized) integer. 
The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="92"/>
        <source>Mean length of the synthetic sequences, positive real number. The default value is 325.</source>
        <translation>Mean length of the synthetic sequences, positive real number. The default value is 325.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="94"/>
        <source>Number of synthetic sequences. If &lt;n&gt; is less than about 1000, the fit to the EVD may fail. &lt;p&gt;Higher numbers of &lt;n&gt; will give better determined EVD parameters. &lt;p&gt;The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.</source>
        <translation>Number of synthetic sequences. If &lt;n&gt; is less than about 1000, the fit to the EVD may fail. 
Higher numbers of &lt;n&gt; will give better determined EVD parameters. 
The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="98"/>
        <source>Standard deviation of the synthetic sequence length. A positive number. &lt;p&gt;The default is 200. Note that the Gaussian is left-truncated so that no sequences have lengths &lt;= 0.</source>
        <translation>Standard deviation of the synthetic sequence length. A positive number. &lt;p&gt;The default is 200. Note that the Gaussian is left-truncated so that no sequences have lengths &lt;= 0.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="101"/>
        <source>The random seed, where &lt;n&gt; is a positive integer. &lt;p&gt;The default is to use time() to generate a different seed for each run, &lt;p&gt;which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. &lt;p&gt;You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.</source>
        <translation>The random seed, where &lt;n&gt; is a positive integer. The default is to use time() to generate a different seed for each run, 
which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. 
You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.</translation>
    </message>
</context>
<context>
    <name>HMMCalibrateDialog</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="22"/>
        <source>HMM file: </source>
        <translation>HMM file: </translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="32"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="297"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="41"/>
        <source>Expert options</source>
        <translation>Expert options</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="53"/>
        <source>
             Fix the length of the random sequences to n
                 , where n is a positive (and reasonably sized) integer. 
The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.</source>
        <translation>Fix the length of the random sequences to n, where n is a positive (and reasonably sized) integer. 
The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="59"/>
        <source>Fix the length of the random sequences to:</source>
        <translation>Fix the length of the random sequences to:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="104"/>
        <source>Mean length of the synthetic sequences:</source>
        <translation>Mean length of the synthetic sequences:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="146"/>
        <source>
             Number of synthetic sequences.
             If n is less than about 1000, the fit to the EVD may fail
             Higher numbers of n will give better determined EVD parameters. 
             The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.</source>
        <translation>Number of synthetic sequences.
If n is less than about 1000, the fit to the EVD may fail
Higher numbers of n will give better determined EVD parameters. 
The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="153"/>
        <source>Number of synthetic sequences:</source>
        <translation>Number of synthetic sequences:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="195"/>
        <source>
             Standard deviation of the synthetic sequence length.
             A positive number. The default is 200.
             Note that the Gaussian is left-truncated so that no sequences have lengths less or equal 0.
         </source>
        <translation>Standard deviation of the synthetic sequence length.
A positive number. The default is 200.
Note that the Gaussian is left-truncated so that no sequences have lengths less or equal 0.
</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="202"/>
        <source>Standard deviation:</source>
        <translation>Standard deviation:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="238"/>
        <source>
             The random seed, where n is a positive integer. 
             The default is to use time() to generate a different seed for each run, 
             which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. 
             You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.</source>
        <translation>The random seed, where n is a positive integer. 
The default is to use time() to generate a different seed for each run, 
which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. 
You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="245"/>
        <source>Random seed:</source>
        <translation>Random seed:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="278"/>
        <source>Save calibrated profile to file</source>
        <translation>Save calibrated profile to file</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="290"/>
        <source>Save calibrated profile to file:</source>
        <translation>Save calibrated profile to file:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="14"/>
        <source>HMM Calibrate</source>
        <translation>HMM Calibrate</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="101"/>
        <source>Mean length of the synthetic sequences, positive real number. The default value is 325.</source>
        <translation>Mean length of the synthetic sequences, positive real number. The default value is 325.</translation>
    </message>
</context>
<context>
    <name>HMMSearchDialog</name>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="23"/>
        <source>HMM Search</source>
        <translation>HMM Search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="37"/>
        <source>File with HMM profile:</source>
        <translation>File with HMM profile:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="47"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="62"/>
        <source>Expert options</source>
        <translation>Expert options</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="77"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="80"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="83"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="226"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="229"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="232"/>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="76"/>
        <source>E-value filtering can be used to exclude low-probability hits from result.</source>
        <translation>E-value filtering can be used to exclude low-probability hits from result.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="86"/>
        <source>Filter results with E-value greater then:</source>
        <translation>Filter results with E-value greater then:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="119"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="122"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="125"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="135"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="138"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="141"/>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="77"/>
        <source>Score based filtering is an alternative to E-value filtering to exclude low-probability hits from result.</source>
        <translation>Score based filtering is an alternative to E-value filtering to exclude low-probability hits from result.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="128"/>
        <source>Filter results with Score lower than:</source>
        <translation>Filter results with Score lower than:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="188"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="191"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="194"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="210"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="213"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="216"/>
        <source>Calculate the E-value scores as if we had seen a sequence database of &lt;n&gt; sequences.</source>
        <translation>Calculate the E-value scores as if we had seen a sequence database of &lt;n&gt; sequences.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="219"/>
        <source>Number of sequences in dababase:</source>
        <translation>Number of sequences in dababase:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="277"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="280"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="283"/>
        <source>Variants of algorithm</source>
        <translation>Variants of algorithm</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="241"/>
        <source>1E</source>
        <translation>1e</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="257"/>
        <source>Algorithm</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="75"/>
        <source>Calculate the E-value scores as if we had seen a sequence database of &amp;lt;n&amp;gt; sequences.</source>
        <translation>Calculate the E-value scores as if we had seen a sequence database of &lt;n&gt; sequences.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="68"/>
        <source>HMM Profile</source>
        <translation>HMM Profile</translation>
    </message>
</context>
<context>
    <name>U2::GTest_uHMMERCalibrate</name>
    <message>
        <location filename="../src/u_tests/uhmmerTests.cpp" line="570"/>
        <source>uhmmer-calibrate-subtask</source>
        <translation>HMM Calibrate</translation>
    </message>
</context>
<context>
    <name>U2::HMM2QDActor</name>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="65"/>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="150"/>
        <source>HMM2</source>
        <translation>HMM2</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="76"/>
        <source>QD HMM2 search</source>
        <translation>QD HMM2 search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="82"/>
        <source>Power of e-value must be less or equal to zero. Using default value: 1e-1</source>
        <translation>Power of e-value must be less or equal to zero. Using default value: 1e-1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="151"/>
        <source>Searches HMM signals in a sequence with one or more profile HMM2 and saves the results as annotations.</source>
        <translation>Searches HMM signals in a sequence with one or more profile HMM2 and saves the results as annotations.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="157"/>
        <source>Profile HMM</source>
        <translation>Profile HMM</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="158"/>
        <source>Semicolon-separated list of input HMM files.</source>
        <translation>Semicolon-separated list of input HMM files.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="161"/>
        <source>Min Length</source>
        <translation>Min Length</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="162"/>
        <source>Minimum length of a result region.</source>
        <translation>Minimum length of a result region.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="165"/>
        <source>Max Length</source>
        <translation>Max Length</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="166"/>
        <source>Maximum length of a result region.</source>
        <translation>Maximum length of a result region.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="169"/>
        <source>Filter by High E-value</source>
        <translation>Filter by High E-value</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="170"/>
        <source>Reports domains &amp;lt;= this E-value threshold in output.</source>
        <translation>Reports domains &amp;lt;= this E-value threshold in output.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="174"/>
        <source>Filter by Low Score</source>
        <translation>Filter by Low Score</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="175"/>
        <source>Reports domains &amp;gt;= this score cutoff in output.</source>
        <translation>Reports domains &amp;gt;= this score cutoff in output.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="178"/>
        <source>Number of Sequences</source>
        <translation>Number of Sequences</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="179"/>
        <source>Specifies number of significant sequences. It is used for domain E-value calculations.</source>
        <translation>Specifies number of significant sequences. It is used for domain E-value calculations.</translation>
    </message>
</context>
<context>
    <name>U2::HMMADVContext</name>
    <message>
        <source>Search with HMM model...</source>
        <translation type="vanished">Search with HMM model...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="257"/>
        <source>Find HMM signals with HMMER2...</source>
        <translation>Find HMM signals with HMMER2...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="274"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="274"/>
        <source>No sequences found</source>
        <translation>No sequences found</translation>
    </message>
</context>
<context>
    <name>U2::HMMBuildDialogController</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="58"/>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="175"/>
        <source>Build</source>
        <translation>Build</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="59"/>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="176"/>
        <source>Close</source>
        <translation>Close</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="80"/>
        <source>Select file with alignment</source>
        <translation>Select file with alignment</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="92"/>
        <source>Select file with HMM profile</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="112"/>
        <source>Incorrect alignment file!</source>
        <translation>Incorrect alignment file!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="117"/>
        <source>Incorrect HMM file!</source>
        <translation>Incorrect HMM file!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="133"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="142"/>
        <source>Starting build process</source>
        <translation>Starting build process</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="145"/>
        <source>Hide</source>
        <translation>Hide</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="146"/>
        <source>Cancel</source>
        <translation>Cancel</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="169"/>
        <source>HMM build finished with errors: %1</source>
        <translation>HMM build finished with errors: %1</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="171"/>
        <source>HMM build canceled</source>
        <translation>HMM build canceled</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="173"/>
        <source>HMM build finished successfuly!</source>
        <translation>HMM build finished successfuly!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="185"/>
        <source>Progress: %1%</source>
        <translation>Progress: %1%</translation>
    </message>
</context>
<context>
    <name>U2::HMMBuildTask</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="325"/>
        <source>Build HMM profile &apos;%1&apos;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="342"/>
        <source>Multiple alignment is empty</source>
        <translation>Multiple alignment is empty</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="346"/>
        <source>Multiple alignment is of 0 length</source>
        <translation>Multiple alignment is of 0 length</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="351"/>
        <source>Invalid alphabet! Only amino and nucleic alphabets are supported</source>
        <translation>Invalid alphabet! Only amino and nucleic alphabets are supported</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="359"/>
        <source>Error creating MSA structure</source>
        <translation>Error creating MSA structure</translation>
    </message>
</context>
<context>
    <name>U2::HMMBuildToFileTask</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="310"/>
        <source>none</source>
        <translation>none</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="196"/>
        <source>Build HMM profile &apos;%1&apos; -&gt; &apos;%2&apos;</source>
        <translation>Build HMM profile &apos;%1&apos; -&gt; &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="209"/>
        <source>Error reading alignment file</source>
        <translation>Error reading alignment file</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="226"/>
        <source>Build HMM profile to &apos;%1&apos;</source>
        <translation>Build HMM profile to &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="251"/>
        <source>Incorrect input file</source>
        <translation>Incorrect input file</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="256"/>
        <source>Alignment object not found!</source>
        <translation>Alignment object not found!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="298"/>
        <source>Source alignment</source>
        <translation>Source alignment</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="299"/>
        <source>Profile name</source>
        <translation>Profile name</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="301"/>
        <source>Task was not finished</source>
        <translation>Task was not finished</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="306"/>
        <source>Profile file</source>
        <translation>Profile file</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="307"/>
        <source>Expert options</source>
        <translation>Expert options</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateDialogController</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="51"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="160"/>
        <source>Calibrate</source>
        <translation>Calibrate</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="52"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="161"/>
        <source>Close</source>
        <translation>Close</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="65"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="74"/>
        <source>Select file with HMM model</source>
        <translation>Select file with HMM model</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="95"/>
        <source>Incorrect HMM file!</source>
        <translation>Incorrect HMM file!</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="101"/>
        <source>Illegal fixed length value!</source>
        <translation>Illegal fixed length value!</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="118"/>
        <source>Invalid output file name</source>
        <translation>Invalid output file name</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="124"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="134"/>
        <source>Starting calibration process</source>
        <translation>Starting calibration process</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="137"/>
        <source>Hide</source>
        <translation>Hide</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="138"/>
        <source>Cancel</source>
        <translation>Cancel</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="154"/>
        <source>Calibration finished with errors: %1</source>
        <translation>Calibration finished with errors: %1</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="156"/>
        <source>Calibration was cancelled</source>
        <translation>Calibration was cancelled</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="158"/>
        <source>Calibration finished successfuly!</source>
        <translation>Calibration finished successfuly!</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="168"/>
        <source>Progress: %1%</source>
        <translation>Progress: %1%</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateParallelSubTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="137"/>
        <source>Parallel HMM calibration subtask</source>
        <translation>Parallel HMM calibration subtask</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateParallelTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="35"/>
        <source>HMM calibrate &apos;%1&apos;</source>
        <translation>HMM calibrate &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="17"/>
        <source>HMM calibrate &apos;%1&apos;</source>
        <translation>HMM calibrate &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateToFileTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="161"/>
        <source>HMM calibrate &apos;%1&apos;</source>
        <translation>HMM calibrate &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="200"/>
        <source>Source profile</source>
        <translation>Source profile</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="203"/>
        <source>Task was not finished</source>
        <translation>Task was not finished</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="208"/>
        <source>Result profile</source>
        <translation>Result profile</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="209"/>
        <source>Expert options</source>
        <translation>Expert options</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="211"/>
        <source>Number of random sequences to sample</source>
        <translation>Number of random sequences to sample</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="212"/>
        <source>Random number seed</source>
        <translation>Random number seed</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="213"/>
        <source>Mean of length distribution</source>
        <translation>Mean of length distribution</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="214"/>
        <source>Standard deviation of length distribution</source>
        <translation>Standard deviation of length distribution</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="216"/>
        <source>Calculated evidence (mu , lambda)</source>
        <translation>Calculated evidence (mu , lambda)</translation>
    </message>
</context>
<context>
    <name>U2::HMMCreateWPoolTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="98"/>
        <source>Initialize parallel context</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>U2::HMMIO</name>
    <message>
        <location filename="../src/HMMIO.cpp" line="339"/>
        <source>ALPH must precede NULE in HMM save files</source>
        <translation>ALPH must precede NULE in HMM save files</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="215"/>
        <location filename="../src/HMMIO.cpp" line="231"/>
        <location filename="../src/HMMIO.cpp" line="391"/>
        <location filename="../src/HMMIO.cpp" line="397"/>
        <location filename="../src/HMMIO.cpp" line="423"/>
        <location filename="../src/HMMIO.cpp" line="454"/>
        <location filename="../src/HMMIO.cpp" line="479"/>
        <source>Illegal line</source>
        <translation>Illegal line</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="219"/>
        <source>File format is not supported</source>
        <translation>File format is not supported</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="277"/>
        <location filename="../src/HMMIO.cpp" line="282"/>
        <location filename="../src/HMMIO.cpp" line="289"/>
        <location filename="../src/HMMIO.cpp" line="294"/>
        <location filename="../src/HMMIO.cpp" line="301"/>
        <location filename="../src/HMMIO.cpp" line="306"/>
        <location filename="../src/HMMIO.cpp" line="313"/>
        <location filename="../src/HMMIO.cpp" line="319"/>
        <location filename="../src/HMMIO.cpp" line="328"/>
        <location filename="../src/HMMIO.cpp" line="333"/>
        <location filename="../src/HMMIO.cpp" line="345"/>
        <location filename="../src/HMMIO.cpp" line="354"/>
        <location filename="../src/HMMIO.cpp" line="359"/>
        <location filename="../src/HMMIO.cpp" line="403"/>
        <location filename="../src/HMMIO.cpp" line="408"/>
        <location filename="../src/HMMIO.cpp" line="412"/>
        <location filename="../src/HMMIO.cpp" line="429"/>
        <location filename="../src/HMMIO.cpp" line="433"/>
        <location filename="../src/HMMIO.cpp" line="438"/>
        <location filename="../src/HMMIO.cpp" line="445"/>
        <location filename="../src/HMMIO.cpp" line="460"/>
        <location filename="../src/HMMIO.cpp" line="469"/>
        <location filename="../src/HMMIO.cpp" line="485"/>
        <location filename="../src/HMMIO.cpp" line="493"/>
        <location filename="../src/HMMIO.cpp" line="499"/>
        <location filename="../src/HMMIO.cpp" line="504"/>
        <source>Invalid file structure near %1</source>
        <translation>Invalid file structure near %1</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="248"/>
        <location filename="../src/HMMIO.cpp" line="374"/>
        <location filename="../src/HMMIO.cpp" line="378"/>
        <source>Value is illegal: %1</source>
        <translation>Value is illegal: %1</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="72"/>
        <source>Alphabet is not set</source>
        <translation>Alphabet is not set</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="382"/>
        <source>Value is not set for &apos;%1&apos;</source>
        <translation>Value is not set for &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="518"/>
        <source>No &apos;//&apos; symbol found</source>
        <translation>No &apos;//&apos; symbol found</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="636"/>
        <source>HMM models</source>
        <translation>HMM models</translation>
    </message>
</context>
<context>
    <name>U2::HMMMSAEditorContext</name>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="215"/>
        <source>Build HMMER2 profile</source>
        <translation>Build HMMER2 profile</translation>
    </message>
</context>
<context>
    <name>U2::HMMReadTask</name>
    <message>
        <location filename="../src/HMMIO.cpp" line="651"/>
        <source>Read HMM profile &apos;%1&apos;.</source>
        <translation>Read HMM profile &apos;%1&apos;.</translation>
    </message>
</context>
<context>
    <name>U2::HMMSearchDialogController</name>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="114"/>
        <source>Select file with HMM model</source>
        <translation>Select file with HMM model</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="131"/>
        <source>HMM file not set!</source>
        <translation>HMM file not set!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="148"/>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="154"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="154"/>
        <source>Cannot create an annotation object. Please check settings</source>
        <translation>Cannot create an annotation object. Please check settings</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="165"/>
        <source>Starting search process</source>
        <translation>Starting search process</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="168"/>
        <source>Hide</source>
        <translation>Hide</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="169"/>
        <source>Cancel</source>
        <translation>Cancel</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="184"/>
        <source>HMM search finished with error: %1</source>
        <translation>HMM search finished with error: %1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="186"/>
        <source>HMM search finished successfuly!</source>
        <translation>HMM search finished successfuly!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="196"/>
        <source>Progress: %1%</source>
        <translation>Progress: %1%</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="77"/>
        <source>Cell BE optimized</source>
        <translation>Cell BE optimized</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="61"/>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="188"/>
        <source>Search</source>
        <translation>Search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="62"/>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="189"/>
        <source>Close</source>
        <translation>Close</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="80"/>
        <source>SSE optimized</source>
        <translation>SSE optimized</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="82"/>
        <source>Conservative</source>
        <translation>Conservative</translation>
    </message>
</context>
<context>
    <name>U2::HMMSearchTask</name>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="48"/>
        <source>HMM Search</source>
        <translation>HMM Search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="266"/>
        <source>Invalid HMM alphabet!</source>
        <translation>Invalid HMM alphabet!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="270"/>
        <source>Invalid sequence alphabet!</source>
        <translation>Invalid sequence alphabet!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="298"/>
        <source>Amino translation is not available for the sequence alphabet!</source>
        <translation>Amino translation is not available for the sequence alphabet!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="332"/>
        <source>Parallel HMM search</source>
        <translation>Parallel HMM search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="40"/>
        <source>HMM search with &apos;%1&apos;</source>
        <translation>HMM search with &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::HMMSearchToAnnotationsTask</name>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="215"/>
        <source>HMM search, file &apos;%1&apos;</source>
        <translation>HMM search, file &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="221"/>
        <source>RAW alphabet is not supported!</source>
        <translation>RAW alphabet is not supported!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="237"/>
        <source>Annotation object was removed</source>
        <translation>Annotation object was removed</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="264"/>
        <source>HMM profile used</source>
        <translation>HMM profile used</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="267"/>
        <source>Task was not finished</source>
        <translation>Task was not finished</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="272"/>
        <source>Result annotation table</source>
        <translation>Result annotation table</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="273"/>
        <source>Result annotation group</source>
        <translation>Result annotation group</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="274"/>
        <source>Result annotation name</source>
        <translation>Result annotation name</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="277"/>
        <source>Results count</source>
        <translation>Results count</translation>
    </message>
</context>
<context>
    <name>U2::HMMWriteTask</name>
    <message>
        <location filename="../src/HMMIO.cpp" line="671"/>
        <source>Write HMM profile &apos;%1&apos;</source>
        <translation>Write HMM profile &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMBuildPrompter</name>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="182"/>
        <source>For each MSA from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>For each MSA from &lt;u&gt;%1&lt;/u&gt;,</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="186"/>
        <source> and calibrate</source>
        <translation> and calibrate</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="188"/>
        <source>default</source>
        <translation>default</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="188"/>
        <source>custom</source>
        <translation>custom</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="190"/>
        <source>%1 build%2 HMM profile using &lt;u&gt;%3&lt;/u&gt; settings.</source>
        <translation>%1 build%2 HMM profile using &lt;u&gt;%3&lt;/u&gt; settings.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMBuildWorker</name>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="296"/>
        <source>Built HMM profile</source>
        <translation>Built HMM profile</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="302"/>
        <source>Calibrated HMM profile</source>
        <translation>Calibrated HMM profile</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="122"/>
        <source>HMM2 Build</source>
        <translation>HMM2 Build</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="65"/>
        <source>HMM profile</source>
        <translation>HMM profile</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="75"/>
        <source>HMM strategy</source>
        <translation>HMM strategy</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="75"/>
        <source>Specifies kind of alignments you want to allow.</source>
        <translation>Specifies kind of alignments you want to allow.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="76"/>
        <source>Profile name</source>
        <translation>Profile name</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="76"/>
        <source>Descriptive name of the HMM profile.</source>
        <translation>Descriptive name of the HMM profile.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="83"/>
        <source>Calibrate profile</source>
        <translation>Calibrate profile</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="85"/>
        <source>Parallel calibration</source>
        <translation>Parallel calibration</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="88"/>
        <source>Fixed length of samples</source>
        <translation>Fixed length of samples</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="91"/>
        <source>Mean length of samples</source>
        <translation>Mean length of samples</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="93"/>
        <source>Number of samples</source>
        <translation>Number of samples</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="97"/>
        <source>Standard deviation</source>
        <translation>Standard deviation</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="100"/>
        <source>Random seed</source>
        <translation>Random seed</translation>
    </message>
    <message>
        <source>HMM Build</source>
        <translation type="obsolete">HMM Build</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="156"/>
        <source>Default</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="218"/>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="219"/>
        <source>Incorrect value for seed parameter</source>
        <translation>Incorrect value for seed parameter</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="237"/>
        <source>Schema name not specified. Using default value: &apos;%1&apos;</source>
        <translation>Schema name not specified. Using default value: &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="63"/>
        <source>Input MSA</source>
        <translation>Input MSA</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="64"/>
        <source>Input multiple sequence alignment for building statistical model.</source>
        <translation>Input multiple sequence alignment for building statistical model.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="65"/>
        <source>Produced HMM profile</source>
        <translation>Produced HMM profile</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="83"/>
        <source>Enables/disables optional profile calibration.&lt;p&gt;An empirical HMM calibration costs time but it only has to be done once per model, and can greatly increase the sensitivity of a database search.</source>
        <translation>Enables/disables optional profile calibration.&lt;p&gt;An empirical HMM calibration costs time but it only has to be done once per model, and can greatly increase the sensitivity of a database search.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="86"/>
        <source>Number of parallel threads that the calibration will run in.</source>
        <translation>Number of parallel threads that the calibration will run in.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="122"/>
        <source>Builds a HMM profile from a multiple sequence alignment.&lt;p&gt;The HMM profile is a statistical model which captures position-specific information about how conserved each column of the alignment is, and which residues are likely.</source>
        <translation>Builds a HMM profile from a multiple sequence alignment.&lt;p&gt;The HMM profile is a statistical model which captures position-specific information about how conserved each column of the alignment is, and which residues are likely.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMLib</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="62"/>
        <source>HMM Profile</source>
        <translation>HMM Profile</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="132"/>
        <location filename="../src/HMMIOWorker.cpp" line="149"/>
        <source>HMM profile</source>
        <translation>HMM profile</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="133"/>
        <source>Location</source>
        <translation>Location</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="156"/>
        <source>Read HMM2 Profile</source>
        <translation>Read HMM2 Profile</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="143"/>
        <source>Write HMM2 Profile</source>
        <translation>Write HMM2 Profile</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="70"/>
        <source>HMMER2 Tools</source>
        <translation>HMMER2 Tools</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="132"/>
        <source>Input HMM profile</source>
        <translation>Input HMM profile</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="133"/>
        <source>Location hint for the target file.</source>
        <translation>Location hint for the target file.</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="143"/>
        <source>Saves all input HMM profiles to specified location.</source>
        <translation>Saves all input HMM profiles to specified location.</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="156"/>
        <source>Reads HMM profiles from file(s). The files can be local or Internet URLs.</source>
        <translation>Reads HMM profiles from file(s). The files can be local or Internet URLs.</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="149"/>
        <source>Loaded HMM profile</source>
        <translation>Loaded HMM profile</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMReadPrompter</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="168"/>
        <source>Read HMM profile(s) from %1.</source>
        <translation>Read HMM profile(s) from %1.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMReader</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="235"/>
        <source>Loaded HMM profile from %1</source>
        <translation>Loaded HMM profile from %1</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMSearchPrompter</name>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="127"/>
        <source>For each sequence from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>For each sequence from &lt;u&gt;%1&lt;/u&gt;,</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="131"/>
        <source>Use &lt;u&gt;default&lt;/u&gt; settings.</source>
        <translation>Use &lt;u&gt;default&lt;/u&gt; settings.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="131"/>
        <source>Use &lt;u&gt;custom&lt;/u&gt; settings.</source>
        <translation>Use &lt;u&gt;custom&lt;/u&gt; settings.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="133"/>
        <source>%1 HMM signals%2. %3&lt;br&gt;Output the list of found regions annotated as &lt;u&gt;%4&lt;/u&gt;.</source>
        <translation>%1 HMM signals%2. %3&lt;br&gt;Output the list of found regions annotated as &lt;u&gt;%4&lt;/u&gt;.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="128"/>
        <source>using all profiles provided by &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>using all profiles provided by &lt;u&gt;%1&lt;/u&gt;,</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMSearchWorker</name>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="215"/>
        <source>Bad sequence supplied to input: %1</source>
        <translation>Bad sequence supplied to input: %1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="238"/>
        <source>Found %1 HMM signals</source>
        <translation>Found %1 HMM signals</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="56"/>
        <source>HMM profile</source>
        <translation>HMM profile</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="57"/>
        <source>Input sequence</source>
        <translation>Input sequence</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="59"/>
        <source>HMM annotations</source>
        <translation>HMM annotations</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="74"/>
        <source>Result annotation</source>
        <translation>Result annotation</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="75"/>
        <source>Number of seqs</source>
        <translation>Number of seqs</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="76"/>
        <source>Filter by high E-value</source>
        <translation>Filter by high E-value</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="77"/>
        <source>Filter by low score</source>
        <translation>Filter by low score</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="85"/>
        <source>HMM2 Search</source>
        <translation>HMM2 Search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="158"/>
        <source>Power of e-value must be less or equal to zero. Using default value: 1e-1</source>
        <translation>Power of e-value must be less or equal to zero. Using default value: 1e-1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="167"/>
        <source>Value for attribute name is empty, default name used</source>
        <translation>Value for attribute name is empty, default name used</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="211"/>
        <source>Find HMM signals in %1</source>
        <translation>Find HMM signals in %1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="56"/>
        <source>HMM profile(s) to search with.</source>
        <translation>HMM profile(s) to search with.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="58"/>
        <source>An input sequence (nucleotide or protein) to search in.</source>
        <translation>An input sequence (nucleotide or protein) to search in.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="60"/>
        <source>Annotations marking found similar sequence regions.</source>
        <translation>Annotations marking found similar sequence regions.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="74"/>
        <source>A name of the result annotations.</source>
        <translation>A name of the result annotations.</translation>
    </message>
    <message>
        <source>HMM Search</source>
        <translation type="obsolete">HMM Search</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="86"/>
        <source>Searches each input sequence for significantly similar sequence matches to all specified HMM profiles. In case several profiles were supplied, searches with all profiles one by one and outputs united set of annotations for each sequence.</source>
        <translation>Searches each input sequence for significantly similar sequence matches to all specified HMM profiles. In case several profiles were supplied, searches with all profiles one by one and outputs united set of annotations for each sequence.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMWritePrompter</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="174"/>
        <source>unset</source>
        <translation>unset</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="178"/>
        <source>Save HMM profile(s) from &lt;u&gt;%1&lt;/u&gt; to &lt;u&gt;%2&lt;/u&gt;.</source>
        <translation>Save HMM profile(s) from &lt;u&gt;%1&lt;/u&gt; to &lt;u&gt;%2&lt;/u&gt;.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMWriter</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="259"/>
        <source>Empty HMM passed for writing to %1</source>
        <translation>Empty HMM passed for writing to %1</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="259"/>
        <source>Unspecified URL for writing HMM</source>
        <translation>Unspecified URL for writing HMM</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="270"/>
        <source>Writing HMM profile to %1</source>
        <translation>Writing HMM profile to %1</translation>
    </message>
</context>
<context>
    <name>U2::UHMMBuild</name>
    <message>
        <location filename="../src/u_build/uhmmbuild.cpp" line="184"/>
        <source>bogus configuration choice</source>
        <translation>bogus configuration choice</translation>
    </message>
</context>
<context>
    <name>U2::uHMMPlugin</name>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="80"/>
        <source>HMM2</source>
        <translation>HMM2</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="80"/>
        <source>Based on HMMER 2.3.2 package. Biological sequence analysis using profile hidden Markov models</source>
        <translation>Based on HMMER 2.3.2 package. Biological sequence analysis using profile hidden Markov models</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="83"/>
        <source>Build HMM2 profile...</source>
        <translation>Build HMM2 profile...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="88"/>
        <source>Calibrate profile with HMMER2...</source>
        <translation>Calibrate profile with HMMER2...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="93"/>
        <source>Search with HMMER2...</source>
        <translation>Search with HMMER2...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="190"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="190"/>
        <source>Error! Select sequence in Project view or open sequence view.</source>
        <translation>Error! Select sequence in Project view or open sequence view.</translation>
    </message>
</context>
</TS>
