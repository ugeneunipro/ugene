<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>ClassificationFilterValidator</name>
    <message>
        <source>Invalid taxon ID: %1</source>
        <translation>Geçersiz takson kimliği: %1</translation>
    </message>
    <message>
        <source>Set &quot;%1&quot; to &quot;True&quot; or select a taxon in &quot;%2&quot;.</source>
        <translation>&quot;%1&quot; i &quot;Doğru&quot; olarak ayarlayın veya &quot;%2&quot; de bir takson seçin.</translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation>NCBI&apos;dan taksonomi sınıflandırma verileri mevcut değildir.</translation>
    </message>
</context>
<context>
    <name>ClassificationReportPrompter</name>
    <message>
        <source>Generate a detailed classification report.</source>
        <translation>Ayrıntılı bir sınıflandırma raporu oluşturun.</translation>
    </message>
    <message>
        <source>Auto</source>
        <translation>Otomatik</translation>
    </message>
</context>
<context>
    <name>ClassificationReportValidator</name>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation>NCBI&apos;dan taksonomi sınıflandırma verileri mevcut değildir.</translation>
    </message>
</context>
<context>
    <name>ClassificationReportWorkerFactory</name>
    <message>
        <source>Number of reads</source>
        <translation>Okuma sayısı</translation>
    </message>
    <message>
        <source>Tax ID</source>
        <translation>Vergi numarası</translation>
    </message>
</context>
<context>
    <name>GenomicLibraryDialog</name>
    <message>
        <source>Select Genomes for Kraken Database</source>
        <translation>Kraken Veritabanı için Genomları Seçin</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Select genomes...</source>
        <translation>Genomları seçin...</translation>
    </message>
    <message>
        <source>Custom genomes</source>
        <translation>Özel genomlar</translation>
    </message>
</context>
<context>
    <name>TaxonSelectionDialog</name>
    <message>
        <source>Select Taxa</source>
        <translation>Taksa Seçin</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationFilterPrompter</name>
    <message>
        <source>Put input sequences that belong to the specified taxonomic group(s) to separate file(s).</source>
        <translation>Dosya (lar) ı ayırmak için belirtilen taksonomik gruba / gruplara ait girdi dizilerini koyun.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationFilterTask</name>
    <message>
        <source>Filter classified reads</source>
        <translation>Sınıflandırılmış okumaları filtrele</translation>
    </message>
    <message>
        <source>Missing pair read for &apos;%1&apos;, input files: %2 and %3.</source>
        <translation>Eksik çift &apos;%1&apos; için okundu, giriş dosyaları: %2 ve %3.</translation>
    </message>
    <message>
        <source>Format %1 is not supported by this task.</source>
        <translation>%1 formatı bu görev tarafından desteklenmiyor.</translation>
    </message>
    <message>
        <source>Warning: classification result for the ‘%1’ (from &apos;%2&apos;) hasn’t been found.</source>
        <translation>Uyarı: &quot;%1&quot; (&quot;%2&quot; den) için sınıflandırma sonucu bulunamadı.</translation>
    </message>
    <message>
        <source>Failed writing sequence to ‘%1’.</source>
        <translation>Sıra &quot;%1&quot; e yazılamadı.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationFilterWorker</name>
    <message>
        <source>Filter by Classification</source>
        <translation>Sınıflandırmaya Göre Filtrele</translation>
    </message>
    <message>
        <source>The filter takes files with NGS reads or contigs, classified by one of the tools: Kraken, CLARK, DIAMOND, WEVOTE. For each input file it outputs a file with unspecific sequences (i.e. sequences not classified by the tools, taxID = 0) and/or one or several files with sequences that belong to specific taxonomic group(s).</source>
        <translation>Filtre, şu araçlardan biri tarafından sınıflandırılan NGS okumaları veya contigs içeren dosyaları alır: Kraken, CLARK, DIAMOND, WEVOTE. Her girdi dosyası için, belirli olmayan dizilere sahip bir dosya (yani araçlar tarafından sınıflandırılmamış diziler, taxID = 0) ve / veya belirli taksonomik grup (lar) a ait dizilere sahip bir veya birkaç dosya çıkarır.</translation>
    </message>
    <message>
        <source>Input sequences and tax IDs</source>
        <translation>Giriş dizileri ve vergi numaraları</translation>
    </message>
    <message>
        <source>The following input should be provided: &lt;ul&gt;&lt;li&gt;URL(s) to FASTQ or FASTA file(s).&lt;li&gt;Corresponding taxonomy classification of sequences in the files.&lt;/ul&gt;To process single-end reads or contigs, pass the URL(s) to  the &quot;Input URL 1&quot; slot.&lt;br&gt;&lt;br&gt;To process paired-end reads, pass the URL(s) to files with the &quot;left&quot; and &quot;right&quot; reads to the &quot;Input URL 1&quot; and &quot;Input URL 2&quot; slots correspondingly.&lt;br&gt;&lt;br&gt;The taxonomy classification data are received by one of the classification tools (Kraken, CLARK, or DIAMOND) and should correspond to the input files.</source>
        <translation>Aşağıdaki girdi sağlanmalıdır: &lt;ul&gt;&lt;li&gt; FASTQ veya FASTA dosyalarının URL&apos;leri. &lt;li&gt;Dosyalardaki dizilerin karşılık gelen sınıflandırma sınıflandırması.&lt;/ul&gt;Tek uçlu okumaları veya devamları işlemek için , URL&apos;leri  &quot;Giriş URL&apos;si 1&quot; yuvasına geçirin.&lt;br&gt;&lt;br&gt;Eşleştirilmiş uçlu okumaları işlemek için, URL&apos;leri &quot;sol&quot; ve &quot;sağ&quot; ifadelerine sahip dosyalara iletin. Karşılık gelen URL 1 &quot;ve&quot; Giriş URL 2 &quot;yuvalarını girin.&lt;br&gt;&lt;br&gt;Taksonomi sınıflandırma verileri, sınıflandırma araçlarından biri (Kraken, CLARK veya DIAMOND) tarafından alınır ve giriş dosyalarına karşılık gelmelidir.</translation>
    </message>
    <message>
        <source>Output File(s)</source>
        <translation>Çıktı Dosyaları</translation>
    </message>
    <message>
        <source>The port outputs URLs to files with NGS reads, classified by taxon IDs: one file per each specified taxon ID per each input file (or pair of files in case of PE reads).

Either one (for SE reads or contigs) or two (for PE reads) output slots are used depending on the input data.

See also the &quot;Input data&quot; parameter of the element.</source>
        <translation>Bağlantı noktası, takson kimliklerine göre sınıflandırılan NGS okumaları olan dosyalara URL&apos;ler verir: her bir girdi dosyası için belirtilen her takson kimliği başına bir dosya (veya PE okumaları durumunda dosya çifti).

Giriş verilerine bağlı olarak bir (SE okumaları veya devamı için) veya iki (PE okumaları için) çıkış yuvası kullanılır.

Öğenin &quot;Giriş verileri&quot; parametresine de bakın.</translation>
    </message>
    <message>
        <source>Input URL 1</source>
        <translation>Giriş URL&apos;si 1</translation>
    </message>
    <message>
        <source>Input URL 1.</source>
        <translation>Giriş URL&apos;si 1.</translation>
    </message>
    <message>
        <source>Input URL 2</source>
        <translation>Giriş URL&apos;si 2</translation>
    </message>
    <message>
        <source>Input URL 2.</source>
        <translation>Giriş URL&apos;si 2.</translation>
    </message>
    <message>
        <source>Output URL 1</source>
        <translation>Çıktı URL&apos;si 1</translation>
    </message>
    <message>
        <source>Output URL 1.</source>
        <translation>Çıktı URL&apos;si 1.</translation>
    </message>
    <message>
        <source>Output URL 2</source>
        <translation>Çıktı URL&apos;si 2</translation>
    </message>
    <message>
        <source>Output URL 2.</source>
        <translation>Çıktı URL&apos;si 2.</translation>
    </message>
    <message>
        <source>Save unspecific sequences</source>
        <translation>Belirsiz dizileri kaydedin</translation>
    </message>
    <message>
        <source>Select &quot;True&quot; to put all unspecific input sequences (i. e. sequences with tax ID = 0) into a separate file.&lt;br&gt;Select &quot;False&quot; to skip unspecific sequences. At least one specific taxon should be selected in the &quot;Save sequences with taxID&quot; parameter in this case.</source>
        <translation>Tüm spesifik olmayan girdi dizilerini (yani, vergi numarası = 0 olan dizileri) ayrı bir dosyaya koymak için &quot;Doğru&quot; seçeneğini seçin.&lt;br&gt;Belirsiz dizileri atlamak için &quot;Yanlış&quot; ı seçin. Bu durumda &quot;TaxID ile sekansları kaydet&quot; parametresinde en az bir spesifik takson seçilmelidir.</translation>
    </message>
    <message>
        <source>Input data</source>
        <translation>Giriş verileri</translation>
    </message>
    <message>
        <source>To filter single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to &quot;SE reads or contigs&quot;. Use the &quot;Input URL 1&quot; slot of the input port.&lt;br&gt;&lt;br&gt;To filter paired-end (PE) reads, set the value to &quot;PE reads&quot;. Use the &quot;&quot;Input URL 1&quot; and &quot;Input URL 2&quot; slots of the input port to input the NGS reads data.&lt;br&gt;&lt;br&gt;Also, input the classification data, received from Kraken, CLARK, or DIAMOND, to the &quot;Taxonomy classification data&quot; input slot.&lt;br&gt;&lt;br&gt;Either one or two slots of the output port are used depending on the input data.</source>
        <translation>Okumalar de novo düzeneği tarafından alınan tek uçlu (SE) okumaları veya devamları filtrelemek için bu parametreyi &quot;SE okumaları veya devamları&quot; olarak ayarlayın. Giriş bağlantı noktasının &quot;Giriş URL 1&quot; yuvasını kullanın. &lt;br&gt;&lt;br&gt; Eşleştirilmiş uç (PE) okumaları filtrelemek için değeri &quot;PE okumaları&quot; olarak ayarlayın. NGS okuma verilerini girmek için giriş portunun &quot;Giriş URL 1&quot; ve &quot;Giriş URL 2&quot; yuvalarını kullanın. &lt;br&gt;&lt;br&gt; Ayrıca Kraken, CLARK veya DIAMOND&apos;dan alınan sınıflandırma verilerini de &quot;Taksonomi sınıflandırma verileri&quot; giriş yuvası. &lt;br&gt;&lt;br&gt; Giriş verilerine bağlı olarak çıkış bağlantı noktasının bir veya iki yuvası kullanılır.</translation>
    </message>
    <message>
        <source>Save sequences with taxID</source>
        <translation>Sıraları taxID ile kaydedin</translation>
    </message>
    <message>
        <source>Select a taxID to put all sequences that belong to this taxonomic group (i. e. the specified taxID and all children in the taxonomy tree) into a separate file.</source>
        <translation>Bu taksonomik gruba ait olan tüm dizileri (yani, taksonomi ağacındaki tüm alt öğeleri) ayrı bir dosyaya koymak için bir taxID seçin.</translation>
    </message>
    <message>
        <source>Invalid taxon ID: %1</source>
        <translation>Geçersiz takson kimliği: %1</translation>
    </message>
    <message>
        <source>Set &quot;%1&quot; to &quot;True&quot; or select a taxon in &quot;%2&quot;.</source>
        <translation>&quot;%1&quot; i &quot;Doğru&quot; olarak ayarlayın veya &quot;%2&quot; de bir takson seçin.</translation>
    </message>
    <message>
        <source>No paired read provided</source>
        <translation>Eşleştirilmiş okuma sağlanmadı</translation>
    </message>
    <message>
        <source>Some input sequences have been skipped, as there was no classification data for them. See log for details.</source>
        <translation>Onlar için herhangi bir sınıflandırma verisi olmadığından bazı girdi dizileri atlanmıştır. Ayrıntılar için günlüğe bakın.</translation>
    </message>
    <message>
        <source>SE reads or contigs</source>
        <translation>SE okur veya devam eder</translation>
    </message>
    <message>
        <source>PE reads</source>
        <translation>PE okur</translation>
    </message>
    <message>
        <source>There are no sequences that belong to taxon ‘%1 (ID: %2)’ in the input ‘%3’ and ‘%4’ files.</source>
        <translation>Giriş &quot;%3&quot; ve &quot;%4&quot; dosyalarında &quot;%1 (Kimlik: %2)&quot; taksonuna ait herhangi bir dizi yok.</translation>
    </message>
    <message>
        <source>There are no sequences that belong to taxon ‘%1 (ID: %2)’ in the input ‘%3’ file.</source>
        <translation>Giriş &quot;%3&quot; dosyasında &quot;%1 (Kimlik: %2)&quot; taksonuna ait dizi yok.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationReportTask</name>
    <message>
        <source>Compose classification report</source>
        <translation>Sınıflandırma raporu oluşturun</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClassificationReportWorker</name>
    <message>
        <source>Classification Report</source>
        <translation>Sınıflandırma Raporu</translation>
    </message>
    <message>
        <source>Based on the input taxonomy classification data the element generates a detailed report and saves it in a tab-delimited text format.</source>
        <translation>Girdi sınıflandırması sınıflandırma verilerine dayalı olarak, öğe ayrıntılı bir rapor oluşturur ve bu raporu sekmeyle ayrılmış metin biçiminde kaydeder.</translation>
    </message>
    <message>
        <source>Input taxonomy data</source>
        <translation>Giriş taksonomisi verileri</translation>
    </message>
    <message>
        <source>Input taxonomy data from one of the classification elements (Kraken, CLARK, etc.).</source>
        <translation>Sınıflandırma unsurlarından (Kraken, CLARK, vb.) Birinden taksonomi verilerini girin.</translation>
    </message>
    <message>
        <source>Output file</source>
        <translation>Çıktı dosyası</translation>
    </message>
    <message>
        <source>Specify the output text file name.</source>
        <translation>Çıktı metin dosyası adını belirtin.</translation>
    </message>
    <message>
        <source>All taxa</source>
        <translation>Tüm taksonlar</translation>
    </message>
    <message>
        <source>By default, taxa with no sequences (reads or scaffolds) assigned are not included into the output. This option specifies to include all taxa.                                           &lt;br&gt;&lt;br&gt;This may be useful when output from several samples is compared.Set &quot;Sort by&quot; to &quot;Tax ID&quot; in this case.</source>
        <translation>Varsayılan olarak, atanmış sekans (okuma veya yapı iskelesi) olmayan taksonlar çıktıya dahil edilmez. Bu seçenek tüm taksonların dahil edileceğini belirtir.                                            &lt;br&gt;&lt;br&gt;Bu, birkaç örnekten çıktı karşılaştırıldığında faydalı olabilir. Bu durumda &quot;Sıralama ölçütü&quot; nü &quot;Vergi Kimliği&quot; olarak ayarlayın.</translation>
    </message>
    <message>
        <source>Sort by</source>
        <translation>Sıralama</translation>
    </message>
    <message>
        <source>It is possible to sort rows in the output file in two ways:             &lt;ul&gt;&lt;li&gt;by the number of reads, covered by the clade rooted at the taxon(i.e. &quot;clade_num&quot; for this taxID)&lt;/li&gt;             &lt;li&gt;by taxIDs&lt;/li&gt;&lt;/ul&gt;             The second option may be useful when output from different samples is compared.</source>
        <translation>Çıktı dosyasındaki satırları iki şekilde sıralamak mümkündür:             &lt;ul&gt;&lt;li&gt; taksonda köklenen sınıfla kapsanan okuma sayısına göre (yani bu taxID için &quot;clade_num&quot;)&lt;/li&gt;             &lt;li&gt; taxID&apos;lere göre&lt;/li&gt;&lt;/ul&gt;             İkinci seçenek, farklı örneklerden elde edilen çıktılar karşılaştırıldığında faydalı olabilir.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::EnsembleClassificationPrompter</name>
    <message>
        <source>Ensemble classification data from other elements into %1</source>
        <translation>Sınıflandırma verilerini diğer öğelerden %1&apos;e toplayın</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::EnsembleClassificationTask</name>
    <message>
        <source>Taxonomy classification for &apos;%1&apos; is missing from %2 slot</source>
        <translation>&quot;%1&quot; için sınıflandırma sınıflandırması %2 yuvasında eksik</translation>
    </message>
    <message>
        <source>Ensemble different classifications</source>
        <translation>Farklı sınıflandırmaları bir araya getirin</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::EnsembleClassificationWorker</name>
    <message>
        <source>Ensemble Classification Data</source>
        <translation>Topluluk Sınıflandırma Verileri</translation>
    </message>
    <message>
        <source>The element ensembles data, produced by classification tools (Kraken, CLARK, DIAMOND), into a single file in CSV format. This file can be used as input for the WEVOTE classifier.</source>
        <translation>Öğe, sınıflandırma araçları (Kraken, CLARK, DIAMOND) tarafından üretilen verileri CSV formatında tek bir dosyada toplar. Bu dosya WEVOTE sınıflandırıcısı için girdi olarak kullanılabilir.</translation>
    </message>
    <message>
        <source>Input taxonomy data 1</source>
        <translation>Giriş taksonomi verileri 1</translation>
    </message>
    <message>
        <source>An input slot for taxonomy classification data.</source>
        <translation>Taksonomi sınıflandırma verileri için bir giriş yuvası.</translation>
    </message>
    <message>
        <source>Input taxonomy data 2</source>
        <translation>Giriş taksonomi verileri 2</translation>
    </message>
    <message>
        <source>Input taxonomy data 3</source>
        <translation>Giriş taksonomi verileri 3</translation>
    </message>
    <message>
        <source>Ensembled classification</source>
        <translation>Toplu sınıflandırma</translation>
    </message>
    <message>
        <source>URL to the CSV file with ensembled classification data.</source>
        <translation>Toplu sınıflandırma verileri içeren CSV dosyasının URL&apos;si.</translation>
    </message>
    <message>
        <source>Input tax data 1</source>
        <translation>Giriş vergisi verileri 1</translation>
    </message>
    <message>
        <source>Input tax data 1.</source>
        <translation>Giriş vergisi verileri 1.</translation>
    </message>
    <message>
        <source>Input tax data 2</source>
        <translation>Giriş vergisi verileri 2</translation>
    </message>
    <message>
        <source>Input tax data 2.</source>
        <translation>Giriş vergisi verileri 2.</translation>
    </message>
    <message>
        <source>Input tax data 3</source>
        <translation>Giriş vergisi verileri 3</translation>
    </message>
    <message>
        <source>Input tax data 3.</source>
        <translation>Giriş vergisi verileri 3.</translation>
    </message>
    <message>
        <source>Output URL</source>
        <translation>Çıktı URL&apos;si</translation>
    </message>
    <message>
        <source>Output URL.</source>
        <translation>Çıktı URL&apos;si.</translation>
    </message>
    <message>
        <source>Number of tools</source>
        <translation>Alet sayısı</translation>
    </message>
    <message>
        <source>Specify the number of classification tools. The corresponding data should be provided using the input ports.</source>
        <translation>Sınıflandırma araçlarının sayısını belirtin. Karşılık gelen veriler, giriş portları kullanılarak sağlanmalıdır.</translation>
    </message>
    <message>
        <source>Output file</source>
        <translation>Çıktı dosyası</translation>
    </message>
    <message>
        <source>Specify the output file. The classification data are stored in CSV format with the following columns:&lt;ol&gt;&lt;li&gt; a sequence name&lt;li&gt;taxID from the first tool&lt;li&gt;taxID from the second tool&lt;li&gt;optionally, taxID from the third tool&lt;/ol&gt;</source>
        <translation>Çıktı dosyasını belirtin. Sınıflandırma verileri, aşağıdaki sütunlarla CSV formatında saklanır: &lt;ol&gt;&lt;li&gt; sıra adı&lt;li&gt;birinci araçtan taxID&lt;li&gt;ikinci araçtan taxID&lt;li&gt;isteğe bağlı olarak, üçüncü araçtan taxID&lt;/ol&gt;</translation>
    </message>
    <message>
        <source>Different taxonomy data do not match. Some sequence names were skipped.</source>
        <translation>Farklı sınıflandırma verileri eşleşmiyor. Bazı dizi adları atlandı.</translation>
    </message>
    <message>
        <source>Not enough classified data in the ports &apos;%1&apos; and &apos;%2&apos;</source>
        <translation>&apos;%1&apos; ve &apos;%2&apos; bağlantı noktalarında yeterince sınıflandırılmış veri yok</translation>
    </message>
    <message>
        <source>Not enough classified data in the port &apos;%1&apos;</source>
        <translation>&apos;%1&apos; bağlantı noktasında yeterince sınıflandırılmış veri yok</translation>
    </message>
    <message>
        <source>Auto</source>
        <translation>Otomatik</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::GenomicLibraryDialog</name>
    <message>
        <source>Select</source>
        <translation>Seç</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Çıkış</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::TaxonSelectionDialog</name>
    <message>
        <source>Select</source>
        <translation>Seç</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Çıkış</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::TaxonomySupport</name>
    <message>
        <source>Taxonomy classification data</source>
        <translation>Taksonomi sınıflandırma verileri</translation>
    </message>
    <message>
        <source>Taxon name</source>
        <translation>Takson adı</translation>
    </message>
    <message>
        <source>Rank</source>
        <translation>Sıra</translation>
    </message>
    <message>
        <source>Taxon ID</source>
        <translation>Takson kimliği</translation>
    </message>
</context>
<context>
    <name>U2::NgsReadsClassificationPlugin</name>
    <message>
        <source>Loading NCBI taxonomy data</source>
        <translation>NCBI taksonomi verilerini yükleme</translation>
    </message>
    <message>
        <source>NCBI taxonomy classification data</source>
        <translation>NCBI taksonomi sınıflandırma verileri</translation>
    </message>
    <message>
        <source>CLARK viral database</source>
        <translation>CLARK viral veritabanı</translation>
    </message>
    <message>
        <source>Minikraken 4Gb database</source>
        <translation>Minikraken 4Gb veritabanı</translation>
    </message>
    <message>
        <source>DIAMOND database built from UniProt50</source>
        <translation>UniProt50&apos;den oluşturulan DIAMOND veritabanı</translation>
    </message>
    <message>
        <source>DIAMOND database built from UniProt90</source>
        <translation>UniProt90&apos;dan oluşturulmuş DIAMOND veritabanı</translation>
    </message>
    <message>
        <source>RefSeq release human data from NCBI</source>
        <translation>RefSeq, NCBI&apos;den insan verilerini yayınladı</translation>
    </message>
    <message>
        <source>RefSeq release viral data from NCBI</source>
        <translation>RefSeq, NCBI&apos;den viral verileri yayınladı</translation>
    </message>
    <message>
        <source>Found the %1 at %2</source>
        <translation>%1&apos;i %2 konumunda buldum</translation>
    </message>
    <message>
        <source>CLARK bacterial and viral database</source>
        <translation>CLARK bakteriyel ve viral veritabanı</translation>
    </message>
    <message>
        <source>RefSeq release bacterial data from NCBI</source>
        <translation>RefSeq, NCBI&apos;dan bakteri verilerini serbest bırakır</translation>
    </message>
    <message>
        <source>MetaPhlAn2 database</source>
        <translation>MetaPhlAn2 veritabanı</translation>
    </message>
</context>
</TS>
