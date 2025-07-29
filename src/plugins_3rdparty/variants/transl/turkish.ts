<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr" sourcelanguage="en">
<context>
    <name>U2::LocalWorkflow::CallVariantsPrompter</name>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="691"/>
        <source>unset</source>
        <translation>ayarlanmadı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="699"/>
        <source>For reference sequence from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; referans dizisi için,</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="702"/>
        <source>with assembly data provided by &lt;u&gt;%1&lt;/u&gt;</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; tarafından sağlanan montaj verileriyle</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="704"/>
        <source>%1 call variants %2.</source>
        <translation>%1 çağrı varyantları %2.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::CallVariantsTask</name>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="53"/>
        <source>Call variants for %1</source>
        <translation>%1 için varyantları arayın</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="66"/>
        <source>reference</source>
        <translation>referans</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="68"/>
        <source>assembly</source>
        <translation>montaj</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="76"/>
        <source>The %1 file does not exist: %2</source>
        <translation>%1 dosyası mevcut değil: %2</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="89"/>
        <source>No assembly files</source>
        <translation>Montaj dosyası yok</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="94"/>
        <source>No dbi storage</source>
        <translation>Dbi depolama yok</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="98"/>
        <source>No sequence URL</source>
        <translation>Sıra URL&apos;si yok</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="136"/>
        <source>No document loaded</source>
        <translation>Yüklü belge yok</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="140"/>
        <source>Incorrect variant track object in %1</source>
        <translation>%1 konumunda yanlış varyant izleme nesnesi</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::CallVariantsWorker</name>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="135"/>
        <source>Empty input slot: %1</source>
        <translation>Boş giriş yuvası: %1</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="155"/>
        <source>Input sequences</source>
        <translation>Giriş dizileri</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="156"/>
        <source>A nucleotide reference sequence.</source>
        <translation>Bir nükleotid referans dizisi.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="163"/>
        <source>Input assembly</source>
        <translation>Giriş montajı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="164"/>
        <source>Position sorted alignment file</source>
        <translation>Sıralanmış hizalama dosyasını konumlandırın</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="171"/>
        <source>Output variations</source>
        <translation>Çıktı varyasyonları</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="172"/>
        <source>Output tracks with SNPs and short INDELs</source>
        <translation>SNP&apos;ler ve kısa INDEL&apos;ler ile çıkış izleri</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="177"/>
        <source>Call Variants with SAMtools</source>
        <translation>SAMtools ile Varyantları Arayın</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="178"/>
        <source>Calls SNPs and INDELS with SAMtools mpileup and bcftools.</source>
        <translation>SAMtools mpileup ve bcftools ile SNP&apos;leri ve INDELS&apos;i çağırır.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="186"/>
        <source>Output variants file</source>
        <translation>Çıktı varyantları dosyası</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="187"/>
        <source>The url to the file with the extracted variations.</source>
        <translation>Ayıklanan varyasyonların bulunduğu dosyanın url&apos;si.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="190"/>
        <source>Use reference from</source>
        <translation>Referans kullanın</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="191"/>
        <source>&lt;p&gt;Specify &quot;File&quot; to set a single reference sequence for all input NGS assemblies. The reference should be set in the &quot;Reference&quot; parameter.&lt;/p&gt;&lt;p&gt;Specify &quot;Input port&quot; to be able to set different references for difference NGS assemblies. The references should be input via the &quot;Input sequences&quot; port (e.g. use datasets in the &quot;Read Sequence&quot; element).&lt;/p&gt;</source>
        <translation>&lt;p&gt;Tüm giriş NGS derlemeleri için tek bir referans dizisi ayarlamak için &quot;Dosya&quot; seçeneğini belirtin. Referans, &quot;Referans&quot; parametresinde ayarlanmalıdır. &lt;/p&gt;&lt;p&gt; Fark NGS montajları için farklı referanslar ayarlayabilmek için &quot;Giriş portu&quot; belirtin. Referanslar, &quot;Giriş dizileri&quot; bağlantı noktası aracılığıyla girilmelidir (ör. &quot;Sırayı Oku&quot; öğesindeki veri kümelerini kullanın).&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="197"/>
        <source>Reference</source>
        <translation>Referans</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="198"/>
        <source>&lt;p&gt;Specify a file with the reference sequence.&lt;/p&gt;&lt;p&gt;The sequence will be used as reference for all datasets with NGS assemblies.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Referans sıralı bir dosya belirtin. &lt;/p&gt;&lt;p&gt;Dizi, NGS derlemelerine sahip tüm veri kümeleri için referans olarak kullanılacaktır.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="202"/>
        <source>Illumina-1.3+ encoding</source>
        <translation>Illumina-1.3 + kodlaması</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="203"/>
        <source>Assume the quality is in the Illumina 1.3+ encoding (mpileup)(-6).</source>
        <translation>Kalitenin Illumina 1.3+ kodlamasında (mpileup) (- 6) olduğunu varsayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="206"/>
        <source>Count anomalous read pairs</source>
        <translation>Anormal okuma çiftlerini sayın</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="207"/>
        <source>Do not skip anomalous read pairs in variant calling(mpileup)(-A).</source>
        <translation>Varyant çağrısında (mpileup)(-A) anormal okuma çiftlerini atlamayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="210"/>
        <source>Disable BAQ computation</source>
        <translation>BAQ hesaplamasını devre dışı bırakın</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="211"/>
        <source>Disable probabilistic realignment for the computation of base alignment quality (BAQ). BAQ is the Phred-scaled probability of a read base being misaligned. Applying this option greatly helps to reduce false SNPs caused by misalignments. (mpileup)(-B).</source>
        <translation>Temel hizalama kalitesinin (BAQ) hesaplanması için olasılıksal yeniden hizalamayı devre dışı bırakın. BAQ, yanlış hizalanmış bir okuma tabanının Phred ölçekli olasılığıdır. Bu seçeneğin uygulanması, yanlış hizalamaların neden olduğu yanlış SNP&apos;lerin azaltılmasına büyük ölçüde yardımcı olur. (mpileup)(-B).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="216"/>
        <source>Mapping quality downgrading coefficient</source>
        <translation>Haritalama kalitesi düşürme katsayısı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="217"/>
        <source>Coefficient for downgrading mapping quality for reads containing excessive mismatches. Given a read with a phred-scaled mapping quality q of being generated from the mapped position, the new mapping quality is about sqrt((INT-q)/INT)*INT. A zero value disables this functionality; if enabled, the recommended value for BWA is 50 (mpileup)(-C).</source>
        <translation>Aşırı uyumsuzluk içeren okumalar için eşleme kalitesini düşürme katsayısı. Eşlenen konumdan üretilen, phred-scaled haritalama kalitesi q ile bir okuma verildiğinde, yeni eşleme kalitesi sqrt ((INT-q)/INT)*INT hakkındadır. Sıfır değeri bu işlevi devre dışı bırakır; etkinleştirilirse, BWA için önerilen değer 50&apos;dir (mpileup)(-C).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="222"/>
        <source>Max number of reads per input BAM</source>
        <translation>Giriş BAM başına maksimum okuma sayısı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="223"/>
        <source>At a position, read maximally the number of reads per input BAM (mpileup)(-d).</source>
        <translation>Bir konumda, giriş BAM başına okuma sayısını maksimum olarak okuyun (mpileup)(-d).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="226"/>
        <source>Extended BAQ computation</source>
        <translation>Genişletilmiş BAQ hesaplaması</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="227"/>
        <source>Extended BAQ computation. This option helps sensitivity especially for MNPs, but may hurt specificity a little bit (mpileup)(-E).</source>
        <translation>Genişletilmiş BAQ hesaplaması. Bu seçenek özellikle MNP&apos;ler için duyarlılığa yardımcı olur, ancak özgüllüğü biraz bozabilir (mpileup) (-E).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="231"/>
        <source>BED or position list file</source>
        <translation>BED veya pozisyon listesi dosyası</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="232"/>
        <source>BED or position list file containing a list of regions or sites where pileup or BCF should be generated (mpileup)(-l).</source>
        <translation>Yığın veya BCF&apos;nin oluşturulması gereken bölgelerin veya sitelerin bir listesini içeren BED veya konum listesi dosyası (mpileup) (-l).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="236"/>
        <source>Pileup region</source>
        <translation>Pileup bölgesi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="237"/>
        <source>Only generate pileup in region STR (mpileup)(-r).</source>
        <translation>Yalnızca STR (mpileup) (-r) bölgesinde yığın oluştur.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="240"/>
        <source>Minimum mapping quality</source>
        <translation>Minimum eşleme kalitesi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="241"/>
        <source>Minimum mapping quality for an alignment to be used (mpileup)(-q).</source>
        <translation>Kullanılacak bir hizalama için minimum eşleme kalitesi (mpileup) (-q).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="244"/>
        <source>Minimum base quality</source>
        <translation>Minimum temel kalite</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="245"/>
        <source>Minimum base quality for a base to be considered (mpileup)(-Q).</source>
        <translation>Dikkate alınacak bir baz için minimum temel kalite (mpileup) (-Q).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="248"/>
        <source>Gap extension error</source>
        <translation>Boşluk uzantısı hatası</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="249"/>
        <source>Phred-scaled gap extension sequencing error probability. Reducing INT leads to longer indels (mpileup)(-e).</source>
        <translation>Phred ölçekli boşluk uzantısı sıralama hata olasılığı. INT&apos;nin azaltılması daha uzun indellere (mpileup) (-e) yol açar.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="252"/>
        <source>Homopolymer errors coefficient</source>
        <translation>Homopolimer hata katsayısı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="253"/>
        <source>Coefficient for modeling homopolymer errors. Given an l-long homopolymer run, the sequencing error of an indel of size s is modeled as INT*s/l (mpileup)(-h).</source>
        <translation>Homopolimer hatalarını modelleme katsayısı. 1 uzunluğunda bir homopolimer çalışması verildiğinde, s boyutundaki bir indelin sıralama hatası INT*s / l (mpileup) (-h) olarak modellenir.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="256"/>
        <source>No INDELs</source>
        <translation>INDEL yok</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="257"/>
        <source>Do not perform INDEL calling (mpileup)(-I).</source>
        <translation>INDEL çağrısı (mpileup) (-I) yapmayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="260"/>
        <source>Max INDEL depth</source>
        <translation>Maksimum INDEL derinliği</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="261"/>
        <source>Skip INDEL calling if the average per-sample depth is above INT (mpileup)(-L).</source>
        <translation>Örnek başına ortalama derinlik INT (mpileup) (- L) üzerindeyse INDEL çağrısını atlayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="264"/>
        <source>Gap open error</source>
        <translation>Boşluk açma hatası</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="265"/>
        <source>Phred-scaled gap open sequencing error probability. Reducing INT leads to more indel calls (mpileup)(-o).</source>
        <translation>Phred ölçekli boşluk açık sıralama hatası olasılığı. INT&apos;nin azaltılması daha fazla indel aramaya (mpileup)(-o) yol açar.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="268"/>
        <source>List of platforms for indels</source>
        <translation>İndeller için platformların listesi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="269"/>
        <source>Comma dilimited list of platforms (determined by @RG-PL) from which indel candidates are obtained.It is recommended to collect indel candidates from sequencing technologies that have low indel error rate such as ILLUMINA (mpileup)(-P).</source>
        <translation>İçinden indel adaylarının elde edildiği (@RG-PL tarafından belirlenen) virgülle bölünmüş platformlar listesi ILLUMINA (mpileup)(-P) gibi düşük indel hata oranına sahip sıralama teknolojilerinden indel adaylarının toplanması önerilir.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="274"/>
        <source>Retain all possible alternate</source>
        <translation>Olası tüm alternatifleri koruyun</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="275"/>
        <source>Retain all possible alternate alleles at variant sites. By default, the view command discards unlikely alleles (bcf view)(-A).</source>
        <translation>Varyant sitelerinde tüm olası alternatif alelleri koruyun. Varsayılan olarak, view komutu olası olmayan allelleri (bcf view)(-A) atar.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="278"/>
        <source>Indicate PL</source>
        <translation>PL belirtin</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="279"/>
        <source>Indicate PL is generated by r921 or before (ordering is different) (bcf view)(-F).</source>
        <translation>PL&apos;nin r921 tarafından veya daha önce üretildiğini belirtin (sıralama farklıdır) (bcf görünümü)(-F).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="282"/>
        <source>No genotype information</source>
        <translation>Genotip bilgisi yok</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="283"/>
        <source>Suppress all individual genotype information (bcf view)(-G).</source>
        <translation>Tüm bireysel genotip bilgilerini bastırın (bcf görünümü)(-G).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="286"/>
        <source>A/C/G/T only</source>
        <translation>Yalnızca A/C/G/T</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="287"/>
        <source>Skip sites where the REF field is not A/C/G/T (bcf view)(-N).</source>
        <translation>REF alanının A/C/G/T (bcf görünümü)(-N) olmadığı siteleri atlayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="290"/>
        <source>List of sites</source>
        <translation>Sitelerin listesi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="291"/>
        <source>List of sites at which information are outputted (bcf view)(-l).</source>
        <translation>Bilgilerin çıktığı sitelerin listesi (bcf görünümü)(-l).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="294"/>
        <source>QCALL likelihood</source>
        <translation>QCALL olasılığı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="295"/>
        <source>Output the QCALL likelihood format (bcf view)(-Q).</source>
        <translation>QCALL olabilirlik biçimini (bcf görünümü)(-Q) çıktılayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="298"/>
        <source>List of samples</source>
        <translation>Örneklerin listesi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="299"/>
        <source>List of samples to use. The first column in the input gives the sample names and the second gives the ploidy, which can only be 1 or 2. When the 2nd column is absent, the sample ploidy is assumed to be 2. In the output, the ordering of samples will be identical to the one in FILE (bcf view)(-s).</source>
        <translation>Kullanılacak örneklerin listesi. Girişteki ilk sütun örnek adlarını verir ve ikincisi yalnızca 1 veya 2 olabilen ploidiyi verir. 2. sütun olmadığında, örnek ploidinin 2 olduğu varsayılır. Çıktıda, örneklerin sıralaması DOSYA (bcf görünümü)(-s) içindekiyle aynı olmalıdır.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="305"/>
        <source>Min samples fraction</source>
        <translation>Min numune fraksiyonu</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="306"/>
        <source>skip loci where the fraction of samples covered by reads is below FLOAT (bcf view)(-d).</source>
        <translation>okumaların kapsadığı örneklerin fraksiyonunun FLOAT (bcf görünümü)(-d) altında olduğu lokusu atlayın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="309"/>
        <source>Per-sample genotypes</source>
        <translation>Örnek başına genotipler</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="310"/>
        <source>Call per-sample genotypes at variant sites (bcf view)(-g).</source>
        <translation>Varyant sitelerinde örnek başına genotipleri çağırın (bcf görünümü)(-g).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="313"/>
        <source>INDEL-to-SNP Ratio</source>
        <translation>INDEL-SNP Oranı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="314"/>
        <source>Ratio of INDEL-to-SNP mutation rate (bcf view)(-i).</source>
        <translation>INDEL-SNP mutasyon oranının oranı (bcf görünümü)(-i).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="317"/>
        <source>Max P(ref|D)</source>
        <translation>Max P(ref|D)</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="318"/>
        <source>A site is considered to be a variant if P(ref|D)&lt;FLOAT (bcf view)(-p).</source>
        <translation>P (ref|D)&lt;FLOAT (bcf görünümü)(-p) ise bir site bir varyant olarak kabul edilir.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="321"/>
        <source>Prior allele frequency spectrum</source>
        <translation>Önceki alel frekans spektrumu</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="322"/>
        <source>If STR can be full, cond2, flat or the file consisting of error output from a previous variant calling run (bcf view)(-P).</source>
        <translation>STR dolu, koşul2, düz veya çalıştırmayı çağıran önceki bir varyantın (bcf görünümü)(-P) hata çıktısından oluşan dosya olabilir.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="325"/>
        <source>Mutation rate</source>
        <translation>Mutasyon oranı</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="326"/>
        <source>Scaled mutation rate for variant calling (bcf view)(-t).</source>
        <translation>Varyant çağırma için ölçeklendirilmiş mutasyon oranı (bcf görünümü)(-t).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="329"/>
        <source>Pair/trio calling</source>
        <translation>Çift/üçlü arama</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="330"/>
        <source>Enable pair/trio calling. For trio calling, option -s is usually needed to be applied to configure the trio members and their ordering. In the file supplied to the option -s, the first sample must be the child, the second the father and the third the mother. The valid values of STR are &apos;pair&apos;, &apos;trioauto&apos;, &apos;trioxd&apos; and &apos;trioxs&apos;, where &apos;pair&apos; calls differences between two input samples, and &apos;trioxd&apos; (&apos;trioxs&apos;)specifies that the input is from the X chromosome non-PAR regions and the child is a female (male) (bcf view)(-T).</source>
        <translation>Çift / üçlü aramayı etkinleştirin. Üçlü arama için, üçlü üyeleri ve sıralarını yapılandırmak için genellikle -s seçeneğinin uygulanması gerekir. -S seçeneğine verilen dosyada ilk örnek çocuk, ikincisi baba ve üçüncüsü anne olmalıdır. STR&apos;nin geçerli değerleri &quot;çift&quot;, &quot;trioauto&quot;, &quot;trioxd&quot; ve &quot;trioxs&quot; olup, burada &quot;çift&quot; iki giriş örneği arasındaki farklılıkları çağırır ve &quot;trioxd&quot; ( &apos;&apos; Triokslar &apos;&apos;), girdinin X kromozomu PAR olmayan bölgelerden olduğunu ve çocuğun bir dişi (erkek) (bcf görünümü) (- T) olduğunu belirtir.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="337"/>
        <source>N group-1 samples</source>
        <translation>N grup-1 örnekleri</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="338"/>
        <source>Number of group-1 samples. This option is used for dividing the samples into two groups for contrast SNP calling or association test. When this option is in use, the followingVCF INFO will be outputted: PC2, PCHI2 and QCHI2 (bcf view)(-1).</source>
        <translation>Grup-1 numunelerinin sayısı. Bu seçenek, örnekleri kontrast SNP çağrısı veya ilişkilendirme testi için iki gruba ayırmak için kullanılır. Bu seçenek kullanımda olduğunda, aşağıdaki VCF BİLGİSİ çıktısı alınacaktır: PC2, PCHI2 ve QCHI2 (bcf görünümü)(-1).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="343"/>
        <source>N permutations</source>
        <translation>N permütasyon</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="344"/>
        <source>Number of permutations for association test (effective only with -1) (bcf view)(-U).</source>
        <translation>İlişki testi için permütasyon sayısı (yalnızca -1 ile etkilidir) (bcf görünümü)(-U).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="347"/>
        <source>Max P(chi^2)</source>
        <translation>Max P(chi^2)</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="348"/>
        <source>Only perform permutations for P(chi^2)&lt;FLOAT (N permutations) (bcf view)(-X).</source>
        <translation>Yalnızca P (chi^2)&lt;FLOAT (N permütasyon) (bcf görünümü) (-X) için permütasyonlar gerçekleştirin.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="352"/>
        <source>Minimum RMS quality</source>
        <translation>Minimum RMS kalitesi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="353"/>
        <source>Minimum RMS mapping quality for SNPs (varFilter) (-Q).</source>
        <translation>SNP&apos;ler için minimum RMS eşleme kalitesi (varFilter) (-Q).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="355"/>
        <source>Minimum read depth</source>
        <translation>Minimum okuma derinliği</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="356"/>
        <source>Minimum read depth (varFilter) (-d).</source>
        <translation>Minimum okuma derinliği (varFilter) (-d).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="358"/>
        <source>Maximum read depth</source>
        <translation>Maksimum okuma derinliği</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="359"/>
        <source>Maximum read depth (varFilter) (-D).</source>
        <translation>Maksimum okuma derinliği (varFilter) (-D).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="361"/>
        <source>Alternate bases</source>
        <translation>Alternatif bazlar</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="362"/>
        <source>Minimum number of alternate bases (varFilter) (-a).</source>
        <translation>Minimum alternatif baz sayısı (varFilter) (-a).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="364"/>
        <source>Gap size</source>
        <translation>Boşluk boyutu</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="365"/>
        <source>SNP within INT bp around a gap to be filtered (varFilter) (-w).</source>
        <translation>INT bp içindeki SNP, filtrelenecek bir boşluk etrafında (varFilter) (-w).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="367"/>
        <source>Window size</source>
        <translation>Pencere boyutu</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="368"/>
        <source>Window size for filtering adjacent gaps (varFilter) (-W).</source>
        <translation>Bitişik boşlukları filtrelemek için pencere boyutu (varFilter) (-W).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="370"/>
        <source>Strand bias</source>
        <translation>Standart sapma</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="371"/>
        <source>Minimum P-value for strand bias (given PV4) (varFilter) (-1).</source>
        <translation>İplik sapması için minimum P değeri (PV4 verilir) (varFilter) (-1).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="373"/>
        <source>BaseQ bias</source>
        <translation>BaseQ sapması</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="374"/>
        <source>Minimum P-value for baseQ bias (varFilter) (-2).</source>
        <translation>BaseQ sapması için minimum P değeri (varFilter) (-2).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="376"/>
        <source>MapQ bias</source>
        <translation>MapQ sapması</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="377"/>
        <source>Minimum P-value for mapQ bias (varFilter) (-3).</source>
        <translation>MapQ sapması için minimum P değeri (varFilter) (-3).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="379"/>
        <source>End distance bias</source>
        <translation>Bitiş mesafesi sapması</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="380"/>
        <source>Minimum P-value for end distance bias (varFilter) (-4).</source>
        <translation>Bitiş mesafesi sapması için minimum P değeri (varFilter) (-4).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="382"/>
        <source>HWE</source>
        <translation>HWE</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="383"/>
        <source>Minimum P-value for HWE (plus F&lt;0) (varFilter) (-e).</source>
        <translation>HWE için minimum P değeri (artı F&lt;0) (varFilter) (-e).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="385"/>
        <source>Log filtered</source>
        <translation>Günlük filtrelendi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="386"/>
        <source>Print filtered variants into the log (varFilter) (-p).</source>
        <translation>Filtrelenmiş varyantları günlüğe (varFilter) (-p) yazdırın.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="658"/>
        <source>Input port</source>
        <translation>Giriş bağlantı noktası</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="659"/>
        <source>File</source>
        <translation>Dosya</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="843"/>
        <source>Assembly URL slot is empty. Please, specify the URL slot</source>
        <translation>Montaj URL yuvası boş. Lütfen URL yuvasını belirtin</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="868"/>
        <source>Ref sequence URL slot is empty. Please, specify the URL slot</source>
        <translation>Referans dizisi URL yuvası boş. Lütfen URL yuvasını belirtin</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="947"/>
        <source>Not enough references</source>
        <translation>Yeterli referans yok</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="952"/>
        <source>The dataset slot is not binded, only the first reference sequence against all assemblies was processed.</source>
        <translation>Veri kümesi yuvası bağlı değil, yalnızca tüm derlemelere karşı ilk referans dizisi işlendi.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="955"/>
        <source>Not enough assemblies</source>
        <translation>Yeterli montaj yok</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SamtoolsMpileupTask</name>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="167"/>
        <source>Samtool mpileup for %1 </source>
        <translation>%1 için Samtool mpileup </translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="174"/>
        <source>No reference sequence URL to do pileup</source>
        <translation>Hata yapacak referans dizisi URL&apos;si yok</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="179"/>
        <source>No assembly URL to do pileup</source>
        <translation>Yığınlama yapacak derleme URL&apos;si yok</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="185"/>
        <source>There is an assembly with an empty path</source>
        <translation>Boş yolu olan bir montaj var</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="194"/>
        <source>Can not create the folder: </source>
        <translation>Klasör oluşturulamıyor: </translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="249"/>
        <source>Can not run %1 tool</source>
        <translation>%1 aracı çalıştırılamıyor</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="255"/>
        <source>%1 tool exited with code %2</source>
        <translation>%1 araçtan %2 koduyla çıkıldı</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="257"/>
        <source>Tool %1 finished successfully</source>
        <translation>%1 aracı başarıyla tamamladı</translation>
    </message>
</context>
<context>
    <name>U2::SamtoolsPlugin</name>
    <message>
        <location filename="../src/SamtoolsPlugin.cpp" line="36"/>
        <source>Samtools plugin</source>
        <translation>Samtools eklentisi</translation>
    </message>
    <message>
        <location filename="../src/SamtoolsPlugin.cpp" line="36"/>
        <source>Samtools plugin for NGS data analysis</source>
        <translation>NGS veri analizi için Samtools eklentisi</translation>
    </message>
</context>
</TS>
