<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>ClarkBuildValidator</name>
    <message>
        <source>Taxonomy classification data from NCBI data are not available.</source>
        <translation>NCBI verilerinden taksonomi sınıflandırma verileri mevcut değildir.</translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not full: file &apos;%1&apos; is missing.</source>
        <translation>NCBI&apos;deki sınıflandırma sınıflandırma verileri dolu değil: &apos;%1&apos; dosyası eksik.</translation>
    </message>
</context>
<context>
    <name>ClarkClassifyValidator</name>
    <message>
        <source>The database folder doesn&apos;t exist: %1.</source>
        <translation>Veritabanı klasörü mevcut değil: %1.</translation>
    </message>
    <message>
        <source>The mandatory database file doesn&apos;t exist: %1.</source>
        <translation>Zorunlu veritabanı dosyası mevcut değil: %1.</translation>
    </message>
    <message>
        <source>Reference database for these CLARK settings is not available. RefSeq data are required to build it.</source>
        <translation>Bu CLARK ayarları için referans veritabanı mevcut değildir. Oluşturmak için RefSeq verileri gereklidir.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>CLARK external tool support</source>
        <translation>CLARK harici araç desteği</translation>
    </message>
    <message>
        <source>The plugin supports CLARK: fast, accurate and versatile sequence classification system (http://clark.cs.ucr.edu)</source>
        <translation>Eklenti CLARK&apos;ı destekler: hızlı, doğru ve çok yönlü dizi sınıflandırma sistemi (http://clark.cs.ucr.edu)</translation>
    </message>
</context>
<context>
    <name>U2::ClarkSupport</name>
    <message>
        <source>CLARK (CLAssifier based on Reduced K-mers) is a tool for supervised sequence classification based on discriminative k-mers. UGENE provides the GUI for CLARK and CLARK-l variants of the CLARK framework for solving the problem of the assignment of metagenomic reads to known genomes.</source>
        <translation>CLARK (İndirgenmiş K-merlere dayalı CLAssifier), ayırt edici k-merlere dayalı denetimli sekans sınıflandırması için bir araçtır. UGENE, bilinen genomlara metagenomik okumaların atanması sorununu çözmek için CLARK çerçevesinin CLARK ve CLARK-l varyantları için ARAYÜZ sağlar.</translation>
    </message>
    <message>
        <source>One of the classifiers from the CLARK framework. This tool is created for powerful workstations and can require a significant amount of RAM.&lt;br&gt;&lt;br&gt;Note that a UGENE-customized version of the tool is required.</source>
        <translation>CLARK çerçevesindeki sınıflandırıcılardan biri. Bu araç, güçlü iş istasyonları için oluşturulmuştur ve önemli miktarda RAM gerektirebilir. &lt;br&gt;&lt;br&gt; Aracın UGENE&apos;ye göre özelleştirilmiş bir sürümünün gerekli olduğunu unutmayın.</translation>
    </message>
    <message>
        <source>Used to set up metagenomic database for CLARK.&lt;br&gt;&lt;br&gt;Note that a UGENE-customized version of the tool is required.</source>
        <translation>CLARK için metagenomik veritabanı kurmak için kullanılır. &lt;br&gt;&lt;br&gt; Aracın UGENE ile özelleştirilmiş bir sürümünün gerekli olduğunu unutmayın.</translation>
    </message>
    <message>
        <source>One of the classifiers from the CLARK framework. This tool is created for workstations with limited memory (i.e., “l” for light), it provides precise classification on small metagenomes.&lt;br&gt;&lt;br&gt;Note that a UGENE-customized version of the tool is required.</source>
        <translation>CLARK çerçevesindeki sınıflandırıcılardan biri. Bu araç, sınırlı belleğe (yani ışık için &quot;l&quot;) sahip iş istasyonları için oluşturulmuştur ve küçük metagenomlar üzerinde hassas sınıflandırma sağlar. &lt;br&gt;&lt;br&gt; Aracın UGENE&apos;ye göre özelleştirilmiş bir sürümünün gerekli olduğunu unutmayın.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClarkBuildPrompter</name>
    <message>
        <source>Use custom data to build %1 CLARK database.</source>
        <translation>%1 CLARK veritabanı oluşturmak için özel verileri kullanın.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClarkBuildTask</name>
    <message>
        <source>Build Clark database</source>
        <translation>Clark veritabanı oluştur</translation>
    </message>
    <message>
        <source>CLARK database URL is undefined</source>
        <translation>CLARK veritabanı URL&apos;si tanımsız</translation>
    </message>
    <message>
        <source>Taxdata URL is undefined</source>
        <translation>Vergi verileri URL&apos;si tanımsız</translation>
    </message>
    <message>
        <source>Genomic library set is empty</source>
        <translation>Genomik kitaplık kümesi boş</translation>
    </message>
    <message>
        <source>Failed to recognize the rank. Please provide a number between 0 and 5, according to the following:
0: species, 1: genus, 2: family, 3: order, 4:class, and 5: phylum.</source>
        <translation>Rütbe tanınamadı. Lütfen aşağıdakilere göre 0 ile 5 arasında bir sayı girin:
0: türler, 1: cins, 2: aile, 3: düzen, 4: sınıf ve 5: filum.</translation>
    </message>
    <message>
        <source>Failed to create folder for CLARK database: %1/%2</source>
        <translation>CLARK veritabanı için klasör oluşturulamadı: %1/%2</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClarkBuildWorker</name>
    <message>
        <source>Build CLARK Database</source>
        <translation>CLARK Veritabanı Oluşturun</translation>
    </message>
    <message>
        <source>Build a CLARK database from a set of reference sequences (&quot;targets&quot;).
NCBI taxonomy data are used to map the accession number found in each reference sequence to its taxonomy ID.</source>
        <translation>Bir dizi referans dizisinden (&quot;hedefler&quot;) bir CLARK veritabanı oluşturun.
NCBI taksonomi verileri, her bir referans dizisinde bulunan erişim numarasını taksonomi kimliği ile eşleştirmek için kullanılır.</translation>
    </message>
    <message>
        <source>Output CLARK database</source>
        <translation>CLARK veritabanı Çıktısı</translation>
    </message>
    <message>
        <source>URL to the folder with the CLARK database.</source>
        <translation>CLARK veritabanına sahip klasörün URL&apos;si.</translation>
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
        <source>Database</source>
        <translation>Veri Tabanı</translation>
    </message>
    <message>
        <source>A folder that should be used to store the database files.</source>
        <translation>Veritabanı dosyalarını saklamak için kullanılması gereken bir klasör.</translation>
    </message>
    <message>
        <source>Genomic library</source>
        <translation>Genomik kütüphane</translation>
    </message>
    <message>
        <source>Genomes that should be used to build the database (&quot;targets&quot;).&lt;br&gt;&lt;br&gt;The genomes should be specified in FASTA format. There should be one FASTA file per reference sequence. A sequence header must contain an accession number (i.e., &amp;gt;accession.number ... or &amp;gt;gi|number|ref|accession.number| ...).</source>
        <translation>Veritabanını oluşturmak için kullanılması gereken genomlar (&quot;hedefler&quot;). &lt;br&gt;&lt;br&gt; Genomlar FAŞTA formatında belirtilmelidir. Referans dizisi başına bir FAŞTA dosyası olmalıdır. Bir sıra başlığı bir erişim numarası (yani, &amp;gt;erişim.numarası ... veya &amp;gt;gi|sayı|ref|erişim.numarası| ...) içermelidir.</translation>
    </message>
    <message>
        <source>Taxonomy rank</source>
        <translation>Sınıflandırma sıralaması</translation>
    </message>
    <message>
        <source>Set the taxonomy rank for the database.&lt;br&gt;&lt;br&gt;CLARK classifies metagenomic samples by using only one taxonomy rank. So as a general rule, consider first the genus or species rank, then if a high proportion of reads cannot be classified, reset your targets definition at a higher taxonomy rank (e.g., family or phylum).</source>
        <translation>Veritabanı için taksonomi sıralamasını ayarlayın. &lt;br&gt;&lt;br&gt; CLARK, metagenomik örnekleri yalnızca bir taksonomi sıralaması kullanarak sınıflandırır. Bu nedenle, genel bir kural olarak, önce cins veya tür sıralamasını göz önünde bulundurun, ardından yüksek oranda okuma sınıflandırılamazsa, hedef tanımınızı daha yüksek bir taksonomi kademesinde (örneğin, aile veya filum) sıfırlayın.</translation>
    </message>
    <message>
        <source>Built Clark database</source>
        <translation>Clark veritabanı oluşturuldu</translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation>NCBI&apos;dan taksonomi sınıflandırma verileri mevcut değildir.</translation>
    </message>
    <message>
        <source>Species</source>
        <translation>Türler</translation>
    </message>
    <message>
        <source>Genus</source>
        <translation>Cins</translation>
    </message>
    <message>
        <source>Family</source>
        <translation>Aile</translation>
    </message>
    <message>
        <source>Order</source>
        <translation>Sipariş</translation>
    </message>
    <message>
        <source>Class</source>
        <translation>Sınıf</translation>
    </message>
    <message>
        <source>Phylum</source>
        <translation>Phylum</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClarkClassifyPrompter</name>
    <message>
        <source>Classify sequences from &lt;u&gt;%1&lt;/u&gt; with CLARK, use %2 database.</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; dizilerini CLARK ile sınıflandırın, %2 veritabanını kullanın.</translation>
    </message>
    <message>
        <source>Classify paired-end reads from &lt;u&gt;%1&lt;/u&gt; with CLARK, use %2 database.</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; &apos;den gelen çift uçlu okumaları CLARK ile sınıflandırın, %2 veritabanını kullanın.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClarkClassifyTask</name>
    <message>
        <source>Classify reads with Clark</source>
        <translation>Okumaları Clark ile sınıflandırın</translation>
    </message>
    <message>
        <source>Unsupported CLARK variant. Only default and light variants are supported.</source>
        <translation>Desteklenmeyen CLARK çeşidi. Yalnızca varsayılan ve hafif varyantlar desteklenir.</translation>
    </message>
    <message>
        <source>Cannot open classification report: %1</source>
        <translation>Sınıflandırma raporu açılamıyor: %1</translation>
    </message>
    <message>
        <source>Failed to recognize CLARK report format: %1</source>
        <translation>CLARK rapor biçimi tanınamadı: %1</translation>
    </message>
    <message>
        <source>Broken CLARK report: %1</source>
        <translation>Bozuk CLARK raporu: %1</translation>
    </message>
    <message>
        <source>Duplicate sequence name &apos;%1&apos; have been detected in the classification output.</source>
        <translation>Sınıflandırma çıktısında yinelenen sıra adı &apos;%1&apos; tespit edildi.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ClarkClassifyWorker</name>
    <message>
        <source>Classify Sequences with CLARK</source>
        <translation>Dizileri CLARK ile Sınıflandırma</translation>
    </message>
    <message>
        <source>CLARK (CLAssifier based on Reduced K-mers) is a tool for supervised sequence classification based on discriminative k-mers. UGENE provides the GUI for CLARK and CLARK-l variants of the CLARK framework for solving the problem of the assignment of metagenomic reads to known genomes.</source>
        <translation>CLARK (İndirgenmiş K-merlere dayalı CLAssifier), ayırt edici k-merlere dayalı denetimli sekans sınıflandırması için bir araçtır. UGENE, bilinen genomlara metagenomik okumaların atanması sorununu çözmek için CLARK çerçevesinin CLARK ve CLARK-l varyantları için ARAYÜZ sağlar.</translation>
    </message>
    <message>
        <source>Input sequences</source>
        <translation>Giriş dizileri</translation>
    </message>
    <message>
        <source>URL(s) to FASTQ or FASTA file(s) should be provided.

In case of SE reads or contigs use the &quot;Input URL 1&quot; slot only.

In case of PE reads input &quot;left&quot; reads to &quot;Input URL 1&quot;, &quot;right&quot; reads to &quot;Input URL 2&quot;.

See also the &quot;Input data&quot; parameter of the element.</source>
        <translation>FASTQ veya FASTA dosyalarının URL&apos;leri sağlanmalıdır.

SE okumaları veya devamı durumunda yalnızca &quot;Giriş URL 1&quot; yuvasını kullanın.

PE&apos;nin &quot;sol&quot; girişi okuması durumunda &quot;Giriş URL&apos;si 1&quot;, &quot;sağ&quot; ise &quot;Giriş URL 2&quot; yi okur.

Öğenin &quot;Giriş verileri&quot; parametresine de bakın.</translation>
    </message>
    <message>
        <source>CLARK Classification</source>
        <translation>CLARK Sınıflandırması</translation>
    </message>
    <message>
        <source>A map of sequence names with the associated taxonomy IDs, classified by CLARK.</source>
        <translation>CLARK tarafından sınıflandırılan, ilişkili taksonomi kimlikleriyle birlikte sıra adlarının bir haritası.</translation>
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
        <source>Classification tool</source>
        <translation>Sınıflandırma aracı</translation>
    </message>
    <message>
        <source>Use CLARK-l on workstations with limited memory (i.e., &quot;l&quot; for light), this software tool provides precise classification on small metagenomes. It works with a sparse or &apos;&apos;light&apos;&apos; database (up to 4 GB of RAM) while still performing ultra accurate and fast results.&lt;br&gt;&lt;br&gt;Use CLARK on powerful workstations, it requires a significant amount of RAM to run with large database (e.g. all bacterial genomes from NCBI/RefSeq).</source>
        <translation>Sınırlı belleğe sahip iş istasyonlarında (yani, ışık için &quot;l&quot;) CLARK-l kullanın, bu yazılım aracı küçük metagenomlar üzerinde hassas sınıflandırma sağlar. Ultra hassas ve hızlı sonuçlar vermeye devam ederken seyrek veya &quot;hafif&quot; bir veritabanıyla (4 GB&apos;a kadar RAM) çalışır.&lt;br&gt;&lt;br&gt;Güçlü iş istasyonlarında CLARK kullanın, çalışmak için önemli miktarda RAM gerektirir geniş bir veri tabanı ile (örneğin NCBI / RefSeq&apos;ten tüm bakteriyel genomlar).</translation>
    </message>
    <message>
        <source>Database</source>
        <translation>Veri Tabanı</translation>
    </message>
    <message>
        <source>A path to the folder with the CLARK database files (-D).&lt;br&gt;&lt;br&gt;It is assumed that &quot;targets.txt&quot; file is located in this folder (the file is passed to the &quot;classify_metagenome.sh&quot; script from the CLARK package via parameter -T).</source>
        <translation>CLARK veritabanı dosyalarının (-D) bulunduğu klasörün yolu.&lt;br&gt;&lt;br&gt;Bu klasörde &quot;hedefler.txt&quot; dosyasının bulunduğu varsayılır (dosya, &quot;classify_metagenome.sh&quot; komut dosyasından, CLARK paketi -T) parametresi aracılığıyla.</translation>
    </message>
    <message>
        <source>Output file</source>
        <translation>Çıktı dosyası</translation>
    </message>
    <message>
        <source>Specify the output file name.</source>
        <translation>Çıktı dosyası adını belirtin.</translation>
    </message>
    <message>
        <source>K-mer length</source>
        <translation>K-mer uzunluğu</translation>
    </message>
    <message>
        <source>Set the k-mer length (-k).&lt;br&gt;&lt;br&gt;This value is critical for the classification accuracy and speed.&lt;br&gt;&lt;br&gt;For high sensitivity, it is recommended to set this value to 20 or 21 (along with the &quot;Full&quot; mode).&lt;br&gt;&lt;br&gt;However, if the precision and the speed are the main concern, use any value between 26 and 32.&lt;br&gt;&lt;br&gt;Note that the higher the value, the higher is the RAM usage. So, as a good tradeoff between speed, precision, and RAM usage, it is recommended to set this value to 31 (along with the &quot;Default&quot; or &quot;Express&quot; mode).</source>
        <translation>K-mer uzunluğunu (-k) ayarlayın.&lt;br&gt;&lt;br&gt;Bu değer, sınıflandırma doğruluğu ve hızı için önemlidir.&lt;br&gt;&lt;br&gt;Yüksek hassasiyet için, bu değerin 20 veya 21 olarak ayarlanması önerilir ( &quot;Tam&quot; mod ile birlikte).&lt;br&gt;&lt;br&gt;Ancak, asıl sorun hassasiyet ve hızsa, 26 ile 32 arasındaki herhangi bir değeri kullanın. &lt;br&gt;&lt;br&gt;Değer ne kadar yüksekse, o kadar RAM kullanımı daha yüksektir. Bu nedenle, hız, hassasiyet ve RAM kullanımı arasında iyi bir değiş tokuş olarak, bu değerin 31&apos;e ayarlanması önerilir (&quot;Varsayılan&quot; veya &quot;Ekspres&quot; mod ile birlikte).</translation>
    </message>
    <message>
        <source>Minimum k-mer frequency</source>
        <translation>Minimum k-mer frekansı</translation>
    </message>
    <message>
        <source>Minimum of k-mer frequency/occurrence for the discriminative k-mers (-t).&lt;br&gt;&lt;br&gt;For example, for 1 (or, 2), the program will discard any discriminative k-mer that appear only once (or, less than twice).</source>
        <translation>Ayrımcı k-merler (-t) için minimum k-mer frekansı / oluşumu. &lt;br&gt;&lt;br&gt; Örneğin, 1 (veya, 2) için, program yalnızca bir kez görünen herhangi bir ayırt edici k-mer&apos;i atar ( veya ikiden az).</translation>
    </message>
    <message>
        <source>Mode</source>
        <translation>Mod</translation>
    </message>
    <message>
        <source>Set the mode of the execution (-m):&lt;ul&gt;&lt;li&gt;&quot;Full&quot; to get detailed results, confidence scores and other statistics.&lt;li&gt;&quot;Default&quot; to get results summary and perform best trade-off between classification speed, accuracy and RAM usage.&lt;li&gt;&quot;Express&quot; to get results summary with the highest speed possible.&lt;/ul&gt;</source>
        <translation>Yürütme modunu (-m) ayarlayın: &lt;ul&gt;&lt;li&gt; Ayrıntılı sonuçlar, güven puanları ve diğer istatistikleri almak için &quot;Tam&quot;. &lt;li&gt; Sonuçların özetini almak ve sınıflandırma hızı arasında en iyi dengeyi sağlamak için &quot;Varsayılan&quot; , doğruluk ve RAM kullanımı.&lt;li&gt; Mümkün olan en yüksek hızda sonuç özetini almak için &quot;İfade edin&quot;.&lt;/ul&gt;</translation>
    </message>
    <message>
        <source>Sampling factor value</source>
        <translation>Örnekleme faktörü değeri</translation>
    </message>
    <message>
        <source>Sample factor value (-s).&lt;br&gt;&lt;br&gt;To load in memory half the discriminative k-mers set this value to 2. To load a third of these k-mers set it to 3.&lt;br&gt;&lt;br&gt;The higher the factor is, the lower the RAM usage is and the higher the classification speed/precision is. However, the sensitivity can be quickly degraded, especially for values higher than 3.</source>
        <translation>Örnek faktör değeri (-s). &lt;br&gt;&lt;br&gt; Ayrımcı k-mer&apos;lerin yarısını belleğe yüklemek için bu değeri 2&apos;ye ayarlayın. Bu k-mer&apos;lerin üçte birini yüklemek için 3&apos;e ayarlayın. &lt;br&gt;&lt;br&gt; Faktör ne kadar yüksekse, RAM kullanımı o kadar düşük ve sınıflandırma hızı / kesinliği o kadar yüksek olur. Bununla birlikte, hassasiyet, özellikle 3&apos;ten yüksek değerler için hızla azalabilir.</translation>
    </message>
    <message>
        <source>Gap</source>
        <translation>Boşluk</translation>
    </message>
    <message>
        <source>Extended output</source>
        <translation>Genişletilmiş çıktı</translation>
    </message>
    <message>
        <source>Request an extended output for the result file (--extended).</source>
        <translation>Sonuç dosyası (--extended) için genişletilmiş bir çıktı isteyin.</translation>
    </message>
    <message>
        <source>Load database into memory</source>
        <translation>Veritabanını belleğe yükle</translation>
    </message>
    <message>
        <source>Request the loading of database file by memory mapped-file (--ldm).&lt;br&gt;&lt;br&gt;This option accelerates the loading time but it will require an additional amount of RAM significant. This option also allows one to load the database in multithreaded-task (see also the &quot;Number of threads&quot; parameter).</source>
        <translation>Veritabanı dosyasının bellek eşlemeli dosya (--ldm) tarafından yüklenmesini isteyin. &lt;br&gt;&lt;br&gt; Bu seçenek, yükleme süresini hızlandırır, ancak önemli miktarda ek RAM gerektirir. Bu seçenek aynı zamanda bir kişinin çok iş parçacıklı görevde veritabanını yüklemesine izin verir (ayrıca bkz. &quot;İş parçacığı sayısı&quot; parametresi).</translation>
    </message>
    <message>
        <source>Number of threads</source>
        <translation>Konu sayısı</translation>
    </message>
    <message>
        <source>Use multiple threads for the classification and, with the &quot;Load database into memory&quot; option enabled, for the loading of the database into RAM (-n).</source>
        <translation>Sınıflandırma için birden çok iş parçacığı kullanın ve &quot;Veritabanını belleğe yükle&quot; seçeneği etkinleştirilmiş olarak, veritabanının RAM&apos;e (-n) yüklenmesi için.</translation>
    </message>
    <message>
        <source>Input data</source>
        <translation>Giriş verileri</translation>
    </message>
    <message>
        <source>To classify single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to &quot;SE reads or contigs&quot;.&lt;br&gt;&lt;br&gt;To classify paired-end (PE) reads, set the value to &quot;PE reads&quot;.&lt;br&gt;&lt;br&gt;One or two slots of the input port are used depending on the value of the parameter. Pass URL(s) to data to these slots.&lt;br&gt;&lt;br&gt;The input files should be in FASTA or FASTQ formats.</source>
        <translation>Okumalar de novo derlemesi tarafından alınan tek uçlu (SE) okumaları veya contigs&apos;i sınıflandırmak için bu parametreyi &quot;SE okumaları veya devamları&quot; olarak ayarlayın.&lt;br&gt;&lt;br&gt;Çift uçlu (PE) okumaları sınıflandırmak için değeri olarak ayarlayın &quot;PE okur&quot;.&lt;br&gt;&lt;br&gt;Giriş portunun bir veya iki yuvası, parametrenin değerine bağlı olarak kullanılır. Verilere URL&apos;leri bu yuvalara geçirin.&lt;br&gt;&lt;br&gt;Giriş dosyaları FAŞTA veya FASTQ biçimlerinde olmalıdır.</translation>
    </message>
    <message>
        <source>Unrecognized mode of execution, expected any of: 0 (full), 1 (default), 2 (express) or 3 (spectrum)</source>
        <translation>Tanınmayan yürütme modu, şunlardan herhangi biri bekleniyor: 0 (tam), 1 (varsayılan), 2 (ekspres) veya 3 (spektrum)</translation>
    </message>
    <message>
        <source>There were %1 input reads, %2 reads were classified.</source>
        <translation>%1 girdisi okundu, %2 okuma sınıflandırıldı.</translation>
    </message>
    <message>
        <source>&quot;Gap&quot; or number of non-overlapping k-mers to pass when creating the database (-п).&lt;br&gt;&lt;br&gt;Increase the value if it is required to reduce the RAM usage. Note that this will degrade the sensitivity.</source>
        <translation>Veritabanını oluştururken &quot;Aralık&quot; veya geçilecek çakışmayan k-mer sayısı (-п).&lt;br&gt;&lt;br&gt;RAM kullanımını azaltmak gerekirse değeri artırın. Bunun hassasiyeti düşüreceğini unutmayın.</translation>
    </message>
    <message>
        <source>SE reads or contigs</source>
        <translation>SE okur veya devam eder</translation>
    </message>
    <message>
        <source>PE reads</source>
        <translation>PE okur</translation>
    </message>
</context>
</TS>
