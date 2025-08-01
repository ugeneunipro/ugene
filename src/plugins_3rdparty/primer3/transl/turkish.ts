<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Main</source>
        <translation>Ana</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Targets</source>
        <translation>Hedefler</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;start,length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of the excluded region, and length is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligolar bu etikette belirtilen herhangi bir bölgeyle çakışamaz. İlişkili değer,&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt; başlangıç, uzunluk &lt;/span&gt;&lt;/p&gt;&lt;p&gt;çiftlerinin boşlukla ayrılmış bir listesi olmalıdır. Hariç tutulan bölgenin ilk tabanının indeksi ve uzunluk onun uzunluğudur. Bu etiket, düşük sıra kalitesindeki bölgeleri hariç tutmak veya ALU&apos;lar veya LINE&apos;lar gibi tekrar eden öğeleri içeren bölgeleri hariç tutmak gibi görevler için kullanışlıdır. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;start,length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of a Target, and length is its length.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir veya daha fazla Hedef belirtilirse, en az birinin yanında yasal bir primer çifti bulunmalıdır. Bir Hedef, basit bir dizi tekrar sahası (örneğin, bir CA tekrarı) veya bir tek-baz-çift polimorfizmi olabilir. Değer, başlangıç dizini olan &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:-600;&quot;&gt;başlangıç, uzunluk &lt;/span&gt;&lt;/p&gt;&lt;p&gt; çiftlerinin boşlukla ayrılmış bir listesi olmalıdır Hedefin ilk tabanının uzunluğu ve uzunluğu onun uzunluğudur. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of product size ranges, for example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 first tries to pick primers in the first range. If that is not possible, it goes to the next range and tries again. It continues in this way until it has either picked all necessary primers or until there are no more ranges. For technical reasons this option makes much lighter computational demands than the Product Size option.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Ürün beden aralıklarının bir listesi, örneğin:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 ilk olarak ilk aralıktaki primerleri seçmeye çalışır. Bu mümkün değilse, bir sonraki aralığa gider ve tekrar dener. Bu şekilde, gerekli tüm primerleri toplayana veya daha fazla aralık kalmayana kadar devam eder. Teknik nedenlerden dolayı bu seçenek, Ürün Boyutu seçeneğinden çok daha hafif hesaplama talepleri sağlar.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This selection indicates what mispriming library (if any) Primer3 should use to screen for interspersed repeats or for other sequence to avoid as a location for primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu seçim, Primer3&apos;ün serpiştirilmiş tekrarları veya primerler için bir konum olarak kaçınılması gereken diğer dizileri taramak için hangi yanlış priming kitaplığının (varsa) kullanması gerektiğini belirtir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of primer pairs to return.&lt;/p&gt;&lt;p&gt;Primer pairs returned are sorted by their &amp;quot;quality&amp;quot;, in other words by the value of the objective function (where a lower number indicates a better primer pair).&lt;/p&gt;&lt;p&gt;Caution: setting this parameter to a large value will increase running time.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Döndürülecek maksimum primer çifti sayısı.&lt;/p&gt;&lt;p&gt;Döndürülen primer çiftleri, kendilerine göre sıralanır &amp;quot;kalite&amp;quot;,başka bir deyişle, amaç fonksiyonunun değeri ile (burada daha düşük bir sayı, daha iyi bir primer çiftini gösterir).&lt;/p&gt;&lt;p&gt;Dikkat: Bu parametrenin büyük bir değere ayarlanması çalışma süresini artıracaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum stability for the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;p&gt;Bigger numbers mean more stable 3&apos; ends. The value is the maximum delta G (kcal/mol) for duplex disruption for the five 3&apos; bases as calculated using the Nearest-Neighbor parameter values specified by the option of &apos;Table of thermodynamic parameters&apos;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sol veya sağ primerin son beş 3 &apos;tabanı için maksimum kararlılık. &lt;/p&gt;&lt;p&gt; Daha büyük sayılar daha kararlı 3&apos; uçlar anlamına gelir. Değer, &apos;Termodinamik parametreler tablosu&apos; seçeneği ile belirtilen En Yakın Komşu parametre değerleri kullanılarak hesaplanan beş 3 &apos;baz için çift yönlü bozulma için maksimum delta G&apos;dir (kcal / mol). &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed weighted similarity with any sequence in Mispriming Library.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Yanlış Eşleme Kitaplığındaki herhangi bir sıra ile izin verilen maksimum ağırlıklı benzerlik.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed sum of similarities of a primer pair (one similarity for each primer) with any single sequence in Mispriming Library. &lt;/p&gt;&lt;p&gt;Library sequence weights are not used in computing the sum of similarities.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primer çiftinin (her primer için bir benzerlik), Mispriming Kitaplığındaki herhangi bir tek sıra ile izin verilen maksimum benzerlik toplamı. &lt;/p&gt;&lt;p&gt;Benzerliklerin toplamının hesaplanmasında kütüphane sıra ağırlıkları kullanılmaz.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the sequence from which you are designing the primers.&lt;/p&gt;&lt;p&gt;The scoring system is the same as used for Max Mispriming, except that an ambiguity code is never treated as a consensus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Astarları tasarladığınız sıradaki ektopik sitelere izin verilen maksimum benzerlik..&lt;/p&gt;&lt;p&gt;Puanlama sistemi, bir belirsizlik kodunun hiçbir zaman bir fikir birliği olarak değerlendirilmemesi dışında, Max Mispriming için kullanılanla aynıdır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the sequence from which you are designing the primers.&lt;/p&gt;&lt;p&gt;The scoring system is the same as used for Max Mispriming, except that an ambiguity code is never treated as a consensus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Astarları tasarladığınız sıradaki her iki primerin ektopik bölgelere izin verilen maksimum toplam benzerliği.&lt;/p&gt;&lt;p&gt;Puanlama sistemi, bir belirsizlik kodunun hiçbir zaman bir fikir birliği olarak değerlendirilmemesi dışında, Max Mispriming için kullanılanla aynıdır..&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Index of the first base of a start codon.&lt;/p&gt;&lt;p&gt;This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter. Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir başlangıç kodonunun ilk tabanının dizini.&lt;/p&gt;&lt;p&gt;Bu parametre, Primer3&apos;ün çerçeve içi amplikonlar, ör. bir füzyon proteini için bir şablon oluşturmak için. Primer3, ideal olarak başlangıç kodonunun solunda veya solunda veya gerekirse sağında başlayarak bir çerçeve içi sol primer seçmeye çalışacaktır. Bu parametrenin negatif değerleri, gerçek başlangıç kodonu mevcut dizinin solundaysa yasaldır. Bu parametre negatif değilse, Primer3 bu parametre ile belirtilen konumdaki kodon bir ATG değilse bir hata sinyali verir. -10 ^ 6&apos;dan küçük veya ona eşit bir değer, Primer3&apos;ün bu parametreyi göz ardı etmesi gerektiğini gösterir. Primer3, bir durdurma kodonu için sol primerden sağa tarayarak sağ primerin konumunu seçer. İdeal olarak, doğru primer durdurma kodonunda veya sonrasında sona erecektir.&lt;/p&gt;&lt;p&gt;Bu parametre DENEYSEL olarak düşünülmelidir. Lütfen çıktıyı dikkatlice kontrol edin; bazı hatalı girişler Primer3&apos;te hataya neden olabilir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pick left primer</source>
        <translation>Sol astarı seçin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>or use left primer below</source>
        <translation>veya aşağıdaki sol astarı kullanın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>General Settings</source>
        <translation>Genel Ayarlar</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers such settings as primer size, temperature, GC and other general settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölüm astar boyutu, sıcaklık, GC ve diğer genel ayarlar gibi ayarları kapsar.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Min</source>
        <translation>Asg</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Opt</source>
        <translation>Ter</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max</source>
        <translation>Azm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer Tm</source>
        <translation>Astar Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum lengths (in bases) of a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick primers shorter than Min or longer than Max, and with default arguments will attempt to pick primers close with size close to Opt.&lt;/p&gt;&lt;p&gt;Min cannot be smaller than 1. Max cannot be larger than 36. (This limit is governed by maximum oligo size for which melting-temperature calculations are valid.)&lt;/p&gt;&lt;p&gt;Min cannot be greater than Max.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primer oligo&apos;nun Minimum, Optimum ve Maksimum uzunlukları (baz olarak).&lt;/p&gt;&lt;p&gt;Primer3, Min&apos;den daha kısa veya Max&apos;ten daha uzun primerleri seçmeyecek ve varsayılan bağımsız değişkenlerle, Opt. Boyutuna yakın olan primerleri seçmeye çalışacaktır.&lt;/p&gt;&lt;p&gt;Min, 1&apos;den küçük olamaz. Maks 36&apos;dan büyük olamaz. (Bu sınır, erime sıcaklığı hesaplamalarının geçerli olduğu maksimum oligo boyutuna tabidir.)&lt;/p&gt;&lt;p&gt;Min, Max&apos;ten büyük olamaz.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum melting temperatures (Celsius) for a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick oligos with temperatures smaller than Min or larger than Max, and with default conditions will try to pick primers with melting temperatures close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir astar oligo için Minimum, Optimum ve Maksimum erime sıcaklıkları (Santigrat).&lt;/p&gt;&lt;p&gt;Primer3, Min&apos;den küçük veya Maks&apos;dan daha büyük sıcaklıklara sahip oligoları seçmeyecek ve varsayılan koşullarda, Opt. Değerine yakın erime sıcaklıklarına sahip primerleri seçmeye çalışacaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Herhangi bir primer veya oligo&apos;da Minimum, Optimum ve Maksimum G ve C yüzdesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Herhangi bir primer veya oligo&apos;da Minimum, Optimum ve Maksimum G ve C yüzdesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum, optimum, and maximum melting temperature of the amplicon.&lt;/p&gt;&lt;p&gt;Primer3 will not pick a product with melting temperature less than min or greater than max. If Opt is supplied and the Penalty Weights for Product Size are non-0 Primer3 will attempt to pick an amplicon with melting temperature close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Amplikonun minimum, optimum ve maksimum erime sıcaklığı.&lt;/p&gt;&lt;p&gt;Primer3, erime sıcaklığı minimumdan düşük veya maks. Seçenek sağlanırsa ve Ürün Boyutu için Ceza Ağırlıkları 0 değilse, Primer3 Opt. 3&apos;e yakın erime sıcaklığına sahip bir amplikon seçmeye çalışacaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum acceptable (unsigned) difference between the melting temperatures of the left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sol ve sağ primerlerin erime sıcaklıkları arasındaki maksimum kabul edilebilir (işaretsiz) fark.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Table of thermodynamic parameters</source>
        <translation>Termodinamik parametreler tablosu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primers</source>
        <translation>Astarlar</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer Pairs</source>
        <translation>Astar Çiftleri</translation>
    </message>
    <message>
        <source>Hyb Oligos</source>
        <translation type="vanished">Hyb Oligos</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section allows one to set output annotations&apos; settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölüm, çıktı açıklamalarının ayarlarının yapılmasına izin verir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Breslauer et. al 1986</source>
        <translation>Breslauer et. al 1986</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>SantaLucia 1998</source>
        <translation>SantaLucia 1998</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer Designer</source>
        <translation>Astar Tasarımcısı</translation>
    </message>
    <message>
        <source>Excluded regions</source>
        <translation type="vanished">Hariç tutulan bölgeler</translation>
    </message>
    <message>
        <source>Product size ranges</source>
        <translation type="vanished">Ürün boyutu aralıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Mispriming/Repeat library</source>
        <translation>Hatalı priming / Tekrar kitaplığı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Number to return</source>
        <translation>Döndürülecek numara</translation>
    </message>
    <message>
        <source>Max repeat mispriming</source>
        <translation type="vanished">Max yanlış priming işlemini tekrarlayın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max template mispriming</source>
        <translation>Maks şablon yanlış priming</translation>
    </message>
    <message>
        <source>Max 3&apos; stability</source>
        <translation type="vanished">Max 3 &apos;kararlılık</translation>
    </message>
    <message>
        <source>Pair max repeat mispriming</source>
        <translation type="vanished">Çift maks. Yanlış priming tekrarı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pair max template mispriming</source>
        <translation>Çift maks. Şablonda yanlış priming</translation>
    </message>
    <message>
        <source>Start codon position</source>
        <translation type="vanished">Kodon konumunu başlat</translation>
    </message>
    <message>
        <source>Pick hybridization probe (internal oligo)</source>
        <translation type="vanished">Hibridizasyon probu seçin (dahili oligo)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>or use oligo below</source>
        <translation>veya aşağıdaki oligo kullanın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pick right primer</source>
        <translation>Doğru astarı seçin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>or use right primer below (5&apos; to 3&apos; on opposite strand)</source>
        <translation>veya aşağıdaki sağ astarı kullanın (zıt iplikçikte 5 &apos;ila 3&apos;)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer size</source>
        <translation>Astar boyutu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Product Tm</source>
        <translation>Ürün Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer GC%</source>
        <translation>Astar % GC</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max Tm difference</source>
        <translation>Max Tm farkı</translation>
    </message>
    <message>
        <source>Max self complementarity</source>
        <translation type="vanished">Maksimum kendini tamamlayıcılık</translation>
    </message>
    <message>
        <source>Max 3&apos; self complementarity</source>
        <translation type="vanished">Maksimum 3 &apos;kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Inside target penalty</source>
        <translation>Hedef içi ceza</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Outside target penalty</source>
        <translation>Hedef dışı ceza</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max poly-X</source>
        <translation>Maksimum poli-X</translation>
    </message>
    <message>
        <source>First base index</source>
        <translation type="vanished">İlk temel dizin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>CG clamp</source>
        <translation>CG kelepçesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the salt correction formula for the melting temperature calculation.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Erime sıcaklığı hesaplaması için tuz düzeltme formülünü belirtir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Annealing oligo concentration</source>
        <translation type="vanished">Tavlama oligo konsantrasyonu</translation>
    </message>
    <message>
        <source> Liberal base</source>
        <translation type="vanished"> Serbest temel</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Show debugging info</source>
        <translation>Hata ayıklama bilgilerini göster</translation>
    </message>
    <message>
        <source>Hyb oligo excluded region</source>
        <translation type="vanished">Hyb oligo hariç bölge</translation>
    </message>
    <message>
        <source>Hyb oligo size</source>
        <translation type="vanished">Hyb oligo boyutu</translation>
    </message>
    <message>
        <source>Hyb oligo Tm</source>
        <translation type="vanished">Hyb oligo Tm</translation>
    </message>
    <message>
        <source>Hyb oligo GC%</source>
        <translation type="vanished">Hyb oligo GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Size for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için Primer Boyutunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Tm for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için Primer Tm&apos;nin eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer% GC&apos;nin eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için Primer GC% eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Hyb oligo self complementarity</source>
        <translation type="vanished">Hyb oligo kendini tamamlayıcılık</translation>
    </message>
    <message>
        <source>Hyb oligo mishyb library</source>
        <translation type="vanished">Hyb oligo mishyb kütüphanesi</translation>
    </message>
    <message>
        <source>Hyb oligo min sequence quality</source>
        <translation type="vanished">Hyb oligo min sekans kalitesi</translation>
    </message>
    <message>
        <source>Hyb oligo conc of monovalent cations</source>
        <translation type="vanished">Tek değerlikli katyonların Hyb oligo konsantrasyonu</translation>
    </message>
    <message>
        <source>Hyb oligo conc of divalent cations</source>
        <translation type="vanished">İki değerlikli katyonların Hyb oligo konsantrasyonu</translation>
    </message>
    <message>
        <source>Hyb oligo max 3&apos; self complementarity</source>
        <translation type="vanished">Hyb oligo max 3 &apos;kendini tamamlayıcılık</translation>
    </message>
    <message>
        <source>Hyb oligo max poly-X</source>
        <translation type="vanished">Hyb oligo max poli-X</translation>
    </message>
    <message>
        <source>Hyb oligo max mishyb</source>
        <translation type="vanished">Hyb oligo max mishyb</translation>
    </message>
    <message>
        <source>Hyb oligo [dNTP]</source>
        <translation type="vanished">Hyb oligo [dNTP]</translation>
    </message>
    <message>
        <source>Hyb oligo DNA concentration</source>
        <translation type="vanished">Hyb oligo DNA konsantrasyonu</translation>
    </message>
    <message>
        <source>Self complementarity</source>
        <translation type="vanished">Kendini tamamlayıcılık</translation>
    </message>
    <message>
        <source>3&apos; self complementarity</source>
        <translation type="vanished">3 &apos;kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>RT-PCR</source>
        <translation>RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Design primers for RT-PCR analysis</source>
        <translation>RT-PCR analizi için tasarım primerleri</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Exon annotation name:</source>
        <translation>Ekson ek açıklama adı:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>exon</source>
        <translation>ekson</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Minimum exon junction overlap size</source>
        <translation>Minimum ekson bağlantı üst üste binme boyutu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Exon range:</source>
        <translation>Ekson aralığı:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max number of pairs to query:</source>
        <translation>Sorgulanacak maksimum çift sayısı:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Min sequence quality:</source>
        <translation>Minimum sıra kalitesi:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Min 3&apos; sequence quality:</source>
        <translation>Min 3 &apos;sekans kalitesi:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequence quality range min:</source>
        <translation>Sıra kalite aralığı min:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequence quality range max:</source>
        <translation>Sıra kalite aralığı maks:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Posterior Actions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can define actions for primers when Primer3 calculation is already finished - for example, check if result primer pairs self- and hetero-dimers are good enough.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Check self- and hetero-dimers of primer pairs and filter pairs with inappropriate parameters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Check complementary</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Save primer report in CSV file format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max base pairs in dimer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Generate CSV report</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Maximum persantage of G/C base pairs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source> %</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max dimer GC-content</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source> bp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Help</source>
        <translation>Yardım</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Save settings</source>
        <translation>Ayarları kaydet</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Load settings</source>
        <translation>Yükleme Ayarları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Reset form</source>
        <translation>Formu Sıfırla</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pick primers</source>
        <translation>Astarları seçin</translation>
    </message>
    <message>
        <source>Concentration of monovalent cations</source>
        <translation type="vanished">Tek değerlikli katyonların konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies details of melting temperature calculation.&lt;/p&gt;&lt;p&gt;First method uses the table of thermodynamic parameters from the paper [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83:4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Second method (recommended) was suggested in the paper [SantaLucia JR (1998), Proc Natl Acad Sci 95:1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Erime sıcaklığı hesaplamasının ayrıntılarını belirtir.&lt;/p&gt;&lt;p&gt;İlk yöntem, kağıttaki termodinamik parametreler tablosunu kullanır [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83: 4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Makalede ikinci yöntem (önerilen) önerilmiştir [SantaLucia JR (1998), Proc Natl Acad Sci 95: 1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Max #N&apos;s</source>
        <translation type="vanished">Max #N&apos;s</translation>
    </message>
    <message>
        <source>Concentration of divalent cations</source>
        <translation type="vanished">İki değerlikli katyonların konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Salt correction formula</source>
        <translation>Tuz düzeltme formülü</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Schildkraut and Lifson 1965</source>
        <translation>Schildkraut ve Lifson 1965</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Owczarzy et. 2004</source>
        <translation>Owczarzy et. 2004</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Concentration of dNTPs</source>
        <translation>DNTP&apos;lerin konsantrasyonu</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity, and the maximum allowable 3&apos;-anchored global alignment score when testing for complementarity between left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Kendini tamamlayıcılık için tek bir primer test edilirken izin verilen maksimum 3&apos;-bağlantılı global hizalama skoru ve sol ve sağ primerler arasında tamamlayıcılık için test edilirken maksimum izin verilebilir 3&apos;-bağlantılı global hizalama skoru.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable local alignment score when testing a single primer for (local) self-complementarity and the maximum allowable local alignment score when testing for complementarity between left and right primers.&lt;/p&gt;&lt;p&gt;Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;(Yerel) kendi kendini tamamlayıcılık için tek bir primer test edilirken izin verilen maksimum yerel hizalama puanı ve sol ve sağ primerler arasında tamamlayıcılık için test edilirken izin verilen maksimum yerel hizalama puanı.&lt;/p&gt;&lt;p&gt;PCR&apos;de zorunlu olarak kendinden hazırlamaya neden olmadan primerlerin birbirine tavlanma eğilimini tahmin etmek için yerel kendi kendini tamamlayıcılık alınır. Puanlama sistemi, tamamlayıcı bazlar için 1.00, herhangi bir taban (veya N) ile bir N ile eşleşme için -0.25, uyumsuzluk için -1.00 ve bir boşluk için -2.00 verir. Yalnızca tek baz çifti boşluklarına izin verilir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum number of unknown bases (N) allowable in any primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Herhangi bir astarda izin verilen maksimum bilinmeyen baz sayısı (N).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable length of a mononucleotide repeat, for example AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir mononükleotid tekrarının izin verilen maksimum uzunluğu, örneğin AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Varsayılan olmayan değerler yalnızca 0 veya 1 hedef bölgeli diziler için geçerlidir.&lt;/p&gt;&lt;p&gt;Primer, bir hedefi kapsayan ve hedefle örtüşmeyen bir çiftin parçasıysa, bu değeri 3 &apos;ucundan (benzersiz) hedefe kadar olan nükleotid konumlarının sayısıyla çarparak&apos; pozisyon cezası &apos;elde edin. Bu parametrenin etkisi, Primer3&apos;ün hedefe yakınlığı amaç fonksiyonunda bir terim olarak dahil etmesine izin vermektir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter is the index of the first base in the input sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu parametre, giriş dizisindeki ilk tabanın dizinidir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Require the specified number of consecutive Gs and Cs at the 3&apos; end of both the left and right primer.&lt;/p&gt;&lt;p&gt;This parameter has no effect on the internal oligo if one is requested.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Hem sol hem de sağ primerin 3 &apos;ucunda belirtilen sayıda ardışık G ve C gerektir.&lt;/p&gt;&lt;p&gt;Bu parametrenin, istenmesi durumunda dahili oligo üzerinde hiçbir etkisi yoktur.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar (mM) concentration of monovalent salt cations (usually KCl) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to calculate oligo and primer melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;PCR&apos;de monovalent tuz katyonlarının (genellikle KCl) milimolar (mM) konsantrasyonu.&lt;/p&gt;&lt;p&gt;Primer3 bu argümanı oligo ve primer erime sıcaklıklarını hesaplamak için kullanır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of divalent salt cations (usually MgCl^(2+)) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 converts concentration of divalent cations to concentration of monovalent cations using following formula:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Monovalent cations] = [Monovalent cations] + 120*(([divalent cations] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In addition, if the specified concentration of dNTPs is larger than the concentration of divalent cations then the effect of the divalent cations is not considered.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;PCR&apos;deki iki değerlikli tuz katyonlarının (genellikle MgCl ^ (2+)) milimolar konsantrasyonu.&lt;/p&gt;&lt;p&gt;Primer3, aşağıdaki formülü kullanarak iki değerlikli katyonların konsantrasyonunu tek değerlikli katyonların konsantrasyonuna dönüştürür:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Tek değerlikli katyonlar] = [Tek değerlikli katyonlar] + 120 * (([iki değerlikli katyonlar] - [dNTP]) ^ 0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Ek olarak, belirtilen dNTP konsantrasyonu iki değerlikli katyonların konsantrasyonundan daha büyükse, iki değerlikli katyonların etkisi dikkate alınmaz.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of the sum of all deoxyribonucleotide triphosphates.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tüm deoksiribonükleotid trifosfatların toplamının milimolar konsantrasyonu.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A value to use as nanomolar (nM) concentration of each annealing oligo over the course the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to esimate oligo melting temperatures.&lt;/p&gt;&lt;p&gt;The default (50nM) works well with the standard protocol used at the Whitehead/MIT Center for Genome Research --0.5 microliters of 20 micromolar concentration for each primer in a 20 microliter reaction with 10 nanograms template, 0.025 units/microliter Taq polymerase in 0.1 mM each dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) using 35 cycles with an annealing temperature of 56 degrees Celsius.&lt;/p&gt;&lt;p&gt;The value of this parameter is less than the actual concentration of oligos in the initial reaction mix because  it is the concentration of annealing oligos, which in turn depends on the amount of template (including PCR product) in a given cycle. This concentration increases a great deal during a PCR; fortunately PCR seems quite robust for a variety of oligo melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;PCR boyunca her tavlama oligo&apos;nun nanomolar (nM) konsantrasyonu olarak kullanılacak bir değer&lt;/p&gt;&lt;p&gt;Primer3 bu argümanı oligo erime sıcaklıklarını tahmin etmek için kullanır.&lt;/p&gt;&lt;p&gt;Varsayılan (50nM), Whitehead / MIT Genom Araştırma Merkezi&apos;nde kullanılan standart protokolle iyi çalışır - 10 nanogram şablonlu 20 mikrolitre reaksiyonda her primer için 20 mikromolar konsantrasyonda 0,5 mikrolitre, 0,1&apos;de 0,025 birim / mikrolitre Taq polimeraz 56 derece Celsius tavlama sıcaklığı ile 35 döngü kullanılarak her bir dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) mM.&lt;/p&gt;&lt;p&gt;Bu parametrenin değeri, başlangıç reaksiyon karışımındaki gerçek oligo konsantrasyonundan daha azdır çünkü  bu, belirli bir döngüdeki şablon miktarına (PCR ürünü dahil) bağlı olan tavlama oligolarının konsantrasyonudur. Bu konsantrasyon, bir PCR sırasında büyük ölçüde artar; Neyse ki PCR, çeşitli oligo eritme sıcaklıkları için oldukça sağlam görünmektedir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;.&lt;/p&gt;&lt;p&gt;The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Varsayılan olmayan değerler yalnızca 0 veya 1 hedef bölgeli diziler için geçerlidir.&lt;/p&gt;&lt;p&gt;Primer, bir hedefi kapsayan ve hedefle örtüşen bir çiftin parçasıysa, bu değeri, &apos;pozisyon cezasını&apos; elde etmek için primerin (benzersiz) hedefle örtüştüğü nükleotid pozisyonlarının sayısıyla çarpın.&lt;/p&gt;&lt;p&gt;Bu parametrenin etkisi, Primer3&apos;ün amaç işlevinde bir terim olarak hedefle örtüşmeyi içermesine izin vermektir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Do not treat ambiguity codes in libraries as consensus</source>
        <translation type="vanished">Kitaplıklardaki belirsizlik kodlarını fikir birliği olarak değerlendirmeyin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Lowercase masking</source>
        <translation>Küçük harf maskeleme</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo</source>
        <translation>Dahili Oligo</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameters governing choice of internal oligos are analogous to the parameters governing choice of primer pairs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligoların seçimini yöneten parametreler, primer çiftlerinin seçimini yöneten parametrelere benzer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&amp;lt;start&amp;gt;,&amp;lt;length&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &amp;lt;start&amp;gt; is the index of the first base of an excluded region, and &amp;lt;length&amp;gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Ortadaki oligolar, bu etiketle belirtilen herhangi bir bölgeyle çakışamaz. İlişkili değer boşlukla ayrılmış bir liste olmalıdır&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&amp;lt;başlangıç&amp;gt;,&amp;lt;uzunluk&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;çiftler, burada &amp;lt;başlangıç&amp;gt; hariç tutulan bir bölgenin ilk tabanının dizinidir ve &amp;lt;uzunluk&amp;gt; uzunluğu. Genellikle iç oligolar için Hedef bölgeler hariç tutulan bölgeler yapılır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Min:</source>
        <translation>Asgari:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Opt:</source>
        <translation>Opt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max:</source>
        <translation>Azami:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max #Ns</source>
        <translation>Max #Ns</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max self complemntarity for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer maksimum kendi kendine uyumluluğunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max #Ns for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer max #Ns&apos;nin eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to primer mispriming library, except that the event we seek to avoid is hybridization of the internal oligo to sequences in this library rather than priming from them.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer yanlış priming kitaplığına benzer, ancak kaçınmaya çalıştığımız olay, dahili oligo&apos;nun, bunlardan hazırlanmak yerine bu kitaplıktaki dizilere melezleştirilmesidir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer minimum quality for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer minimum kalite eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of monovalent cations for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için tek değerlikli katyonların primer konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of divalent cations for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için iki değerlikli katyonların primer konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max 3&apos; self complementarity for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer max 3 &apos;kendi kendini tamamlama eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max poly-X for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer max poly-X&apos;in eşdeğer parametresi&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to primer max library mispriming.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer max kitaplık yanlış priming&apos;e benzer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of the dNTPs for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için dNTP&apos;lerin primer konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer DNA concentration for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer DNA konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Penalty Weights</source>
        <translation>Ceza Ağırlıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Tm</source>
        <translation>Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Size  </source>
        <translation>Boyut  </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>GC%</source>
        <translation>GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Lt:</source>
        <translation>Lt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Gt:</source>
        <translation>Gt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>#N&apos;s</source>
        <translation>#N&apos;s</translation>
    </message>
    <message>
        <source>Mispriming</source>
        <translation type="vanished">Yanlış astarlama</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequence quality</source>
        <translation>Sıra kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>End sequence quality</source>
        <translation>Bitiş sırası kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Position penalty</source>
        <translation>Pozisyon cezası</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>End stability</source>
        <translation>Son kararlılık</translation>
    </message>
    <message>
        <source>Template mispriming</source>
        <translation type="vanished">Şablon yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Product size</source>
        <translation>Ürün boyutu</translation>
    </message>
    <message>
        <source>Tm difference</source>
        <translation type="vanished">Tm farkı</translation>
    </message>
    <message>
        <source>Any complementarity</source>
        <translation type="vanished">Herhangi bir tamamlayıcılık</translation>
    </message>
    <message>
        <source>3&apos; complementarity</source>
        <translation type="vanished">3 &apos;tamamlayıcılık</translation>
    </message>
    <message>
        <source>Pair mispriming</source>
        <translation type="vanished">Çift yanlış priming</translation>
    </message>
    <message>
        <source>Primer penalty weight</source>
        <translation type="vanished">Astar ceza ağırlığı</translation>
    </message>
    <message>
        <source>Hyb oligo penalty weight</source>
        <translation type="vanished">Hyb oligo ceza ağırlığı</translation>
    </message>
    <message>
        <source>Primer pair template mispriming weight</source>
        <translation type="vanished">Astar çifti şablonu yanlış astarlama ağırlığı</translation>
    </message>
    <message>
        <source>Hyb oligo #N&apos;s</source>
        <translation type="vanished">Hyb oligo #N&apos;s</translation>
    </message>
    <message>
        <source>Hyb oligo mishybing</source>
        <translation type="vanished">Hyb oligo mishybing</translation>
    </message>
    <message>
        <source>Hyb oligo sequence quality</source>
        <translation type="vanished">Hyb oligo sekans kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>mRNA sequence</source>
        <translation>mRNA dizisi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>At 5&apos; side (bp):</source>
        <translation>5 &apos;tarafında (bp):</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>At 3&apos;side (bp)</source>
        <translation>3&apos;ün yanında (bp)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer product must span at least one intron on the corresponding genomic DNA</source>
        <translation>Primer ürün, karşılık gelen genomik DNA üzerinde en az bir intronu kapsamalıdır</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequence Quality</source>
        <translation>Sıra Kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;
   &lt;head/&gt;
   &lt;body&gt;
      &lt;p&gt;Specifies the Primer3 parameters set. Following presets are available:&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Default&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Primer3 default parameters.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Recombinase Polymerase Amplification&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Preset with weaker temperature bounds, longer primer and product lengths. Also, this parameter enables &quot;Check complementary&quot; by default (see the &quot;Posterior Actions&quot; tab).&lt;/p&gt;
   &lt;/body&gt;
&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Choose preset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the Primer3 parameters set. Following presets are available: &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Default&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 default parameters. &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Default2&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 default parameters with advanced thermodynamics. &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;qPCR&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 parameters for Quantitative Polymerase Chain Reaction. &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Cloning Primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 parameters for cloning primers picking. &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Annealing Temp&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 parameters for annealing.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Secondary Structures&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 parameters for secondary structures estimation.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Probe&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 parameters for probe calculation.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Recombinase Polymerase Amplification&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Preset with weaker temperature bounds, longer primer and product lengths. Also, this parameter enables &amp;quot;Check complementary&amp;quot; by default (see the &amp;quot;Posterior Actions&amp;quot; tab). &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Default</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Default2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>qPCR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Cloning Primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Annealing Temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Secondary Structures</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Probe</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Recombinase Polymerase Amplification</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag tells Primer3 what task to perform. Legal values are:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Design a primer pair (the classic Primer3 task) if the &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=1&lt;/span&gt;. In addition, pick an internal hybridization oligo if &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;NOTE: If &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=0&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;, then behaves similarly to &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_TASK=pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 only checks the primers provided in &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INTERNAL_OLIGO&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_REVCOMP&lt;/span&gt;. It is the only task that does not require a sequence. However, if &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; is provided, it is used.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick all primers in &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; (possibly limited by &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_EXCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_PAIR_OK_REGION_LIST&lt;/span&gt;, etc.). Returns the primers sorted by quality starting with the best primers. If &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER&lt;/span&gt; is selected Primer3 does not to pick primer pairs but generates independent lists of left primers, right primers, and, if requested, internal oligos.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to sequence a region. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; can be used to indicate several targets. The position of each primer is calculated for optimal sequencing results.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to clone a gene were the start nucleotide and the end nucleotide of the PCR fragment must be fixed, for example to clone an ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt; must be used to indicate the first and the last nucleotide. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to select primers which bind with their end at a specific position. This can be used to force the end of a primer to a polymorphic site, with the goal of discriminating between sequence variants. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; must be used to provide a single target indicating the last nucleotide of the left (nucleotide before the first nucleotide of the target) and the right primer (nucleotide following the target). The primers border the &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt;. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Select the Task for primer selection</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag tells Primer3 what task to perform. Legal values are:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Design a primer pair (the classic Primer3 task) if the &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer &lt;/span&gt;is checked and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick right primer&lt;/span&gt; is checked. In addition, pick an internal hybridization oligo if &lt;span style=&quot; font-weight:700;&quot;&gt;Pick internal oligo&lt;/span&gt; is checked.&lt;/p&gt;&lt;p&gt;NOTE: If &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt; Pick right primer&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick internal oligo &lt;/span&gt;are checked, then behaves similarly to &lt;span style=&quot; font-weight:700;&quot;&gt;Primer task &lt;/span&gt;is&lt;span style=&quot; font-weight:700;&quot;&gt; pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 only checks the primers provided in &lt;span style=&quot; font-weight:700;&quot;&gt;Left primer&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Internal oligo&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Right primer&lt;/span&gt;. It is the only task that does not require a sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick all primers in sequence (possibly limited by &lt;span style=&quot; font-weight:700;&quot;&gt;Include region&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Exclude region&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Pair OK Region List&lt;/span&gt;, etc.). Returns the primers sorted by quality starting with the best primers. If &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick right primer &lt;/span&gt;are selected, Primer3 does not to pick primer pairs but generates independent lists of left primers, right primers, and, if requested, internal oligos.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to sequence a region. The position of each primer is calculated for optimal sequencing results.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to clone a gene were the start nucleotide and the end nucleotide of the PCR fragment must be fixed, for example to clone an ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;Included region&lt;/span&gt; must be used to indicate the first and the last nucleotide. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;Pick anyway&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to select primers which bind with their end at a specific position. This can be used to force the end of a primer to a polymorphic site, with the goal of discriminating between sequence variants. &lt;span style=&quot; font-weight:700;&quot;&gt;Targets&lt;/span&gt; must be used to provide a single target indicating the last nucleotide of the left (nucleotide before the first nucleotide of the target) and the right primer (nucleotide following the target). The primers border the &lt;span style=&quot; font-weight:700;&quot;&gt;Targets&lt;/span&gt;. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;Pick anyway&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>generic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>pick_sequencing_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>pick_primer_list</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>check_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>pick_cloning_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>pick_discriminative_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick left primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick an internal oligo (hybridization probe to detect the PCR product).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pick internal oligo</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick a right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of a left primer to check and around which to design right primers and optional internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of an internal oligo to check and around which to design left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of a right primer to check and around which to design left primers and optional internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the left primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>5&apos; Overhang:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the right primer. It is reverse complementary to the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of a Target, and length is its &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this list is not empty, then the forward OR the reverse primer must overlap one of these junction points by at least &lt;span style=&quot; font-weight:700;&quot;&gt;3 Prime Junction Overlap&lt;/span&gt; nucleotides at the 3&apos; end and at least &lt;span style=&quot; font-weight:700;&quot;&gt;5 Prime Junction Overlap&lt;/span&gt; nucleotides at the 5&apos; end.&lt;/p&gt;&lt;p&gt;In more detail: The junction associated with a given position is the space immediately to the right of that position in the template sequence on the strand given as input.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Overlap Junction List</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of the excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Excluded Regions</source>
        <translation type="unfinished">Hariç Tutulan Bölgeler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag allows detailed specification of possible locations of left and right primers in primer pairs.&lt;/p&gt;&lt;p&gt;The associated value must be a semicolon-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;quadruples. The left primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; and the right primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; specify the location of the left primer in terms of the index of the first base in the region and the length of the region. &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt; specify the location of the right primer in analogous fashion. As seen in the example below, if no integers are specified for a region then the location of the corresponding primer is not constrained.&lt;/p&gt;&lt;p&gt;Example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Pair OK Region List&lt;/span&gt;=100,50,300,50 ; 900,60,, ; ,,930,100&lt;/p&gt;&lt;p&gt;Specifies that there are three choices:&lt;/p&gt;&lt;p&gt;Left primer in the 50 bp region starting at position 100 AND right primer in the 50 bp region starting at position 300&lt;/p&gt;&lt;p&gt;OR&lt;/p&gt;&lt;p&gt;Left primer in the 60 bp region starting at position 900 (and right primer anywhere)&lt;/p&gt;&lt;p&gt;OR&lt;/p&gt;&lt;p&gt;Right primer in the 100 bp region starting at position 930 (and left primer anywhere)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pair OK Region List</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A sub-region of the given sequence in which to pick primers. For example, often the first dozen or so bases of a sequence are vector, and should be excluded from consideration. The value for this parameter has the form&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base to consider, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is the number of subsequent bases in the primer-picking region.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Included region</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL at this point. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;br&gt;&lt;br&gt;
Index of the first base of a start codon. This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter.&lt;br&gt;&lt;br&gt;
Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Start Codon Position</source>
        <translation type="unfinished">Codon Pozisyonunu Başlat</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of the start codon, by default ATG. Some bacteria use different start codons, this tag allows one to specify alternative start codons.

Any triplet can be provided as start codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Start Codon Sequence</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Force Left Primer Start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Force Left Primer End</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 5&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucleotides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 5&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Force Right Primer End</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 3&apos; end. Similar parameter to &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt;, but limits the 3&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucleotides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 3&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 3&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Force Right Primer Start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>ATG</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Product Size Ranges</source>
        <translation type="unfinished">Ürün Boyut Aralıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max 3&apos; Stability</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pair Max Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum acceptable fraction of primer bound at &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Temperature&lt;/span&gt; for a primer oligo in percent.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The annealing temperature (Celsius) used in the PCR reaction. Usually it is chosen up to 10°C below the melting temperature of the primers. If provided, Primer3 will calculate the fraction of primers bound at the provided annealing temperature for each oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Annealing Temperature</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the propensity of oligos to form hairpins and dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Use Thermodynamic Oligo Alignment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as&lt;span style=&quot; font-weight:700;&quot;&gt; Max 3&apos; Self Complementarity&lt;/span&gt; but is based on thermodynamical approach - the stability of structure is analyzed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This is the most stable monomer structure of internal oligo calculated by thermodynamic approach. The hairpin loops, bulge loops, internal loops, internal single mismatches, dangling ends, terminal mismatches have been considered. This parameter is calculated only if &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Oligo Alignment&lt;/span&gt; is checked. The default value is 10 degrees lower than the default value of &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Pair Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END of the left primer to the right primer and scores the best binding it can find. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max 3&apos; Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of a primer to bind to itself (interfering with target sequence binding). It will score ANY binding occurring within the entire primer sequence.&lt;/p&gt;&lt;p&gt;It is the maximum allowable local alignment score when testing a single primer for (local) self-complementarity. Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; but all calculations are based on thermodynamical approach.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Max Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Max Primer Hairpin	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Max 3&apos; Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer. It is the maximum allowable local alignment score when testing for complementarity between left and right primers. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Max Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Max 3&apos; Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END to a identical primer and scores the best binding it can find. This is critical for primer quality because it allows primers use itself as a target and amplify a short piece (forming a primer-dimer). These primers are then unable to bind and amplify the target sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; is the maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity. The 3&apos;-anchored global alignment score is taken to predict the likelihood of PCR-priming primer-dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max 3&apos; Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to anneal to undesired sites in the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Use Thermodynamic Template Alignment	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max template mispriming&lt;/span&gt; but assesses alternative binding sites in the template using thermodynamic models. This parameter specifies the maximum allowed melting temperature of an oligo (primer) at an &amp;quot;ectopic&amp;quot; site within the template sequence; 47.0 would be a reasonable choice if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt; is 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Max Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed melting temperatures of both primers at ectopic sites within the template (with the two primers in an orientation that would allow PCR amplification.) The melting temperatures are calculated as for &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Template Mispriming&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Pair Max Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Advanced Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers more accurate settings, which are related to &lt;span style=&quot; font-weight:700;&quot;&gt;General Settings&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the reverse strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequencing Interval</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the calculated position of the 3&apos;end to both sides in which Primer3Plus picks the best primer. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of Gs or Cs allowed in the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max GC in primer 3&apos; end</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Annealing Oligo Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Concentration of Divalent Cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values are valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the point were the trace signals are readable. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>5 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequencing Accuracy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>First Base Index</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequencing Lead</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;When returning multiple primer pairs, the minimum number of base pairs between the 3&apos; ends of any two left primers.&lt;/p&gt;&lt;p&gt;Primers with 3&apos; ends at positions e.g. 30 and 31 in the template sequence have a three-prime distance of 1.&lt;/p&gt;&lt;p&gt;In addition to positive values, the values -1 and 0 are acceptable and have special interpretations:&lt;/p&gt;&lt;p&gt;-1 indicates that a given left primer can appear in multiple primer pairs returned by Primer3. This is the default behavior.&lt;/p&gt;&lt;p&gt;0 indicates that a left primer is acceptable if it was not already used. In other words, two left primers are allowed to have the same 3&apos; position provided their 5&apos; positions differ.&lt;/p&gt;&lt;p&gt;For n &amp;gt; 0: A left primer is acceptable if:&lt;/p&gt;&lt;p&gt;NOT(3&apos; end of left primer closer than n to the 3&apos; end of a previously used left primer)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the same strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Sequencing Spacing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>3 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>3&apos; End Distance Between Right Primers	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Max #N&apos;s accepted</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Non-default values are valid only for sequences with 0 or 1 target regions. If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>3&apos; End Distance Between Left Primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of DMSO in percent. See PRIMER_DMSO_FACTOR for details of Tm correction.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Concentration of Monovalent Cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of formamide in mol/l. The melting temperature of primers can be approximately corrected for formamide:&lt;/p&gt;&lt;p&gt;Tm = Tm (without formamide) +(0.453 * PRIMER_[LEFT/INTERNAL/RIGHT]_4_GC_PERCENT / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; correction was suggested by Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Convert % into mol/l:&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % weight] * 10 / 45.04 g/mol&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % volume] * 10 * 1.13 g/cm3 / 45.04 g/mol&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The melting temperature of primers can be approximately corrected for DMSO:&lt;/p&gt;&lt;p&gt;Tm = Tm (without DMSO) - &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; concentration must be given in %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Use formatted output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this flag is true, produce PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN and/or PRIMER_PAIR_EXPLAIN output tags as appropriate.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Print Statistics</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value = 1, then Primer3 will print out the calculated secondary structures&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Print secondary structures</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If set to 1, treat ambiguity codes as if they were consensus codes when matching oligos to mispriming or mishyb libraries. For example, if this flag is set, then a C in an oligo will be scored as a perfect match to an S in a library sequence, as will a G in the oligo. More importantly, though, any base in an oligo will be scored as a perfect match to an N in the library. This is very bad if the library contains strings of Ns, as no oligo will be legal (and it will take a long time to find this out). So unless you know for sure that your library does not have runs of Ns (or Xs), then set this flag to 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Treat ambiguity codes in libraries as consensus</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If true use primer provided in left, right, or internal primer even if it violates specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Pick anyway</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option allows for intelligent design of primers in sequence in which masked regions (for example repeat-masked regions) are lower-cased. (New in v. 1.1.0, added by Maido Remm and Triinu Koressaar)&lt;/p&gt;&lt;p&gt;A value of 1 directs Primer3 to reject primers overlapping lowercase a base exactly at the 3&apos; end.&lt;/p&gt;&lt;p&gt;This property relies on the assumption that masked features (e.g. repeats) can partly overlap primer, but they cannot overlap the 3&apos;-end of the primer. In other words, lowercase bases at other positions in the primer are accepted, assuming that the masked features do not influence the primer performance if they do not overlap the 3&apos;-end of primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter provides a quick-and-dirty way to get Primer3 to accept IUB / IUPAC codes for ambiguous bases (i.e. by changing all unrecognized bases to N). If you wish to include an ambiguous base in an oligo, you must set &lt;span style=&quot; font-weight:700;&quot;&gt;Max #N&apos;s accepted&lt;/span&gt; to a 1 (non-0) value.&lt;/p&gt;&lt;p&gt;Perhaps &apos;-&apos; and &apos;* &apos; should be squeezed out rather than changed to &apos;N&apos;, but currently they simply get converted to N&apos;s. The authors invite user comments.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Liberal base</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of an excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Excluded Region</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Overlap Positions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Bound for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Tm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt; for the internal oligo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Max 3&apos; End Self Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Internal Oligo Max 3&apos; End Self Complementarity	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Internal Oligo Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The most stable monomer structure of internal oligo calculated by thermodynamic approach. See &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;	 for details&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Internal Oligo Hairpin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Max 3&apos; End Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max poly-X&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Max Poly-X</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Oligo Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo DNA Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Library Mispriming&lt;/span&gt; except that this parameter applies to the similarity of candidate internal oligos to the library specified in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Mishyb Library&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Min sequence quality&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Min Sequence Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameter for internal oligos analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max #Ns&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo [dNTP]</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Monovalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Divalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo 5 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Conc of monovalent cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Mishyb Library</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo 3 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Max Library Mishyb	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo conc of divalent cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for internal primer / probe.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Min Internal Oligo End Distance</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölüm açıklar &amp;quot;ceza ağırlıkları&amp;quot;,bu, kullanıcının Primer3&apos;ün seçtiği kriterleri değiştirmesine izin verir.&amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;İki ağırlık sınıfı vardır: bazı parametreler için bir &apos;Lt&apos; (küçüktür) ve bir &apos;Gt&apos; (büyüktür) ağırlık vardır. Bunlar, Primer3&apos;ün, değer belirtilen optimumdan (sırasıyla) küçük veya büyük olduğunda kullandığı ağırlıklardır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimal değerden (Lt) küçük veya (Gt) büyük bir primerin karşılık gelen parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Önceden tanımlanmış optimum değerden farklı bir primer parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: 3&apos; End Self Complementarity	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Hairpin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>3&apos; End Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Determines the overall weight of the position penalty in calculating the penalty for a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir astar cezasının hesaplanmasında pozisyon cezasının toplam ağırlığını belirler.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer failure rate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer pair less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimum değerden (Lt) küçük veya (Gt) büyük bir primer çiftinin karşılık gelen parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Any Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Template Mispriming Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer pair parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Önceden tanımlanmış optimum değerden farklı bir primer çifti parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Any Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Template Mispriming Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Tm Difference</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Primer Penalty Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Penalty Weight	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a internal oligo less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimal değerden (Lt) daha küçük veya (Gt) daha büyük bir dahili oligo için karşılık gelen parametre için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal oligo Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal oligo Tm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal oligo GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal oligo Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>TH: Internal Oligo 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for an internal oligo parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Önceden tanımlanmış optimum değerden farklı bir dahili oligo parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo #N&apos;s</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Library Mishybing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Sequence Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Internal Oligo Sequence End Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölümde kullanıcı, primer ürününün ekson-ekson birleşimiyle örtüşmesi veya intron yayılması gerektiğini belirleyebilir. Bu sadece açıklamalı eksonlarla bir cDNA (mRNA) dizisi için primerler tasarlarken geçerlidir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This feature allows one to specify if primer product must span intron-exon boundaries.&lt;/p&gt;&lt;p&gt;Checking this option will result in ignoring &lt;span style=&quot; font-style:italic;&quot;&gt;Excluded&lt;/span&gt; and &lt;span style=&quot; font-style:italic;&quot;&gt;Target Regions&lt;/span&gt; from Main section.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu özellik, primer ürününün intron-ekson sınırlarını kapsaması gerekip gerekmediğini belirlemeye izin verir.&lt;/p&gt;&lt;p&gt;Bu seçeneğin işaretlenmesi, göz ardı edilmesine neden olacaktır. &lt;span style=&quot; font-style:italic;&quot;&gt;Hariç tutuldu&lt;/span&gt; ve &lt;span style=&quot; font-style:italic;&quot;&gt;Hedef Bölgeler&lt;/span&gt; Ana bölümden.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The name of the annotation which defines the exons in the mRNA sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;MRNA dizisindeki eksonları tanımlayan ek açıklamanın adı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Aralık ayarlanmışsa, primer araması seçilen eksonlarla sınırlı olacaktır. Örneğin:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Aralık, gerçek ekson aralığından daha büyükse veya başlangıç ekson sayısı ekson sayısını aşarsa, hata mesajı görüntülenir. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This number of found primer pairs wil bel queried to check if they fulfill the requirements for RTPCR.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bulunan bu sayıda primer çifti, RTPCR gereksinimlerini karşılayıp karşılamadıklarını kontrol etmek için sorgulanacaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölümde kullanıcı, hedef sıranın ve ilgili parametrelerin sıra kalitesini belirleyebilir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of space separated integers. There must be exactly one integer for each base in the Source Sequence if this argument is non-empty. High numbers indicate high confidence in the base call at that position and low numbers indicate low confidence in the base call at that position.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Boşlukla ayrılmış tam sayıların listesi. Bu bağımsız değişken boş değilse, Kaynak Sırasındaki her taban için tam olarak bir tamsayı olmalıdır. Yüksek sayılar, o konumda baz aramasında yüksek güveni gösterir ve düşük sayılar, bu konumdaki baz aramasına düşük güveni gösterir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primer içinde izin verilen minimum sıra kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within the 3&apos; pentamer of a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primerin 3 &apos;pentameri içinde izin verilen minimum sekans kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum yasal sıra kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maksimum yasal sıralama kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui"/>
        <source>Result Settings</source>
        <translation>Sonuç Ayarları</translation>
    </message>
</context>
<context>
    <name>Primer3TmCalculatorFactory</name>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorFactory.cpp" line="29"/>
        <source>Primer 3</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Primer3TmCalculatorSettingsWidget</name>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Form</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Concentration of DNA strands</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>DNA Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source> nM</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Concentration of monovalent cations</source>
        <translation type="unfinished">Tek değerlikli katyonların konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Monovalent Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source> mM</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Concentration of divalent cations</source>
        <translation type="unfinished">İki değerlikli katyonların konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Divalent Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Concentration of deoxynycleotide triphosphate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>DNTP Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Concentration of DMSO</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source> %</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Correction factor for DMSO</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Concentration of formamide</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source> mol/l</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>The maximum sequence length for using the nearest neighbor model. For sequences longer than this, uses the &quot;GC%&quot; formula</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>NN Max Length</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source> nt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Specifies the table of thermodynamic parameters and the method of melting temperature calculation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Thermodynamic Table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Breslauer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>SantaLucia</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Specifies salt correction formula for the melting temperature calculation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Salt Correction Formula</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Schildkraut</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui"/>
        <source>Owczarzy</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="167"/>
        <source>Primer</source>
        <translation>Astar</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="168"/>
        <source>PCR primer design</source>
        <translation>PCR primer tasarımı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="170"/>
        <source>Excluded regions</source>
        <translation>Hariç tutulan bölgeler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="171"/>
        <source>Targets</source>
        <translation>Hedefler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="172"/>
        <source>Product size ranges</source>
        <translation>Ürün boyutu aralıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="173"/>
        <source>Number to return</source>
        <translation>Döndürülecek numara</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="175"/>
        <source>Max repeat mispriming</source>
        <translation>Max yanlış priming işlemini tekrarlayın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="176"/>
        <source>Max template mispriming</source>
        <translation>Maks şablon yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="177"/>
        <source>Max 3&apos; stability</source>
        <translation>Max 3 &apos;kararlılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="178"/>
        <source>Pair max repeat mispriming</source>
        <translation>Çift maks. Yanlış priming tekrarı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="179"/>
        <source>Pair max template mispriming</source>
        <translation>Çift maks. Şablonda yanlış priming</translation>
    </message>
</context>
<context>
    <name>U2::CheckComplementTask</name>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="37"/>
        <source>Check complement task</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="44"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="48"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="51"/>
        <source>Sequence object has been closed, abort</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="73"/>
        <source>Check complement</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="74"/>
        <source>The following filtering settings have been used</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="76"/>
        <source>Max base pairs in dimers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="76"/>
        <source>bp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="79"/>
        <source>Max dimer GC-content</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="81"/>
        <source>The following primers have been found and processed (red - filtered, green - passed)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="86"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="141"/>
        <source>No.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="87"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="142"/>
        <source>Strand</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="88"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="143"/>
        <source>Primer</source>
        <translation type="unfinished">Astar</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="89"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="144"/>
        <source>Self-dimer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="90"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="95"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="145"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="150"/>
        <source>Delta G (kcal/mol)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="91"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="96"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="146"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="151"/>
        <source>Base Pairs (bp)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="92"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="97"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="147"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="152"/>
        <source>G/C pairs (bp)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="93"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="98"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="148"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="153"/>
        <source>G/C-content (%)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="94"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="149"/>
        <source>Hetero-dimer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="105"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="168"/>
        <source>Forward</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="121"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="182"/>
        <source>Reverse</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::FindExonRegionsTask</name>
    <message>
        <location filename="../src/task/FindExonRegionsTask.cpp" line="90"/>
        <source>Sequence object has been closed, abort</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/FindExonRegionsTask.cpp" line="95"/>
        <source>Failed to search for exon annotations. The sequence %1 doesn&apos;t have any related annotations.</source>
        <translation>Ekson notları aranamadı. %1 dizisinin ilgili ek açıklaması yok.</translation>
    </message>
</context>
<context>
    <name>U2::GTest</name>
    <message>
        <source>Illegal TARGET value: %1</source>
        <translation type="vanished">Geçersiz HEDEF değeri: %1</translation>
    </message>
    <message>
        <source>Illegal PRIMER_DEFAULT_PRODUCT value: %1</source>
        <translation type="vanished">Geçersiz PRIMER_VARSAYILAN_ÜRÜN değeri: %1</translation>
    </message>
    <message>
        <source>Illegal PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION value: %1</source>
        <translation type="vanished">Yasadışı PRIMER_DAHİLİ_OLIGO_HARİÇ_BÖLGE değeri: %1</translation>
    </message>
    <message>
        <source>Illegal INCLUDED_REGION value: %1</source>
        <translation type="vanished">Yasadışı INCLUDED_REGION değeri: %1</translation>
    </message>
    <message>
        <source>Illegal EXCLUDED_REGION value: %1</source>
        <translation type="vanished">Yasadışı EXCLUDED_REGION değeri: %1</translation>
    </message>
    <message>
        <source>Unrecognized PRIMER_TASK</source>
        <translation type="vanished">Tanınmayan PRIMER_GÖREV</translation>
    </message>
    <message>
        <source>Contradiction in primer_task definition</source>
        <translation type="vanished">Birincil görev tanımında çelişki</translation>
    </message>
    <message>
        <source>Missing SEQUENCE tag</source>
        <translation type="vanished">Eksik SEQUENCE etiketi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="128"/>
        <location filename="../src/Primer3Tests.cpp" line="134"/>
        <source>Illegal SEQUENCE_TARGET value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="148"/>
        <source>Illegal SEQUENCE_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="163"/>
        <source>Illegal SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="178"/>
        <source>Illegal SEQUENCE_EXCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="204"/>
        <source>Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="218"/>
        <source>Illegal SEQUENCE_INCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="236"/>
        <location filename="../src/Primer3Tests.cpp" line="243"/>
        <source>Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="258"/>
        <source>Illegal PRIMER_PRODUCT_SIZE_RANGE value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="409"/>
        <source>Incorrect results num. Pairs: %1, left: %2, right: %3, inernal: %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="424"/>
        <location filename="../src/Primer3Tests.cpp" line="435"/>
        <location filename="../src/Primer3Tests.cpp" line="446"/>
        <source>Incorrect parameter: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="554"/>
        <source>Error in sequence quality data</source>
        <translation>Sıra kalitesi verilerinde hata</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="558"/>
        <source>Sequence quality data missing</source>
        <translation>Sıra kalitesi verileri eksik</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="585"/>
        <source>No error, but expected: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="588"/>
        <source>An unexpected error. Expected: %1, but Actual: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="597"/>
        <source>No warning, but expected: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="601"/>
        <source>An unexpected warning. Expected: %1, but Actual: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PRIMER_PAIRS_NUMBER is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="vanished">ASTAR_ÇİFTLERİ_NUMARASI yanlış. Beklenen: %2, ancak Gerçek: %3</translation>
    </message>
    <message>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:NULL</source>
        <translation type="vanished">%1 yanlış. Beklenen:%2,%3, ancak Gerçek:BOŞ</translation>
    </message>
    <message>
        <source>%1 is incorrect. Expected:NULL, but Actual:%2,%3</source>
        <translation type="vanished">%1 yanlış. Beklenen: BOŞ, ancak Gerçek:%2,%3</translation>
    </message>
    <message>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:%4,%5</source>
        <translation type="vanished">%1 yanlış. Beklenen:%2,%3, ancak Gerçek:%4,%5</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="888"/>
        <location filename="../src/Primer3Tests.cpp" line="896"/>
        <source>%1 is incorrect. Expected:%2, but Actual:%3</source>
        <translation>%1 yanlış. Beklenen:%2, ancak Gerçekleşen:%3</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ADVContext</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="97"/>
        <source>Primer3...</source>
        <translation>Primer3...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="132"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="133"/>
        <source>Cannot create an annotation object. Please check settings</source>
        <translation>Ek açıklama nesnesi oluşturulamaz. Lütfen ayarları kontrol edin</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="80"/>
        <source>Default</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="81"/>
        <source>Default2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="82"/>
        <source>qPCR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="83"/>
        <source>Cloning Primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="84"/>
        <source>Annealing Temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="85"/>
        <source>Secondary Structures</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="86"/>
        <source>Probe</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="87"/>
        <location filename="../src/Primer3Dialog.cpp" line="1006"/>
        <source>Recombinase Polymerase Amplification</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="143"/>
        <source>Save result to file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="152"/>
        <source>Save to...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="157"/>
        <source>NONE</source>
        <translation>YOK</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="158"/>
        <source>HUMAN</source>
        <translation>İNSAN</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="159"/>
        <source>RODENT_AND_SIMPLE</source>
        <translation>KÜÇÜK_VE_BASİT</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="160"/>
        <source>RODENT</source>
        <translation>KEMİRGEN</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="161"/>
        <source>DROSOPHILA</source>
        <translation>MEYVE SİNEĞİ</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="520"/>
        <source>The &quot;Include region&quot; should be the only one.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="627"/>
        <source>%1 sequence has incorrect alphabet, should be be simple DNA.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="637"/>
        <source>Left primer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="643"/>
        <source>Left 5&apos; overhang</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="657"/>
        <source>Internal oligo</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="669"/>
        <source>Right primer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="675"/>
        <source>Right 5&apos; overhang</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="702"/>
        <source>Sequence quality list length must be equal to the sequence length. Sequence length = %1, quality list length = %2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="735"/>
        <source>Task &quot;pick_discriminative_primers&quot; requires exactly one &quot;Targets&quot; region.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="751"/>
        <source>At least one primer on the &quot;Main&quot; settings page should be enabled - this is required by the &quot;check_primers&quot; task.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="754"/>
        <source>The %1 primer on the &quot;Main&quot; settings page is enabled, but not set.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="761"/>
        <source>left</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="762"/>
        <source>internal</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="763"/>
        <source>right</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="786"/>
        <source>Primer Size Ranges should have at least one range.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="798"/>
        <source>Sequence range region is too small for current product size ranges.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="816"/>
        <source>The priming sequence is too long, please, decrease the region. Maximum length allowed: %1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="826"/>
        <source>Incorrect sum &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="831"/>
        <source>The priming sequence is out of range.
Either make the priming region end &quot;%1&quot; less or equal than the sequence size &quot;%2&quot; plus the first base index value &quot;%3&quot;or mark the sequence as circular.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="842"/>
        <source>Result file path is empty. Please, set this value on the &quot;Result Settings&quot; tab.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="858"/>
        <source>%1 parameter(s) have an incorrect value(s), pay attention on red widgets. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="861"/>
        <source>The following errors are critical and does not allow one to continue calculation: 

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="868"/>
        <source>The following not critical errors are possible: 

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="874"/>
        <source>

Fix criticals and re-run Primer3.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="877"/>
        <source>

In case of calculation, all incorrect values will be ignored. Continue?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="886"/>
        <source>The &quot;%1&quot; parameter has incorrect value, please, read the tooltip of this parameter to find out how the correct one looks like.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1008"/>
        <source>Info: &quot;Check complementary&quot; has been enabled (see the &quot;Posterior Actions&quot; tab)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1020"/>
        <source>Warning: &quot;Check complementary&quot; requires left and right primers enabled (&quot;Main&quot; page).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1022"/>
        <source>Warning: &quot;Check complementary&quot; requires any task but &quot;pick_primer_list&quot; (&quot;Main&quot; page).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1038"/>
        <source>CSV report</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1040"/>
        <source>Save CSV report to...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1143"/>
        <location filename="../src/Primer3Dialog.cpp" line="1152"/>
        <location filename="../src/Primer3Dialog.cpp" line="1161"/>
        <location filename="../src/Primer3Dialog.cpp" line="1173"/>
        <location filename="../src/Primer3Dialog.cpp" line="1226"/>
        <location filename="../src/Primer3Dialog.cpp" line="1232"/>
        <location filename="../src/Primer3Dialog.cpp" line="1267"/>
        <source>Can&apos;t parse &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1162"/>
        <source>Incorrect value for &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1245"/>
        <source>PRIMER_MISPRIMING_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1258"/>
        <source>PRIMER_INTERNAL_MISHYB_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1262"/>
        <source>PRIMER_MIN_THREE_PRIME_DISTANCE is unused in the UGENE GUI interface. We may either skip it or set PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE and PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE to %1. Do you want to set?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Start Codon Position</source>
        <translation type="vanished">Codon Pozisyonunu Başlat</translation>
    </message>
    <message>
        <source>Product Min Tm</source>
        <translation type="vanished">Ürün Min Tm</translation>
    </message>
    <message>
        <source>Product Opt Tm</source>
        <translation type="vanished">Ürün Seçimi Tm</translation>
    </message>
    <message>
        <source>Product Max Tm</source>
        <translation type="vanished">Ürün Max Tm</translation>
    </message>
    <message>
        <source>Opt GC%</source>
        <translation type="vanished">Opt GC%</translation>
    </message>
    <message>
        <source>Inside Penalty</source>
        <translation type="vanished">İç Ceza</translation>
    </message>
    <message>
        <source>Internal Oligo Opt Tm</source>
        <translation type="vanished">Dahili Oligo Opt Tm</translation>
    </message>
    <message>
        <source>Excluded Regions</source>
        <translation type="vanished">Hariç Tutulan Bölgeler</translation>
    </message>
    <message>
        <source>Internal Oligo Excluded Regions</source>
        <translation type="vanished">Dahili Oligo Hariç Tutulan Bölgeler</translation>
    </message>
    <message>
        <source>Targets</source>
        <translation type="vanished">Hedefler</translation>
    </message>
    <message>
        <source>Included region is too small for current product size ranges</source>
        <translation type="vanished">Dahil edilen bölge, mevcut ürün boyutu aralıkları için çok küçük</translation>
    </message>
    <message>
        <source>Product Size Ranges</source>
        <translation type="vanished">Ürün Boyut Aralıkları</translation>
    </message>
    <message>
        <source>Sequence Quality</source>
        <translation type="vanished">Sıra Kalitesi</translation>
    </message>
    <message>
        <source>Sequence quality list length must be equal to the sequence length</source>
        <translation type="vanished">Sıra kalitesi liste uzunluğu, sıra uzunluğuna eşit olmalıdır</translation>
    </message>
    <message>
        <source>Can&apos;t pick hyb oligo and only one primer</source>
        <translation type="vanished">Hyb oligo ve sadece bir primer seçemiyorum</translation>
    </message>
    <message>
        <source>Nothing to pick</source>
        <translation type="vanished">Seçecek bir şey yok</translation>
    </message>
    <message>
        <source>The field &apos;%1&apos; has invalid value</source>
        <translation type="vanished">&apos;%1&apos; alanı geçersiz değere sahip</translation>
    </message>
    <message>
        <source>Cannot create an annotation object. Please check settings.</source>
        <translation type="vanished">Ek açıklama nesnesi oluşturulamaz. Lütfen ayarları kontrol edin.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="919"/>
        <source>Save primer settings</source>
        <translation>Astar ayarlarını kaydedin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="930"/>
        <source>Text files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="931"/>
        <source>Load settings</source>
        <translation>Yükleme Ayarları</translation>
    </message>
    <message>
        <source>Can not load settings file: invalid format.</source>
        <translation type="vanished">Ayarlar dosyası yüklenemiyor: geçersiz format.</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Plugin</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="60"/>
        <source>Primer3</source>
        <translation>Primer3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="60"/>
        <source>Integrated tool for PCR primers design.</source>
        <translation>PCR primerleri tasarımı için entegre araç.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="65"/>
        <source>Primer3 (no target sequence)...</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::Primer3Task</name>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="44"/>
        <source>Pick primers task</source>
        <translation>Astar görevi seçin</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="57"/>
        <source>Incorrect sum &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="131"/>
        <source>Primer3 failed during execution. Please, fix all possible errors in the dialog before run.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="176"/>
        <source>Global Primer3 error: &quot;%1&quot;. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="180"/>
        <source>Sequence Primer3 error: &quot;%1&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::Primer3ToAnnotationsTask</name>
    <message>
        <source>Search primers to annotations</source>
        <translation type="vanished">Ek açıklamalara giden primerleri ara</translation>
    </message>
    <message>
        <source>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</source>
        <translation type="vanished">%1. dizisiyle ilişkili herhangi bir ekson ek açıklaması bulunamadı Sağlanan dizinin cDNA olduğundan ve ek açıklamalı eksonik yapıya sahip olduğundan emin olun</translation>
    </message>
    <message>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation type="vanished">Seçilen aralıktaki [%1,%2] ilk ekson, ekson sayısından (%2) daha büyük. Lütfen doğru ekson aralığını ayarlayın.</translation>
    </message>
    <message>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation type="vanished">Seçilen ekson aralığı [%1,%2] ekson sayısından (%2) daha büyük. Lütfen doğru ekson aralığını ayarlayın.</translation>
    </message>
</context>
<context>
    <name>U2::Primer3TopLevelTask</name>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="61"/>
        <source>Find primers with target sequence task</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="76"/>
        <source>Find primers without target sequence task</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="138"/>
        <source>The task was canceled by the user</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="150"/>
        <source>High any compl</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="151"/>
        <source>High 3&apos; compl</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="152"/>
        <source>High hairpins</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="153"/>
        <source>Considered</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="154"/>
        <source>Too many Ns</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="155"/>
        <source>In target</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="156"/>
        <source>In exclude</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="157"/>
        <source>Not OK reg</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="158"/>
        <source>Bad GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="159"/>
        <source>No GC clamp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="160"/>
        <source>Tm too low</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="161"/>
        <source>Tm too high</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="165"/>
        <source>Poly X</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="166"/>
        <source>High end stab</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="167"/>
        <source>OK</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="171"/>
        <source>th</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="218"/>
        <source>Left</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="221"/>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="230"/>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="239"/>
        <source>&lt;th&gt; %1 &lt;/th&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="227"/>
        <source>Right</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="236"/>
        <source>Internal</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="245"/>
        <source>Pair stats</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="247"/>
        <source>considered %1, unacceptable product size %2, high end compl %3, ok %4.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="269"/>
        <source>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</source>
        <translation type="unfinished">%1. dizisiyle ilişkili herhangi bir ekson ek açıklaması bulunamadı Sağlanan dizinin cDNA olduğundan ve ek açıklamalı eksonik yapıya sahip olduğundan emin olun</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="281"/>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation type="unfinished">Seçilen aralıktaki [%1,%2] ilk ekson, ekson sayısından (%2) daha büyük. Lütfen doğru ekson aralığını ayarlayın. {1,%2]?} {3)?}</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="290"/>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation type="unfinished">Seçilen ekson aralığı [%1,%2] ekson sayısından (%2) daha büyük. Lütfen doğru ekson aralığını ayarlayın. {1,%2]?} {3)?}</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="336"/>
        <source>No primers has been found due to the parameters you&apos;ve set up</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="338"/>
        <source>All found primers has been filtered due to the &quot;Check complement&quot; parameters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="350"/>
        <source>Object with annotations was removed</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::ProcessPrimer3ResultsToAnnotationsTask</name>
    <message>
        <location filename="../src/task/ProcessPrimer3ResultsToAnnotationsTask.cpp" line="50"/>
        <source>Search primers to annotations</source>
        <translation type="unfinished">Ek açıklamalara giden primerleri ara</translation>
    </message>
</context>
<context>
    <name>U2::QDPrimerActor</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="83"/>
        <source>%1 invalid input. Excluded regions.</source>
        <translation>%1 geçersiz girdi. Hariç tutulan bölgeler.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="92"/>
        <source>%1 invalid input. Targets.</source>
        <translation>%1 geçersiz girdi. Hedefler.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="101"/>
        <source>%1 invalid input. Product size ranges.</source>
        <translation>%1 geçersiz girdi. Ürün boyutu aralıkları.</translation>
    </message>
</context>
</TS>
