<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>PairwiseAlignmentSmithWatermanOptionsPanelMainWidget</name>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="29"/>
        <source>Algorithm version:</source>
        <translation>Algoritma sürümü:</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="39"/>
        <source>Scoring matrix:</source>
        <translation>Puanlama matrisi:</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="62"/>
        <source>Gap penalty</source>
        <translation>Boşluk cezası</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="77"/>
        <source>Open:</source>
        <translation>Aç:</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="84"/>
        <source>Extension:</source>
        <translation>Uzantı:</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/SmithWatermanAlgorithm.cpp" line="155"/>
        <source>Smith-Waterman algorithm trying to allocate more memory than it was limited (%1 Mb). Calculation stopped.</source>
        <translation>Smith-Waterman algoritması, sınırlı olduğundan daha fazla bellek ayırmaya çalışıyor (%1 Mb). Hesaplama durduruldu.</translation>
    </message>
</context>
<context>
    <name>U2::AlignmentAlgorithmsRegistry</name>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="171"/>
        <source>Smith-Waterman</source>
        <translation>Smith-Waterman</translation>
    </message>
</context>
<context>
    <name>U2::GTest_SmithWatermnan</name>
    <message>
        <location filename="../src/SmithWatermanTests.cpp" line="216"/>
        <source>**</source>
        <translation>**</translation>
    </message>
    <message>
        <location filename="../src/SmithWatermanTests.cpp" line="219"/>
        <source>,</source>
        <translation>,</translation>
    </message>
    <message>
        <location filename="../src/SmithWatermanTests.cpp" line="226"/>
        <source>..</source>
        <translation>..</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SWPrompter</name>
    <message>
        <location filename="../src/SWWorker.cpp" line="267"/>
        <source>unset</source>
        <translation>ayarlanmadı</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="296"/>
        <source>translated</source>
        <translation>tercüme edilmiş</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="306"/>
        <source>Searches regions in each sequence from &lt;u&gt;%1&lt;/u&gt; similar to all pattern(s) taken from &lt;u&gt;%2&lt;/u&gt;. &lt;br/&gt;Percent similarity between a sequence and a pattern is %3. &lt;br/&gt;Seaches in %4 of a %5 sequence. &lt;br/&gt;Outputs the regions found annotated as %6.</source>
        <translation>Her sıradaki bölgeleri arar &lt;u&gt;%1&lt;/u&gt; alınan tüm kalıplara benzer &lt;u&gt;%2&lt;/u&gt;.&lt;br/&gt; Bir dizi ile kalıp arasındaki benzerlik yüzdesi %3&apos;tür. &lt;br/&gt;%5 dizisinin %4&apos;ünde arar. &lt;br/&gt;%6 olarak açıklanmış bulunan bölgelerin çıktısını alır.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SWWorker</name>
    <message>
        <location filename="../src/SWWorker.cpp" line="127"/>
        <source>Annotate as</source>
        <translation>Olarak ek açıklama</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="135"/>
        <source>Algorithm</source>
        <translation>Algoritma</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="102"/>
        <source>Input Data</source>
        <translation>Giriş Verileri</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="106"/>
        <source>Pattern Annotations</source>
        <translation>Desen Açıklamaları</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="107"/>
        <source>The regions found.</source>
        <translation>Bölgeler bulundu.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="128"/>
        <source>Name of the result annotations.</source>
        <translation>Sonuç ek açıklamalarının adı.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="131"/>
        <source>Min Score</source>
        <translation>Asgari Puan</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="132"/>
        <source>Minimal percent similarity between a sequence and a pattern.</source>
        <translation>Bir dizi ve bir model arasında asgari benzerlik yüzdesi.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="141"/>
        <source>Search in Translation</source>
        <translation>Çeviride Ara</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="142"/>
        <source>Translates a supplied nucleotide sequence to protein and searches in the translated sequence.</source>
        <translation>Sağlanan bir nükleotid dizisini proteine çevirir ve çevrilen dizide arama yapar.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="146"/>
        <source>Substitution Matrix</source>
        <translation>İkame Matrisi</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="147"/>
        <source>Describes the rate at which one character in a sequence changes to other character states over time.</source>
        <translation>Bir dizideki bir karakterin zaman içinde diğer karakter durumlarına geçiş oranını açıklar.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="151"/>
        <source>Filter Results</source>
        <translation>Sonuçları Filtrele</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="152"/>
        <source>Specifies either to filter the intersected results or to return all the results.</source>
        <translation>Kesişen sonuçlara filtre uygulanacağını veya tüm sonuçların döndürüleceğini belirtir.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="156"/>
        <source>Gap Open Score</source>
        <translation>Boşluk Açma Puanı</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="157"/>
        <source>Penalty for opening a gap.</source>
        <translation>Bir boşluk açmanın cezası.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="160"/>
        <source>Gap Extension Score</source>
        <translation>Boşluk Uzatma Puanı</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="161"/>
        <source>Penalty for extending a gap.</source>
        <translation>Bir boşluğu genişletmenin cezası.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="164"/>
        <source>Use Pattern Names</source>
        <translation>Desen Adlarını Kullanın</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="165"/>
        <source>Use a pattern name as an annotation name.</source>
        <translation>Ek açıklama adı olarak bir desen adı kullanın.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="168"/>
        <source>Qualifier name for pattern name</source>
        <translation>Desen adı için niteleyici adı</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="169"/>
        <source>Name of qualifier in result annotations which is containing a pattern name.</source>
        <translation>Bir model adı içeren sonuç ek açıklamalarındaki niteleyicinin adı.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="186"/>
        <source>Smith-Waterman Search</source>
        <translation>Smith-Waterman Araması</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="187"/>
        <source>Searches regions in a sequence similar to a pattern sequence. Outputs a set of annotations. &lt;p&gt;Under the hood is the well-known Smith-Waterman algorithm for performing local sequence alignment.</source>
        <translation>Bölgeleri bir model dizisine benzer bir sırayla arar. Bir dizi ek açıklama çıkarır. &lt;p&gt;Kapsam altında, yerel dizi hizalamasını gerçekleştirmek için iyi bilinen Smith-Waterman algoritması var.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="220"/>
        <source>Auto</source>
        <translation>Otomatik</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="282"/>
        <source>both strands</source>
        <translation>her iki dize</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="285"/>
        <source>direct strand</source>
        <translation>direkt dize</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="288"/>
        <source>complementary strand</source>
        <translation>tamamlayıcı dize</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="387"/>
        <source>Null sequence supplied to Smith-Waterman: %1</source>
        <translation>Smith-Waterman&apos;a sağlanan boş dizi: %1</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="483"/>
        <source>Incorrect value: pattern alphabet doesn&apos;t match sequence alphabet </source>
        <translation>Yanlış değer: kalıp alfabesi sıra alfabesiyle eşleşmiyor </translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="484"/>
        <source>Pattern symbols not matching to alphabet</source>
        <translation>Alfabeye uymayan desen sembolleri</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="424"/>
        <location filename="../src/SWWorker.cpp" line="425"/>
        <source>Incorrect value: score value must lay between 0 and 100</source>
        <translation>Yanlış değer: puan değeri 0 ile 100 arasında olmalıdır</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="435"/>
        <source>Incorrect value:  filter name incorrect, default value used</source>
        <translation>Yanlış değer:  filtre adı yanlış, varsayılan değer kullanıldı</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="442"/>
        <source>Incorrect value: result name is empty, default value used</source>
        <translation>Yanlış değer: sonuç adı boş, varsayılan değer kullanıldı</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="456"/>
        <source>Could not find complement translation for %1, searching only direct strand</source>
        <translation>%1 için tamamlayıcı çeviri bulunamadı, yalnızca doğrudan iplik aranıyor</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="396"/>
        <location filename="../src/SWWorker.cpp" line="397"/>
        <source>Invalid value: weight matrix with given name not exists</source>
        <translation>Geçersiz değer: verilen ada sahip ağırlık matrisi mevcut değil</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="98"/>
        <source>Pattern Data</source>
        <translation>Desen Verileri</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="99"/>
        <source>An input pattern sequence to search for.</source>
        <translation>Aranacak bir giriş düzeni dizisi.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="103"/>
        <source>An input reference sequence to search in.</source>
        <translation>Aranacak bir giriş referans dizisi.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="359"/>
        <source>Null pattern supplied to Smith-Waterman: %1</source>
        <translation>Smith-Waterman&apos;a sağlanan boş kalıp: %1</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="407"/>
        <source>Can&apos;t find weight matrix name: &apos;%1&apos;!</source>
        <translation>Ağırlık matrisi adı bulunamıyor: &apos;%1&apos;!</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="407"/>
        <source>&lt;empty&gt;</source>
        <translation>&lt;empty&gt;</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="414"/>
        <source>Wrong matrix selected. Alphabets do not match</source>
        <translation>Yanlış matris seçildi. Alfabe eşleşmiyor</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="472"/>
        <source>SmithWaterman algorithm not found: %1</source>
        <translation>Smith Waterman algoritması bulunamadı: %1</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="477"/>
        <location filename="../src/SWWorker.cpp" line="478"/>
        <source>Incorrect value: search pattern, pattern is empty</source>
        <translation>Yanlış değer: arama deseni, desen boş</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="500"/>
        <source>smith_waterman_task</source>
        <translation>smith_waterman_görevi</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="507"/>
        <source>Smith waterman subtasks</source>
        <translation>Smith Waterman alt görevleri</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="550"/>
        <source>Found %1 matches of pattern &apos;%2&apos;</source>
        <translation>&apos;%2&apos; kalıbının %1 eşleşmesi bulundu</translation>
    </message>
</context>
<context>
    <name>U2::PairwiseAlignmentSmithWatermanTask</name>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="549"/>
        <source>Substitution matrix is empty</source>
        <translation>İkame matrisi boş</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="759"/>
        <source>Needed amount of memory for this task is %1 MB, but it limited to %2 MB.</source>
        <translation>Bu görev için gereken bellek miktarı %1 MB&apos;dir, ancak %2 MB ile sınırlıdır.</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="762"/>
        <source>Smith Waterman2 SequenceWalker</source>
        <translation>Smith Waterman2 SequenceWalker</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="854"/>
        <source>%1 results found</source>
        <translation>%1 sonuç bulundu</translation>
    </message>
</context>
<context>
    <name>U2::QDSWActor</name>
    <message>
        <location filename="../src/SWQuery.cpp" line="92"/>
        <source>matches with &lt;u&gt;at least %1 score&lt;/u&gt;</source>
        <translation>&lt;u&gt;en az %1 puan&lt;/u&gt; ile eşleşir</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="92"/>
        <source>exact matches</source>
        <translation>tam eşleşmeler</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="97"/>
        <source>both strands</source>
        <translation>her iki dize</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="100"/>
        <source>direct strand</source>
        <translation>direkt dize</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="103"/>
        <source>complement strand</source>
        <translation>tamamlayıcı dize</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="109"/>
        <source>Finds pattern &lt;u&gt;%1&lt;/u&gt;.&lt;br&gt;Looks for &lt;u&gt;%2&lt;/u&gt; in &lt;u&gt;%3&lt;/u&gt;.</source>
        <translation>Desen bul &lt;u&gt;%1&lt;/u&gt;.&lt;br&gt;İçin ara &lt;u&gt;%2&lt;/u&gt; in &lt;u&gt;%3&lt;/u&gt;.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="137"/>
        <source>%1: percent of score out of bounds.</source>
        <translation>%1: sınırların dışındaki puanın yüzdesi.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="147"/>
        <source>%1: incorrect result filter.</source>
        <translation>%1: yanlış sonuç filtresi.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="152"/>
        <source>%1: pattern is empty.</source>
        <translation>%1: desen boş.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="158"/>
        <source>%1: can not find %2.</source>
        <translation>%1: %2 bulunamıyor.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="201"/>
        <source>SSearch</source>
        <translation>SAra</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="213"/>
        <source>smith_waterman_task</source>
        <translation>smith_waterman_görevi</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="243"/>
        <source>Smith-Waterman</source>
        <translation>Smith-Waterman</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="244"/>
        <source>Finds regions of similarity to the specified pattern in each input sequence (nucleotide or protein one). &lt;p&gt;Under the hood is the well-known Smith-Waterman algorithm for performing local sequence alignment.</source>
        <translation>Her giriş dizisinde (nükleotid veya protein bir) belirtilen modele benzerlik gösteren bölgeleri bulur. &lt;p&gt;Kapsam altında, yerel dizi hizalamasını gerçekleştirmek için iyi bilinen Smith-Waterman algoritması var.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="253"/>
        <source>Pattern</source>
        <translation>Desen</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="253"/>
        <source>A subsequence pattern to look for.</source>
        <translation>Aranacak bir alt sıra kalıbı.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="254"/>
        <source>Min score</source>
        <translation>Asgari puan</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="254"/>
        <source>The search stringency.</source>
        <translation>Arama sertliği.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="255"/>
        <source>Algorithm</source>
        <translation>Algoritma</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="255"/>
        <source>Algorithm version.</source>
        <translation>Algoritma sürümü.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="256"/>
        <source>Search in translation</source>
        <translation>Çeviride ara</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="256"/>
        <source>Translate a supplied nucleotide sequence to protein then search in the translated sequence.</source>
        <translation>Sağlanan bir nükleotid dizisini proteine çevirin ve ardından çevrilen dizide arayın.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="257"/>
        <source>Scoring matrix</source>
        <translation>Puanlama matrisi</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="257"/>
        <source>The scoring matrix.</source>
        <translation>Puanlama matrisi.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="258"/>
        <source>Filter results</source>
        <translation>Filtre sonuçları</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="258"/>
        <source>Result filtering strategy.</source>
        <translation>Sonuç filtreleme stratejisi.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="259"/>
        <source>Gap open score</source>
        <translation>Boşluk açma puanı</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="259"/>
        <source>Gap open score.</source>
        <translation>Boşluk açma puanı.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="260"/>
        <source>Gap ext score</source>
        <translation>Boşluk ek puanı</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="260"/>
        <source>Gap extension score.</source>
        <translation>Boşluk uzatma puanı.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="297"/>
        <source>Auto</source>
        <translation>Otomatik</translation>
    </message>
</context>
<context>
    <name>U2::SWAlgorithmADVContext</name>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="147"/>
        <source>Find pattern [Smith-Waterman]...</source>
        <translation>Desen bul [Smith-Waterman]...</translation>
    </message>
</context>
<context>
    <name>U2::SWAlgorithmPlugin</name>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="68"/>
        <source>Optimized Smith-Waterman </source>
        <translation>Optimize edilmiş Smith-Waterman </translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="68"/>
        <source>Various implementations of Smith-Waterman algorithm</source>
        <translation>Smith-Waterman algoritmasının çeşitli uygulamaları</translation>
    </message>
</context>
<context>
    <name>U2::SWAlgorithmTask</name>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="195"/>
        <source>Needed amount of memory for this task is %1 MB, but it limited to %2 MB.</source>
        <translation>Bu görev için gereken bellek miktarı %1 MB&apos;dir, ancak %2 MB ile sınırlıdır.</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="198"/>
        <source>Smith Waterman2 SequenceWalker</source>
        <translation>Smith Waterman2 SequenceWalker</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="413"/>
        <source>%1 results found</source>
        <translation>%1 sonuç bulundu</translation>
    </message>
</context>
</TS>
