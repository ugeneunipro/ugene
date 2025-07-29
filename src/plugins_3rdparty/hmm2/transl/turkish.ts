<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
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
        <translation>HMM Yapısı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="22"/>
        <source>Multiple alignment file:</source>
        <translation>Çoklu hizalama dosyası:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="39"/>
        <source>File to save HMM profile:</source>
        <translation>HMM profilini kaydetmek için dosya:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="58"/>
        <source>Expert options</source>
        <translation>Uzman seçenekleri</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="70"/>
        <source>Name can be any string of non-whitespace characters (e.g. one ”word”).</source>
        <translation>Ad, boşluk olmayan herhangi bir karakter dizisi olabilir (ör. Bir &quot;kelime&quot;).</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="73"/>
        <source>Name this HMM:</source>
        <translation>Bu HMM&apos;yi adlandırın:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="84"/>
        <source>
             By default, the model is configured to find one or more nonoverlapping alignments to the complete model:
             multiple global alignments with respect to the model, and local with respect to the sequence
         </source>
        <translation>
             Varsayılan olarak model, tam modele bir veya daha fazla örtüşmeyen hizalama bulacak şekilde yapılandırılmıştır:
             modele göre çoklu global hizalama ve sekansa göre yerel
         </translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="90"/>
        <source>Default (hmmls) behaviour:</source>
        <translation>Varsayılan (hmmls) davranış:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="120"/>
        <source>
             Configure the model for finding multiple domains per sequence, where each domain can be a local (fragmentary) alignment.
             This is analogous to the old hmmfs program of HMMER 1.
         </source>
        <translation>
             Modeli, her bir alanın yerel (parçalı) bir hizalama olabileceği, dizi başına birden çok alan bulmak için yapılandırın.
             Bu, HMMER 1&apos;in eski hmmfs programına benzer.
         </translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="126"/>
        <source>Emulate hmmfs behaviour:</source>
        <translation>Hmmfs davranışını taklit edin:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="153"/>
        <source>
             Configure the model for finding a single global alignment to a target sequence,
             analogous to the old hmms program of HMMER 1.
         </source>
        <translation>
             Modeli, bir hedef diziye tek bir global hizalama bulmak için yapılandırın,
             HMMER 1&apos;in eski hmms programına benzer.
         </translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="159"/>
        <source>Emulate hmms behaviour:</source>
        <translation>Hmms davranışını taklit edin:</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="186"/>
        <source>
             Configure the model for finding a single local alignment per target sequence.
             This is analogous to the standard Smith/Waterman algorithm or the hmmsw program of HMMER 1.
         </source>
        <translation>
             Modeli, hedef sıra başına tek bir yerel hizalama bulmak için yapılandırın.
             Bu, standart Smith / Waterman algoritmasına veya HMMER 1&apos;in hmmsw programına benzer.
         </translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialog.ui" line="192"/>
        <source>Emulate hmmsw behaviour:</source>
        <translation>Hmmsw davranışını taklit edin:</translation>
    </message>
</context>
<context>
    <name>HMMBuildWorker</name>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="104"/>
        <source>Fix the length of the random sequences to &lt;n&gt;, where &lt;n&gt; is a positive (and reasonably sized) integer. &lt;p&gt;The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.</source>
        <translation>Rastgele dizilerin uzunluğunu &lt;n&gt; olarak sabitleyin; burada &lt;n&gt; pozitif (ve makul boyutta) bir tamsayıdır. &lt;p&gt; Varsayılan, bunun yerine, bir Gauss (normal) dağılımla kontrol edilen çeşitli farklı uzunluklarda diziler oluşturmaktır.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="107"/>
        <source>Mean length of the synthetic sequences, positive real number. The default value is 325.</source>
        <translation>Sentetik dizilerin ortalama uzunluğu, pozitif gerçek sayı. Varsayılan değer 325&apos;tir.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="108"/>
        <source>Number of synthetic sequences. If &lt;n&gt; is less than about 1000, the fit to the EVD may fail. &lt;p&gt;Higher numbers of &lt;n&gt; will give better determined EVD parameters. &lt;p&gt;The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.</source>
        <translation>Sentetik dizilerin sayısı. &lt;n&gt; yaklaşık 1000&apos;den azsa, EVD&apos;ye uyum başarısız olabilir. &lt;p&gt; Daha yüksek &lt;n&gt; sayısı, daha iyi belirlenmiş EVD parametrelerini verecektir. &lt;p&gt; Varsayılan 5000&apos;dir; doğruluk ve hesaplama zamanı arasında bir değiş tokuş olarak deneysel olarak seçilmiştir.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="112"/>
        <source>Standard deviation of the synthetic sequence length. A positive number. &lt;p&gt;The default is 200. Note that the Gaussian is left-truncated so that no sequences have lengths &lt;= 0.</source>
        <translation>Sentetik sıra uzunluğunun standart sapması. Pozitif bir sayı. &lt;p&gt; Varsayılan değer 200&apos;dür. Gauss&apos;un sola kesildiğine ve böylece hiçbir dizinin &lt;= 0 uzunluğuna sahip olmadığına dikkat edin.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="115"/>
        <source>The random seed, where &lt;n&gt; is a positive integer. &lt;p&gt;The default is to use time() to generate a different seed for each run, &lt;p&gt;which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. &lt;p&gt;You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.</source>
        <translation>&lt;n&gt; pozitif bir tamsayı olduğu rastgele tohum. &lt;p&gt; Varsayılan, her çalıştırma için farklı bir çekirdek oluşturmak için time () kullanmaktır, &lt;p&gt; bu, aynı HMM üzerinde iki farklı hmmkalibrasyon çalıştırmasının biraz farklı sonuçlar vereceği anlamına gelir. &lt;p&gt; Bu seçeneği, aynı HMM üzerindeki farklı hmmkalibre çalışmaları için tekrarlanabilir sonuçlar oluşturmak üzere kullanabilirsiniz.</translation>
    </message>
</context>
<context>
    <name>HMMCalibrateDialog</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="22"/>
        <source>HMM file: </source>
        <translation>HMM dosyası: </translation>
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
        <translation>Uzman seçenekleri</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="53"/>
        <source>
             Fix the length of the random sequences to n
                 , where n is a positive (and reasonably sized) integer. 
The default is instead to generate sequences with a variety of different lengths, controlled by a Gaussian (normal) distribution.</source>
        <translation>
             Rastgele dizilerin uzunluğunu n&apos;ye sabitleyin
                 , burada n pozitif (ve makul boyutta) bir tamsayıdır.
Bunun yerine varsayılan, bir Gauss (normal) dağılımla kontrol edilen çeşitli farklı uzunluklarda diziler oluşturmaktır.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="59"/>
        <source>Fix the length of the random sequences to:</source>
        <translation>Rastgele dizilerin uzunluğunu şu şekilde sabitleyin:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="104"/>
        <source>Mean length of the synthetic sequences:</source>
        <translation>Sentetik dizilerin ortalama uzunluğu:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="146"/>
        <source>
             Number of synthetic sequences.
             If n is less than about 1000, the fit to the EVD may fail
             Higher numbers of n will give better determined EVD parameters. 
             The default is 5000; it was empirically chosen as a tradeoff between accuracy and computation time.</source>
        <translation>
             Sentetik dizilerin sayısı.
             N yaklaşık 1000&apos;den küçükse, EVD&apos;ye uyum başarısız olabilir
             Daha yüksek n sayıları, daha iyi belirlenmiş EVD parametrelerini verecektir.
             Varsayılan değer 5000&apos;dir; doğruluk ve hesaplama zamanı arasında bir değiş tokuş olarak deneysel olarak seçilmiştir.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="153"/>
        <source>Number of synthetic sequences:</source>
        <translation>Sentetik dizi sayısı:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="195"/>
        <source>
             Standard deviation of the synthetic sequence length.
             A positive number. The default is 200.
             Note that the Gaussian is left-truncated so that no sequences have lengths less or equal 0.
         </source>
        <translation>
             Sentetik sıra uzunluğunun standart sapması.
             Pozitif bir sayı. Varsayılan değer 200&apos;dür.
             Hiçbir dizinin uzunluğu 0&apos;dan küçük veya 0&apos;a eşit olmayacak şekilde Gauss&apos;un sola kesildiğine dikkat edin.
         </translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="202"/>
        <source>Standard deviation:</source>
        <translation>Standart sapma:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="238"/>
        <source>
             The random seed, where n is a positive integer. 
             The default is to use time() to generate a different seed for each run, 
             which means that two different runs of hmmcalibrate on the same HMM will give slightly different results. 
             You can use this option to generate reproducible results for different hmmcalibrate runs on the same HMM.</source>
        <translation>
             Rastgele tohum, burada n pozitif bir tamsayıdır.
             Varsayılan, her çalıştırma için farklı bir çekirdek oluşturmak için time () kullanmaktır,
             bu, aynı HMM üzerinde iki farklı hmmkalibrasyon çalıştırmasının biraz farklı sonuçlar vereceği anlamına gelir.
             Aynı HMM üzerindeki farklı hmmkalibre çalışmaları için tekrarlanabilir sonuçlar oluşturmak için bu seçeneği kullanabilirsiniz.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="245"/>
        <source>Random seed:</source>
        <translation>Rastgele parçacık:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="278"/>
        <source>Save calibrated profile to file</source>
        <translation>Kalibre edilmiş profili dosyaya kaydedin</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="290"/>
        <source>Save calibrated profile to file:</source>
        <translation>Kalibre edilmiş profili dosyaya kaydedin:</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="14"/>
        <source>HMM Calibrate</source>
        <translation>HMM Kalibre Et</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialog.ui" line="101"/>
        <source>Mean length of the synthetic sequences, positive real number. The default value is 325.</source>
        <translation>Sentetik dizilerin ortalama uzunluğu, pozitif gerçek sayı. Varsayılan değer 325&apos;tir.</translation>
    </message>
</context>
<context>
    <name>HMMSearchDialog</name>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="23"/>
        <source>HMM Search</source>
        <translation>HMM Araması</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="37"/>
        <source>File with HMM profile:</source>
        <translation>HMM profiline sahip dosya:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="47"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="62"/>
        <source>Expert options</source>
        <translation>Uzman seçenekleri</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="77"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="80"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="83"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="226"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="229"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="232"/>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="92"/>
        <source>E-value filtering can be used to exclude low-probability hits from result.</source>
        <translation>E-değer filtreleme, düşük olasılıklı isabetleri sonuçtan hariç tutmak için kullanılabilir.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="86"/>
        <source>Filter results with E-value greater then:</source>
        <translation>E-değeri daha büyük olan sonuçları filtreleyin:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="119"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="122"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="125"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="135"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="138"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="141"/>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="93"/>
        <source>Score based filtering is an alternative to E-value filtering to exclude low-probability hits from result.</source>
        <translation>Puan tabanlı filtreleme, sonuçtan düşük olasılıklı isabetleri hariç tutmak için E-değer filtrelemeye bir alternatiftir.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="128"/>
        <source>Filter results with Score lower than:</source>
        <translation>Şundan düşük Puanı olan sonuçları filtrele:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="188"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="191"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="194"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="210"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="213"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="216"/>
        <source>Calculate the E-value scores as if we had seen a sequence database of &lt;n&gt; sequences.</source>
        <translation>E-değeri puanlarını, &lt;n&gt; dizilerinin bir dizi veritabanı görmüş gibi hesaplayın.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="219"/>
        <source>Number of sequences in dababase:</source>
        <translation>Veritabanındaki dizi sayısı:</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="277"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="280"/>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="283"/>
        <source>Variants of algorithm</source>
        <translation>Algoritmanın çeşitleri</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="241"/>
        <source>1E</source>
        <translation>1E</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialog.ui" line="257"/>
        <source>Algorithm</source>
        <translation>Algoritma</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="91"/>
        <source>Calculate the E-value scores as if we had seen a sequence database of &amp;lt;n&amp;gt; sequences.</source>
        <translation>E-değeri puanlarını, bir &amp;lt;n&amp;gt; diziler.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="70"/>
        <source>HMM Profile</source>
        <translation>HMM Profili</translation>
    </message>
</context>
<context>
    <name>U2::GTest_uHMMERCalibrate</name>
    <message>
        <location filename="../src/u_tests/uhmmerTests.cpp" line="569"/>
        <source>uhmmer-calibrate-subtask</source>
        <translation>uhmmer-kalibre-alt görev</translation>
    </message>
</context>
<context>
    <name>U2::HMM2QDActor</name>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="66"/>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="151"/>
        <source>HMM2</source>
        <translation>HMM2</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="77"/>
        <source>QD HMM2 search</source>
        <translation>QD HMM2 araması</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="83"/>
        <source>Power of e-value must be less or equal to zero. Using default value: 1e-1</source>
        <translation>E-değerin gücü sıfırdan küçük veya eşit olmalıdır. Varsayılan değeri kullanma: 1e-1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="152"/>
        <source>Searches HMM signals in a sequence with one or more profile HMM2 and saves the results as annotations.</source>
        <translation>HMM sinyallerini bir veya daha fazla profil HMM2 ile sırayla arar ve sonuçları açıklama olarak kaydeder.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="158"/>
        <source>Profile HMM</source>
        <translation>Profil HMM</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="159"/>
        <source>Semicolon-separated list of input HMM files.</source>
        <translation>Giriş HMM dosyalarının noktalı virgülle ayrılmış listesi.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="162"/>
        <source>Min Length</source>
        <translation>Min Uzunluk</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="163"/>
        <source>Minimum length of a result region.</source>
        <translation>Sonuç bölgesinin minimum uzunluğu.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="166"/>
        <source>Max Length</source>
        <translation>Mak uzunluk</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="167"/>
        <source>Maximum length of a result region.</source>
        <translation>Sonuç bölgesinin maksimum uzunluğu.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="170"/>
        <source>Filter by High E-value</source>
        <translation>Yüksek E-değerine göre filtreleyin</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="171"/>
        <source>Reports domains &amp;lt;= this E-value threshold in output.</source>
        <translation>Rapor alanları &amp;lt;= çıktıdaki bu E-değeri eşiği.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="175"/>
        <source>Filter by Low Score</source>
        <translation>Düşük Puana Göre Filtrele</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="176"/>
        <source>Reports domains &amp;gt;= this score cutoff in output.</source>
        <translation>Rapor alanları &amp; gt; = çıktıdaki bu puan kesintisi.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="179"/>
        <source>Number of Sequences</source>
        <translation>Dizi Sayısı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchQDActor.cpp" line="180"/>
        <source>Specifies number of significant sequences. It is used for domain E-value calculations.</source>
        <translation>Önemli dizi sayısını belirtir. Etki alanı E-değeri hesaplamaları için kullanılır.</translation>
    </message>
</context>
<context>
    <name>U2::HMMADVContext</name>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="248"/>
        <source>Find HMM signals with HMMER2...</source>
        <translation>HMMER2 ile HMM sinyallerini bulun...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="265"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="265"/>
        <source>No sequences found</source>
        <translation>Dizi bulunamadı</translation>
    </message>
</context>
<context>
    <name>U2::HMMBuildDialogController</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="56"/>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="162"/>
        <source>Build</source>
        <translation>Oluştur</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="57"/>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="163"/>
        <source>Close</source>
        <translation>Kapat</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="78"/>
        <source>Select file with alignment</source>
        <translation>Hizalamalı dosyayı seçin</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="181"/>
        <source>Select file with HMM profile</source>
        <translation>HMM profiline sahip dosyayı seçin</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="100"/>
        <source>Incorrect alignment file!</source>
        <translation>Yanlış hizalama dosyası!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="105"/>
        <source>Incorrect HMM file!</source>
        <translation>Yanlış HMM dosyası!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="121"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="130"/>
        <source>Starting build process</source>
        <translation>Derleme sürecini başlatma</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="133"/>
        <source>Hide</source>
        <translation>Gizle</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="134"/>
        <source>Cancel</source>
        <translation>Vazgeç</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="156"/>
        <source>HMM build finished with errors: %1</source>
        <translation>HMM derlemesi şu hatalarla tamamlandı: %1</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="158"/>
        <source>HMM build canceled</source>
        <translation>HMM derlemesi iptal edildi</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="160"/>
        <source>HMM build finished successfully!</source>
        <translation>HMM derlemesi başarıyla tamamlandı!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="171"/>
        <source>Progress: %1%</source>
        <translation>İlerleme: %1%</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="184"/>
        <source>HMM models</source>
        <translation>HMM modelleri</translation>
    </message>
</context>
<context>
    <name>U2::HMMBuildTask</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="328"/>
        <source>Build HMM profile &apos;%1&apos;</source>
        <translation>&apos;%1&apos; HMM profilini oluştur</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="345"/>
        <source>Multiple alignment is empty</source>
        <translation>Çoklu hizalama boş</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="349"/>
        <source>Multiple alignment is of 0 length</source>
        <translation>Çoklu hizalama 0 uzunluktadır</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="354"/>
        <source>Invalid alphabet! Only amino and nucleic alphabets are supported</source>
        <translation>Geçersiz alfabe! Yalnızca amino ve nükleik alfabeler desteklenir</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="362"/>
        <source>Error creating MSA structure</source>
        <translation>MSA yapısı oluşturulurken hata meydana geldi</translation>
    </message>
</context>
<context>
    <name>U2::HMMBuildToFileTask</name>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="310"/>
        <source>none</source>
        <translation>hiçbiri</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="198"/>
        <source>Build HMM profile &apos;%1&apos; -&gt; &apos;%2&apos;</source>
        <translation>HMM profilini oluştur &apos;%1&apos; -&gt; &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="211"/>
        <source>Error reading alignment file</source>
        <translation>Hizalama dosyası okunurken hata oluştu</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="224"/>
        <source>Build HMM profile to &apos;%1&apos;</source>
        <translation>HMM profilini &apos;%1&apos; olarak oluşturun</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="248"/>
        <source>Incorrect input file</source>
        <translation>Yanlış girdi dosyası</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="253"/>
        <source>Alignment object not found!</source>
        <translation>Hizalama nesnesi bulunamadı!</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="295"/>
        <source>Source alignment</source>
        <translation>Kaynak hizalaması</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="296"/>
        <source>Profile name</source>
        <translation>Profil adı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="298"/>
        <source>Task was not finished</source>
        <translation>Görev tamamlanmadı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="303"/>
        <source>Profile file</source>
        <translation>Profil dosyası</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildDialogController.cpp" line="304"/>
        <source>Expert options</source>
        <translation>Uzman seçenekleri</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateDialogController</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="47"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="145"/>
        <source>Calibrate</source>
        <translation>Kalibre et</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="48"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="146"/>
        <source>Close</source>
        <translation>Kapat</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="60"/>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="163"/>
        <source>Select file with HMM model</source>
        <translation>HMM modeliyle dosya seçin</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="81"/>
        <source>Incorrect HMM file!</source>
        <translation>Yanlış HMM dosyası!</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="87"/>
        <source>Illegal fixed length value!</source>
        <translation>Geçersiz sabit uzunluk değeri!</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="104"/>
        <source>Invalid output file name</source>
        <translation>Geçersiz çıktı dosyası adı</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="110"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="120"/>
        <source>Starting calibration process</source>
        <translation>Kalibrasyon işleminin başlatılması</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="123"/>
        <source>Hide</source>
        <translation>Gizle</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="124"/>
        <source>Cancel</source>
        <translation>Vazgeç</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="139"/>
        <source>Calibration finished with errors: %1</source>
        <translation>Kalibrasyon hatalarla tamamlandı: %1</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="141"/>
        <source>Calibration was cancelled</source>
        <translation>Kalibrasyon iptal edildi</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="143"/>
        <source>Calibration finished successfully!</source>
        <translation>Kalibrasyon başarıyla tamamlandı!</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="153"/>
        <source>Progress: %1%</source>
        <translation>İlerleme: %1%</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateDialogController.cpp" line="166"/>
        <source>HMM models</source>
        <translation>HMM modelleri</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateParallelSubTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="130"/>
        <source>Parallel HMM calibration subtask</source>
        <translation>Paralel HMM kalibrasyon alt görevi</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateParallelTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="32"/>
        <source>HMM calibrate &apos;%1&apos;</source>
        <translation>HMM kalibre &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="15"/>
        <source>HMM calibrate &apos;%1&apos;</source>
        <translation>HMM kalibre &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::HMMCalibrateToFileTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="150"/>
        <source>HMM calibrate &apos;%1&apos;</source>
        <translation>HMM kalibre &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="172"/>
        <source>HMMReadTask didn&apos;t generate &quot;hmm&quot; object, stop.</source>
        <translation>HMMReadTask &quot;hmm&quot; nesnesi oluşturmadı, dur.</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="192"/>
        <source>Source profile</source>
        <translation>Kaynak profil</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="195"/>
        <source>Task was not finished</source>
        <translation>Görev tamamlanmadı</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="200"/>
        <source>Result profile</source>
        <translation>Sonuç profili</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="201"/>
        <source>Expert options</source>
        <translation>Uzman seçenekleri</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="203"/>
        <source>Number of random sequences to sample</source>
        <translation>Örneklenecek rastgele dizilerin sayısı</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="204"/>
        <source>Random number seed</source>
        <translation>Rastgele sayı tohumu</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="205"/>
        <source>Mean of length distribution</source>
        <translation>Uzunluk dağılımının ortalaması</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="206"/>
        <source>Standard deviation of length distribution</source>
        <translation>Uzunluk dağılımının standart sapması</translation>
    </message>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="208"/>
        <source>Calculated evidence (mu , lambda)</source>
        <translation>Hesaplanan kanıt (mu, lambda)</translation>
    </message>
</context>
<context>
    <name>U2::HMMCreateWPoolTask</name>
    <message>
        <location filename="../src/u_calibrate/HMMCalibrateTask.cpp" line="93"/>
        <source>Initialize parallel context</source>
        <translation>Paralel bağlamı başlatın</translation>
    </message>
</context>
<context>
    <name>U2::HMMIO</name>
    <message>
        <location filename="../src/HMMIO.cpp" line="221"/>
        <location filename="../src/HMMIO.cpp" line="241"/>
        <location filename="../src/HMMIO.cpp" line="410"/>
        <location filename="../src/HMMIO.cpp" line="416"/>
        <location filename="../src/HMMIO.cpp" line="442"/>
        <location filename="../src/HMMIO.cpp" line="473"/>
        <location filename="../src/HMMIO.cpp" line="498"/>
        <source>Illegal line</source>
        <translation>Yasadışı hat</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="228"/>
        <source>File format is not supported</source>
        <translation>Dosya biçimi desteklenmiyor</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="296"/>
        <location filename="../src/HMMIO.cpp" line="301"/>
        <location filename="../src/HMMIO.cpp" line="308"/>
        <location filename="../src/HMMIO.cpp" line="313"/>
        <location filename="../src/HMMIO.cpp" line="320"/>
        <location filename="../src/HMMIO.cpp" line="325"/>
        <location filename="../src/HMMIO.cpp" line="332"/>
        <location filename="../src/HMMIO.cpp" line="338"/>
        <location filename="../src/HMMIO.cpp" line="347"/>
        <location filename="../src/HMMIO.cpp" line="352"/>
        <location filename="../src/HMMIO.cpp" line="364"/>
        <location filename="../src/HMMIO.cpp" line="373"/>
        <location filename="../src/HMMIO.cpp" line="378"/>
        <location filename="../src/HMMIO.cpp" line="422"/>
        <location filename="../src/HMMIO.cpp" line="427"/>
        <location filename="../src/HMMIO.cpp" line="431"/>
        <location filename="../src/HMMIO.cpp" line="448"/>
        <location filename="../src/HMMIO.cpp" line="452"/>
        <location filename="../src/HMMIO.cpp" line="457"/>
        <location filename="../src/HMMIO.cpp" line="464"/>
        <location filename="../src/HMMIO.cpp" line="479"/>
        <location filename="../src/HMMIO.cpp" line="488"/>
        <location filename="../src/HMMIO.cpp" line="504"/>
        <location filename="../src/HMMIO.cpp" line="512"/>
        <location filename="../src/HMMIO.cpp" line="519"/>
        <location filename="../src/HMMIO.cpp" line="524"/>
        <source>Invalid file structure near %1</source>
        <translation>%1 yakınında geçersiz dosya yapısı</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="358"/>
        <source>ALPH must precede NULE in HMM save files</source>
        <translation>ALPH, HMM kaydetme dosyalarında NULE&apos;den önce gelmelidir</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="265"/>
        <location filename="../src/HMMIO.cpp" line="393"/>
        <location filename="../src/HMMIO.cpp" line="397"/>
        <source>Value is illegal: %1</source>
        <translation>Değer yasa dışı: %1</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="71"/>
        <source>Alphabet is not set</source>
        <translation>Alfabe ayarlanmadı</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="226"/>
        <source>Model &quot;%1&quot; is a HMMER3 model, it can&apos;t be used with HMMER2.</source>
        <translation>&quot;%1&quot; modeli bir HMMER3 modelidir, HMMER2 ile kullanılamaz.</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="401"/>
        <source>Value is not set for &apos;%1&apos;</source>
        <translation>&apos;%1&apos; için değer ayarlanmadı</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="538"/>
        <source>No &apos;//&apos; symbol found</source>
        <translation>&apos;//&apos; sembolü bulunamadı</translation>
    </message>
    <message>
        <location filename="../src/HMMIO.cpp" line="656"/>
        <source>HMM models</source>
        <translation>HMM modelleri</translation>
    </message>
</context>
<context>
    <name>U2::HMMMSAEditorContext</name>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="208"/>
        <source>Build HMMER2 profile</source>
        <translation>HMMER2 profili oluştur</translation>
    </message>
</context>
<context>
    <name>U2::HMMReadTask</name>
    <message>
        <location filename="../src/HMMIO.cpp" line="670"/>
        <source>Read HMM profile &apos;%1&apos;.</source>
        <translation>&apos;%1&apos; HMM profilini okuyun.</translation>
    </message>
</context>
<context>
    <name>U2::HMMSearchDialogController</name>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="116"/>
        <source>Select file with HMM model</source>
        <translation>HMM modeliyle dosya seçin</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="133"/>
        <source>HMM file not set!</source>
        <translation>HMM dosyası ayarlanmadı!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="149"/>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="155"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="155"/>
        <source>Cannot create an annotation object. Please check settings</source>
        <translation>Ek açıklama nesnesi oluşturulamaz. Lütfen ayarları kontrol edin</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="169"/>
        <source>Starting search process</source>
        <translation>Arama işleminin başlatılması</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="172"/>
        <source>Hide</source>
        <translation>Gizle</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="173"/>
        <source>Cancel</source>
        <translation>Vazgeç</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="188"/>
        <source>HMM search finished with error: %1</source>
        <translation>HMM araması şu hatayla tamamlandı: %1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="190"/>
        <source>HMM search finished successfully!</source>
        <translation>HMM araması başarıyla tamamlandı!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="200"/>
        <source>Progress: %1%</source>
        <translation>İlerleme: %1%</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="69"/>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="192"/>
        <source>Search</source>
        <translation>Ara</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="70"/>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="193"/>
        <source>Close</source>
        <translation>Kapat</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="84"/>
        <source>SSE optimized</source>
        <translation>SSE optimize edildi</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="85"/>
        <source>Conservative</source>
        <translation>Muhafazakar</translation>
    </message>
</context>
<context>
    <name>U2::HMMSearchTask</name>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="44"/>
        <source>HMM Search</source>
        <translation>HMM Araması</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="257"/>
        <source>Invalid HMM alphabet!</source>
        <translation>Geçersiz HMM alfabesi!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="261"/>
        <source>Invalid sequence alphabet!</source>
        <translation>Geçersiz sıra alfabesi!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="289"/>
        <source>Amino translation is not available for the sequence alphabet!</source>
        <translation>Sıralı alfabe için amino çevirisi mevcut değil!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="322"/>
        <source>Parallel HMM search</source>
        <translation>Paralel HMM araması</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchTask.cpp" line="37"/>
        <source>HMM search with &apos;%1&apos;</source>
        <translation>&apos;%1&apos; ile HMM araması</translation>
    </message>
</context>
<context>
    <name>U2::HMMSearchToAnnotationsTask</name>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="218"/>
        <source>HMM search, file &apos;%1&apos;</source>
        <translation>HMM araması, dosya &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="224"/>
        <source>RAW alphabet is not supported!</source>
        <translation>RAW alfabesi desteklenmiyor!</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="240"/>
        <source>Annotation object was removed</source>
        <translation>Ek açıklama nesnesi kaldırıldı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="267"/>
        <source>HMM profile used</source>
        <translation>HMM profili kullanıldı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="270"/>
        <source>Task was not finished</source>
        <translation>Görev tamamlanmadı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="275"/>
        <source>Result annotation table</source>
        <translation>Sonuç ek açıklama tablosu</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="276"/>
        <source>Result annotation group</source>
        <translation>Sonuç ek açıklama grubu</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="277"/>
        <source>Result annotation name</source>
        <translation>Sonuç ek açıklama adı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchDialogController.cpp" line="280"/>
        <source>Results count</source>
        <translation>Sonuç sayısı</translation>
    </message>
</context>
<context>
    <name>U2::HMMWriteTask</name>
    <message>
        <location filename="../src/HMMIO.cpp" line="689"/>
        <source>Write HMM profile &apos;%1&apos;</source>
        <translation>HMM profilini &apos;%1&apos; yazın</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMBuildPrompter</name>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="210"/>
        <source>For each MSA from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; kaynaklı her MSA için,</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="214"/>
        <source> and calibrate</source>
        <translation> ve kalibre et</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="216"/>
        <source>default</source>
        <translation>varsayılan</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="216"/>
        <source>custom</source>
        <translation>özel</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="218"/>
        <source>%1 build%2 HMM profile using &lt;u&gt;%3&lt;/u&gt; settings.</source>
        <translation>%1, &lt;u&gt;%3&lt;/u&gt; ayarlarını kullanarak %2 HMM profili oluştur.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMBuildWorker</name>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="328"/>
        <source>Built HMM profile</source>
        <translation>HMM profili oluştur</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="334"/>
        <source>Calibrated HMM profile</source>
        <translation>Kalibre edilmiş HMM profili</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="137"/>
        <source>HMM2 Build</source>
        <translation>HMM2 Yapısı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="82"/>
        <source>HMM profile</source>
        <translation>HMM profili</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="92"/>
        <source>HMM strategy</source>
        <translation>HMM stratejisi</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="92"/>
        <source>Specifies kind of alignments you want to allow.</source>
        <translation>İzin vermek istediğiniz hizalama türlerini belirtir.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="93"/>
        <source>Profile name</source>
        <translation>Profil adı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="93"/>
        <source>Descriptive name of the HMM profile.</source>
        <translation>HMM profilinin açıklayıcı adı.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="100"/>
        <source>Calibrate profile</source>
        <translation>Kalibrasyon profili</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="102"/>
        <source>Parallel calibration</source>
        <translation>Paralel kalibrasyon</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="104"/>
        <source>Fixed length of samples</source>
        <translation>Sabit uzunlukta numune</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="107"/>
        <source>Mean length of samples</source>
        <translation>Ortalama numune uzunluğu</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="108"/>
        <source>Number of samples</source>
        <translation>Numune sayısı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="112"/>
        <source>Standard deviation</source>
        <translation>Standart sapma</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="115"/>
        <source>Random seed</source>
        <translation>Rastgele parçacık</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="184"/>
        <source>Default</source>
        <translation>Öntanımlı</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="249"/>
        <source>Incorrect value for seed parameter</source>
        <translation>Parçacık parametresi için yanlış değer</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="267"/>
        <source>Schema name not specified. Using default value: &apos;%1&apos;</source>
        <translation>Şema adı belirtilmedi. Varsayılan değer kullanılıyor: &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="81"/>
        <source>Input MSA</source>
        <translation>MSA&apos;yı girin</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="81"/>
        <source>Input multiple sequence alignment for building statistical model.</source>
        <translation>İstatistiksel model oluşturmak için çoklu dizi hizalamasını girin.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="82"/>
        <source>Produced HMM profile</source>
        <translation>Üretilen HMM profili</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="100"/>
        <source>Enables/disables optional profile calibration.&lt;p&gt;An empirical HMM calibration costs time but it only has to be done once per model, and can greatly increase the sensitivity of a database search.</source>
        <translation>İsteğe bağlı profil kalibrasyonunu etkinleştirir / devre dışı bırakır.&lt;p&gt;Deneysel bir HMM kalibrasyonu zaman alır, ancak model başına yalnızca bir kez yapılması gerekir ve bir veritabanı aramasının hassasiyetini büyük ölçüde artırabilir.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="102"/>
        <source>Number of parallel threads that the calibration will run in.</source>
        <translation>Kalibrasyonun çalışacağı paralel iplik sayısı.</translation>
    </message>
    <message>
        <location filename="../src/u_build/HMMBuildWorker.cpp" line="137"/>
        <source>Builds a HMM profile from a multiple sequence alignment.&lt;p&gt;The HMM profile is a statistical model which captures position-specific information about how conserved each column of the alignment is, and which residues are likely.</source>
        <translation>Çoklu dizi hizalamasından bir HMM profili oluşturur.&lt;p&gt;HMM profili, hizalamanın her bir sütununun ne kadar korunduğu ve hangi kalıntıların muhtemel olduğu hakkında konuma özgü bilgileri yakalayan istatistiksel bir modeldir.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMLib</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="73"/>
        <source>HMMER2 Tools</source>
        <translation>HMMER2 Araçları</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="63"/>
        <source>HMM Profile</source>
        <translation>HMM Profili</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="133"/>
        <location filename="../src/HMMIOWorker.cpp" line="151"/>
        <source>HMM profile</source>
        <translation>HMM profili</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="134"/>
        <source>Location</source>
        <translation>Konum</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="159"/>
        <source>Read HMM2 Profile</source>
        <translation>HMM2 Profilini Oku</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="145"/>
        <source>Write HMM2 Profile</source>
        <translation>HMM2 Profili Yaz</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="133"/>
        <source>Input HMM profile</source>
        <translation>HMM profilini girin</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="134"/>
        <source>Location hint for the target file.</source>
        <translation>Hedef dosya için konum ipucu.</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="145"/>
        <source>Saves all input HMM profiles to specified location.</source>
        <translation>Tüm giriş HMM profillerini belirtilen konuma kaydeder.</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="159"/>
        <source>Reads HMM profiles from file(s). The files can be local or Internet URLs.</source>
        <translation>HMM profillerini dosyalardan okur. Dosyalar yerel veya İnternet URL&apos;leri olabilir.</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="151"/>
        <source>Loaded HMM profile</source>
        <translation>Yüklü HMM profili</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMReadPrompter</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="171"/>
        <source>Read HMM profile(s) from %1.</source>
        <translation>%1&apos;den HMM profillerini okuyun.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMReader</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="240"/>
        <source>Loaded HMM profile from %1</source>
        <translation>%1&apos;den HMM profili yüklendi</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMSearchPrompter</name>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="147"/>
        <source>For each sequence from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; &apos;den her bir dizi için,</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="151"/>
        <source>Use &lt;u&gt;default&lt;/u&gt; settings.</source>
        <translation>&lt;u&gt;Varsayılan&lt;/u&gt;ayarları kullanın.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="151"/>
        <source>Use &lt;u&gt;custom&lt;/u&gt; settings.</source>
        <translation>&lt;u&gt;Özel&lt;/u&gt;ayarları kullanın.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="153"/>
        <source>%1 HMM signals%2. %3&lt;br&gt;Output the list of found regions annotated as &lt;u&gt;%4&lt;/u&gt;.</source>
        <translation>%1 HMM sinyalleri %2. %3&lt;br&gt;Bulunan bölgelerin listesini &lt;u&gt;%4&lt;/u&gt; ek açıklamalı olarak yazdırın.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="148"/>
        <source>using all profiles provided by &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>&lt;u&gt;%1&lt;/u&gt; tarafından sağlanan tüm profilleri kullanarak,</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMSearchWorker</name>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="236"/>
        <source>Bad sequence supplied to input: %1</source>
        <translation>Girişe hatalı sıra sağlandı: %1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="260"/>
        <source>Found %1 HMM signals</source>
        <translation>%1 HMM sinyali bulundu</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="74"/>
        <source>HMM profile</source>
        <translation>HMM profili</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="75"/>
        <source>Input sequence</source>
        <translation>Giriş sırası</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="76"/>
        <source>HMM annotations</source>
        <translation>HMM ek açıklamaları</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="90"/>
        <source>Result annotation</source>
        <translation>Sonuç ek açıklaması</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="91"/>
        <source>Number of seqs</source>
        <translation>Sıra sayısı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="92"/>
        <source>Filter by high E-value</source>
        <translation>Yüksek E-değerine göre filtreleyin</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="93"/>
        <source>Filter by low score</source>
        <translation>Düşük puana göre filtrele</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="101"/>
        <source>HMM2 Search</source>
        <translation>HMM2 Arama</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="179"/>
        <source>Power of e-value must be less or equal to zero. Using default value: 1e-1</source>
        <translation>E-değerin gücü sıfırdan küçük veya eşit olmalıdır. Varsayılan değeri kullanma: 1e-1</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="188"/>
        <source>Value for attribute name is empty, default name used</source>
        <translation>Öznitelik adı değeri boş, varsayılan ad kullanıldı</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="232"/>
        <source>Find HMM signals in %1</source>
        <translation>%1&apos;de HMM sinyallerini bulun</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="74"/>
        <source>HMM profile(s) to search with.</source>
        <translation>Aranacak HMM profil (ler) i.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="75"/>
        <source>An input sequence (nucleotide or protein) to search in.</source>
        <translation>Aranacak bir girdi dizisi (nükleotid veya protein).</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="76"/>
        <source>Annotations marking found similar sequence regions.</source>
        <translation>İşaretleme notları, benzer dizi bölgeleri buldu.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="90"/>
        <source>A name of the result annotations.</source>
        <translation>Sonuç ek açıklamalarının adı.</translation>
    </message>
    <message>
        <location filename="../src/u_search/HMMSearchWorker.cpp" line="101"/>
        <source>Searches each input sequence for significantly similar sequence matches to all specified HMM profiles. In case several profiles were supplied, searches with all profiles one by one and outputs united set of annotations for each sequence.</source>
        <translation>Belirtilen tüm HMM profilleriyle önemli ölçüde benzer dizi eşleşmeleri için her giriş sırasını arar. Birkaç profilin sağlanması durumunda, tüm profilleri tek tek arar ve her bir sekans için birleşik açıklama kümeleri çıkarır.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMWritePrompter</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="177"/>
        <source>unset</source>
        <translation>ayarlanmadı</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="181"/>
        <source>Save HMM profile(s) from &lt;u&gt;%1&lt;/u&gt; to &lt;u&gt;%2&lt;/u&gt;.</source>
        <translation>&lt;u&gt;%1&lt;/u&gt;&apos;den&lt;u&gt;%2&lt;/u&gt;&apos; ye HMM profillerini kaydedin.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::HMMWriter</name>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="264"/>
        <source>Empty HMM passed for writing to %1</source>
        <translation>%1&apos;e yazmak için boş HMM geçti</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="264"/>
        <source>Unspecified URL for writing HMM</source>
        <translation>HMM yazmak için belirtilmemiş URL</translation>
    </message>
    <message>
        <location filename="../src/HMMIOWorker.cpp" line="275"/>
        <source>Writing HMM profile to %1</source>
        <translation>HMM profili %1&apos;e yazılıyor</translation>
    </message>
</context>
<context>
    <name>U2::UHMMBuild</name>
    <message>
        <location filename="../src/u_build/uhmmbuild.cpp" line="196"/>
        <source>bogus configuration choice</source>
        <translation>sahte yapılandırma seçimi</translation>
    </message>
</context>
<context>
    <name>U2::uHMMPlugin</name>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="75"/>
        <source>HMM2</source>
        <translation>HMM2</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="75"/>
        <source>Based on HMMER 2.3.2 package. Biological sequence analysis using profile hidden Markov models</source>
        <translation>HMMER 2.3.2 paketine dayanmaktadır. Profil gizli Markov modellerini kullanarak biyolojik sekans analizi</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="77"/>
        <source>Build HMM2 profile...</source>
        <translation>HMM2 profili oluştur...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="82"/>
        <source>Calibrate profile with HMMER2...</source>
        <translation>Profili HMMER2 ile kalibre edin...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="87"/>
        <source>Search with HMMER2...</source>
        <translation>HMMER2 ile ara...</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="184"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/uHMMPlugin.cpp" line="184"/>
        <source>Error! Select sequence in Project view or open sequence view.</source>
        <translation>Hata! Proje görünümünde veya açık sekans görünümünde sekansı seçin.</translation>
    </message>
</context>
</TS>
