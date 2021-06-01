<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="33"/>
        <source>Main</source>
        <translation>Ana</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="48"/>
        <source>Targets</source>
        <translation>Hedefler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="55"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;start,length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of the excluded region, and length is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligolar bu etikette belirtilen herhangi bir bölgeyle çakışamaz. İlişkili değer,&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt; başlangıç, uzunluk &lt;/span&gt;&lt;/p&gt;&lt;p&gt;çiftlerinin boşlukla ayrılmış bir listesi olmalıdır. Hariç tutulan bölgenin ilk tabanının indeksi ve uzunluk onun uzunluğudur. Bu etiket, düşük sıra kalitesindeki bölgeleri hariç tutmak veya ALU&apos;lar veya LINE&apos;lar gibi tekrar eden öğeleri içeren bölgeleri hariç tutmak gibi görevler için kullanışlıdır. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="62"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;start,length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of a Target, and length is its length.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir veya daha fazla Hedef belirtilirse, en az birinin yanında yasal bir primer çifti bulunmalıdır. Bir Hedef, basit bir dizi tekrar sahası (örneğin, bir CA tekrarı) veya bir tek-baz-çift polimorfizmi olabilir. Değer, başlangıç dizini olan &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:-600;&quot;&gt;başlangıç, uzunluk &lt;/span&gt;&lt;/p&gt;&lt;p&gt; çiftlerinin boşlukla ayrılmış bir listesi olmalıdır Hedefin ilk tabanının uzunluğu ve uzunluğu onun uzunluğudur. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="113"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of product size ranges, for example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 first tries to pick primers in the first range. If that is not possible, it goes to the next range and tries again. It continues in this way until it has either picked all necessary primers or until there are no more ranges. For technical reasons this option makes much lighter computational demands than the Product Size option.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Ürün beden aralıklarının bir listesi, örneğin:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 ilk olarak ilk aralıktaki primerleri seçmeye çalışır. Bu mümkün değilse, bir sonraki aralığa gider ve tekrar dener. Bu şekilde, gerekli tüm primerleri toplayana veya daha fazla aralık kalmayana kadar devam eder. Teknik nedenlerden dolayı bu seçenek, Ürün Boyutu seçeneğinden çok daha hafif hesaplama talepleri sağlar.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="130"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This selection indicates what mispriming library (if any) Primer3 should use to screen for interspersed repeats or for other sequence to avoid as a location for primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu seçim, Primer3&apos;ün serpiştirilmiş tekrarları veya primerler için bir konum olarak kaçınılması gereken diğer dizileri taramak için hangi yanlış priming kitaplığının (varsa) kullanması gerektiğini belirtir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="235"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of primer pairs to return.&lt;/p&gt;&lt;p&gt;Primer pairs returned are sorted by their &amp;quot;quality&amp;quot;, in other words by the value of the objective function (where a lower number indicates a better primer pair).&lt;/p&gt;&lt;p&gt;Caution: setting this parameter to a large value will increase running time.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Döndürülecek maksimum primer çifti sayısı.&lt;/p&gt;&lt;p&gt;Döndürülen primer çiftleri, kendilerine göre sıralanır &amp;quot;kalite&amp;quot;,başka bir deyişle, amaç fonksiyonunun değeri ile (burada daha düşük bir sayı, daha iyi bir primer çiftini gösterir).&lt;/p&gt;&lt;p&gt;Dikkat: Bu parametrenin büyük bir değere ayarlanması çalışma süresini artıracaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="254"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum stability for the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;p&gt;Bigger numbers mean more stable 3&apos; ends. The value is the maximum delta G (kcal/mol) for duplex disruption for the five 3&apos; bases as calculated using the Nearest-Neighbor parameter values specified by the option of &apos;Table of thermodynamic parameters&apos;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sol veya sağ primerin son beş 3 &apos;tabanı için maksimum kararlılık. &lt;/p&gt;&lt;p&gt; Daha büyük sayılar daha kararlı 3&apos; uçlar anlamına gelir. Değer, &apos;Termodinamik parametreler tablosu&apos; seçeneği ile belirtilen En Yakın Komşu parametre değerleri kullanılarak hesaplanan beş 3 &apos;baz için çift yönlü bozulma için maksimum delta G&apos;dir (kcal / mol). &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="270"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed weighted similarity with any sequence in Mispriming Library.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Yanlış Eşleme Kitaplığındaki herhangi bir sıra ile izin verilen maksimum ağırlıklı benzerlik.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="286"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed sum of similarities of a primer pair (one similarity for each primer) with any single sequence in Mispriming Library. &lt;/p&gt;&lt;p&gt;Library sequence weights are not used in computing the sum of similarities.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primer çiftinin (her primer için bir benzerlik), Mispriming Kitaplığındaki herhangi bir tek sıra ile izin verilen maksimum benzerlik toplamı. &lt;/p&gt;&lt;p&gt;Benzerliklerin toplamının hesaplanmasında kütüphane sıra ağırlıkları kullanılmaz.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="302"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the sequence from which you are designing the primers.&lt;/p&gt;&lt;p&gt;The scoring system is the same as used for Max Mispriming, except that an ambiguity code is never treated as a consensus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Astarları tasarladığınız sıradaki ektopik sitelere izin verilen maksimum benzerlik..&lt;/p&gt;&lt;p&gt;Puanlama sistemi, bir belirsizlik kodunun hiçbir zaman bir fikir birliği olarak değerlendirilmemesi dışında, Max Mispriming için kullanılanla aynıdır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="318"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the sequence from which you are designing the primers.&lt;/p&gt;&lt;p&gt;The scoring system is the same as used for Max Mispriming, except that an ambiguity code is never treated as a consensus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Astarları tasarladığınız sıradaki her iki primerin ektopik bölgelere izin verilen maksimum toplam benzerliği.&lt;/p&gt;&lt;p&gt;Puanlama sistemi, bir belirsizlik kodunun hiçbir zaman bir fikir birliği olarak değerlendirilmemesi dışında, Max Mispriming için kullanılanla aynıdır..&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="335"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Index of the first base of a start codon.&lt;/p&gt;&lt;p&gt;This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter. Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir başlangıç kodonunun ilk tabanının dizini.&lt;/p&gt;&lt;p&gt;Bu parametre, Primer3&apos;ün çerçeve içi amplikonlar, ör. bir füzyon proteini için bir şablon oluşturmak için. Primer3, ideal olarak başlangıç kodonunun solunda veya solunda veya gerekirse sağında başlayarak bir çerçeve içi sol primer seçmeye çalışacaktır. Bu parametrenin negatif değerleri, gerçek başlangıç kodonu mevcut dizinin solundaysa yasaldır. Bu parametre negatif değilse, Primer3 bu parametre ile belirtilen konumdaki kodon bir ATG değilse bir hata sinyali verir. -10 ^ 6&apos;dan küçük veya ona eşit bir değer, Primer3&apos;ün bu parametreyi göz ardı etmesi gerektiğini gösterir. Primer3, bir durdurma kodonu için sol primerden sağa tarayarak sağ primerin konumunu seçer. İdeal olarak, doğru primer durdurma kodonunda veya sonrasında sona erecektir.&lt;/p&gt;&lt;p&gt;Bu parametre DENEYSEL olarak düşünülmelidir. Lütfen çıktıyı dikkatlice kontrol edin; bazı hatalı girişler Primer3&apos;te hataya neden olabilir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="395"/>
        <source>Pick left primer</source>
        <translation>Sol astarı seçin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="408"/>
        <source>or use left primer below</source>
        <translation>veya aşağıdaki sol astarı kullanın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="501"/>
        <source>General Settings</source>
        <translation>Genel Ayarlar</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="504"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers such settings as primer size, temperature, GC and other general settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölüm astar boyutu, sıcaklık, GC ve diğer genel ayarlar gibi ayarları kapsar.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="519"/>
        <location filename="../src/Primer3Dialog.ui" line="547"/>
        <location filename="../src/Primer3Dialog.ui" line="575"/>
        <location filename="../src/Primer3Dialog.ui" line="603"/>
        <source>Min</source>
        <translation>Asg</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="526"/>
        <location filename="../src/Primer3Dialog.ui" line="554"/>
        <location filename="../src/Primer3Dialog.ui" line="582"/>
        <location filename="../src/Primer3Dialog.ui" line="610"/>
        <source>Opt</source>
        <translation>Ter</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="533"/>
        <location filename="../src/Primer3Dialog.ui" line="561"/>
        <location filename="../src/Primer3Dialog.ui" line="589"/>
        <location filename="../src/Primer3Dialog.ui" line="617"/>
        <source>Max</source>
        <translation>Azm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="540"/>
        <source>Primer Tm</source>
        <translation>Astar Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="734"/>
        <location filename="../src/Primer3Dialog.ui" line="747"/>
        <location filename="../src/Primer3Dialog.ui" line="760"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum lengths (in bases) of a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick primers shorter than Min or longer than Max, and with default arguments will attempt to pick primers close with size close to Opt.&lt;/p&gt;&lt;p&gt;Min cannot be smaller than 1. Max cannot be larger than 36. (This limit is governed by maximum oligo size for which melting-temperature calculations are valid.)&lt;/p&gt;&lt;p&gt;Min cannot be greater than Max.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primer oligo&apos;nun Minimum, Optimum ve Maksimum uzunlukları (baz olarak).&lt;/p&gt;&lt;p&gt;Primer3, Min&apos;den daha kısa veya Max&apos;ten daha uzun primerleri seçmeyecek ve varsayılan bağımsız değişkenlerle, Opt. Boyutuna yakın olan primerleri seçmeye çalışacaktır.&lt;/p&gt;&lt;p&gt;Min, 1&apos;den küçük olamaz. Maks 36&apos;dan büyük olamaz. (Bu sınır, erime sıcaklığı hesaplamalarının geçerli olduğu maksimum oligo boyutuna tabidir.)&lt;/p&gt;&lt;p&gt;Min, Max&apos;ten büyük olamaz.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="773"/>
        <location filename="../src/Primer3Dialog.ui" line="789"/>
        <location filename="../src/Primer3Dialog.ui" line="805"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum melting temperatures (Celsius) for a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick oligos with temperatures smaller than Min or larger than Max, and with default conditions will try to pick primers with melting temperatures close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir astar oligo için Minimum, Optimum ve Maksimum erime sıcaklıkları (Santigrat).&lt;/p&gt;&lt;p&gt;Primer3, Min&apos;den küçük veya Maks&apos;dan daha büyük sıcaklıklara sahip oligoları seçmeyecek ve varsayılan koşullarda, Opt. Değerine yakın erime sıcaklıklarına sahip primerleri seçmeye çalışacaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="821"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Herhangi bir primer veya oligo&apos;da Minimum, Optimum ve Maksimum G ve C yüzdesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="838"/>
        <location filename="../src/Primer3Dialog.ui" line="869"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Herhangi bir primer veya oligo&apos;da Minimum, Optimum ve Maksimum G ve C yüzdesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="848"/>
        <location filename="../src/Primer3Dialog.ui" line="855"/>
        <location filename="../src/Primer3Dialog.ui" line="862"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum, optimum, and maximum melting temperature of the amplicon.&lt;/p&gt;&lt;p&gt;Primer3 will not pick a product with melting temperature less than min or greater than max. If Opt is supplied and the Penalty Weights for Product Size are non-0 Primer3 will attempt to pick an amplicon with melting temperature close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Amplikonun minimum, optimum ve maksimum erime sıcaklığı.&lt;/p&gt;&lt;p&gt;Primer3, erime sıcaklığı minimumdan düşük veya maks. Seçenek sağlanırsa ve Ürün Boyutu için Ceza Ağırlıkları 0 değilse, Primer3 Opt. 3&apos;e yakın erime sıcaklığına sahip bir amplikon seçmeye çalışacaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="967"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum acceptable (unsigned) difference between the melting temperatures of the left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Sol ve sağ primerlerin erime sıcaklıkları arasındaki maksimum kabul edilebilir (işaretsiz) fark.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="977"/>
        <source>Table of thermodynamic parameters</source>
        <translation>Termodinamik parametreler tablosu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2013"/>
        <source>Primers</source>
        <translation>Astarlar</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2431"/>
        <source>Primer Pairs</source>
        <translation>Astar Çiftleri</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2750"/>
        <source>Hyb Oligos</source>
        <translation>Hyb Oligos</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3402"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section allows to set output annotations&apos; settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölüm, çıktı açıklamalarının ayarlarının yapılmasına izin verir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="996"/>
        <source>Breslauer et. al 1986</source>
        <translation>Breslauer et. al 1986</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="991"/>
        <location filename="../src/Primer3Dialog.ui" line="1099"/>
        <source>SantaLucia 1998</source>
        <translation>SantaLucia 1998</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="20"/>
        <source>Primer Designer</source>
        <translation>Astar Tasarımcısı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="41"/>
        <source>Excluded regions</source>
        <translation>Hariç tutulan bölgeler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="106"/>
        <source>Product size ranges</source>
        <translation>Ürün boyutu aralıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="120"/>
        <source>Mispriming/Repeat library</source>
        <translation>Hatalı priming / Tekrar kitaplığı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="148"/>
        <source>Number to return</source>
        <translation>Döndürülecek numara</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="155"/>
        <source>Max repeat mispriming</source>
        <translation>Max yanlış priming işlemini tekrarlayın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="162"/>
        <source>Max template mispriming</source>
        <translation>Maks şablon yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="169"/>
        <source>Max 3&apos; stability</source>
        <translation>Max 3 &apos;kararlılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="176"/>
        <source>Pair max repeat mispriming</source>
        <translation>Çift maks. Yanlış priming tekrarı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="183"/>
        <source>Pair max template mispriming</source>
        <translation>Çift maks. Şablonda yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="328"/>
        <source>Start codon position</source>
        <translation>Kodon konumunu başlat</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="425"/>
        <source>Pick hybridization probe (internal oligo)</source>
        <translation>Hibridizasyon probu seçin (dahili oligo)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="438"/>
        <source>or use oligo below</source>
        <translation>veya aşağıdaki oligo kullanın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="458"/>
        <source>Pick right primer</source>
        <translation>Doğru astarı seçin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="471"/>
        <source>or use right primer below (5&apos; to 3&apos; on opposite strand)</source>
        <translation>veya aşağıdaki sağ astarı kullanın (zıt iplikçikte 5 &apos;ila 3&apos;)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="512"/>
        <source>Primer size</source>
        <translation>Astar boyutu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="568"/>
        <location filename="../src/Primer3Dialog.ui" line="2447"/>
        <source>Product Tm</source>
        <translation>Ürün Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="596"/>
        <source>Primer GC%</source>
        <translation>Astar % GC</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="954"/>
        <source>Max Tm difference</source>
        <translation>Max Tm farkı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1015"/>
        <source>Max self complementarity</source>
        <translation>Maksimum kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1022"/>
        <source>Max 3&apos; self complementarity</source>
        <translation>Maksimum 3 &apos;kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1029"/>
        <source>Inside target penalty</source>
        <translation>Hedef içi ceza</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1036"/>
        <source>Outside target penalty</source>
        <translation>Hedef dışı ceza</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1057"/>
        <source>Max poly-X</source>
        <translation>Maksimum poli-X</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1064"/>
        <source>First base index</source>
        <translation>İlk temel dizin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1071"/>
        <source>CG clamp</source>
        <translation>CG kelepçesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1095"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the salt correction formula for the melting temperature calculation.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Erime sıcaklığı hesaplaması için tuz düzeltme formülünü belirtir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1124"/>
        <source>Annealing oligo concentration</source>
        <translation>Tavlama oligo konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1433"/>
        <source> Liberal base</source>
        <translation> Serbest temel</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1446"/>
        <source>Show debugging info</source>
        <translation>Hata ayıklama bilgilerini göster</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1484"/>
        <source>Hyb oligo excluded region</source>
        <translation>Hyb oligo hariç bölge</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1509"/>
        <location filename="../src/Primer3Dialog.ui" line="2766"/>
        <source>Hyb oligo size</source>
        <translation>Hyb oligo boyutu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1516"/>
        <location filename="../src/Primer3Dialog.ui" line="2759"/>
        <source>Hyb oligo Tm</source>
        <translation>Hyb oligo Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1523"/>
        <location filename="../src/Primer3Dialog.ui" line="2773"/>
        <source>Hyb oligo GC%</source>
        <translation>Hyb oligo GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1557"/>
        <location filename="../src/Primer3Dialog.ui" line="1626"/>
        <location filename="../src/Primer3Dialog.ui" line="1686"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Size for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için Primer Boyutunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1573"/>
        <location filename="../src/Primer3Dialog.ui" line="1642"/>
        <location filename="../src/Primer3Dialog.ui" line="1699"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Tm for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için Primer Tm&apos;nin eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1589"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer% GC&apos;nin eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1652"/>
        <location filename="../src/Primer3Dialog.ui" line="1712"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için Primer GC% eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1730"/>
        <location filename="../src/Primer3Dialog.ui" line="2929"/>
        <source>Hyb oligo self complementarity</source>
        <translation>Hyb oligo kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1744"/>
        <source>Hyb oligo mishyb library</source>
        <translation>Hyb oligo mishyb kütüphanesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1751"/>
        <source>Hyb oligo min sequence quality</source>
        <translation>Hyb oligo min sekans kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1758"/>
        <source>Hyb oligo conc of monovalent cations</source>
        <translation>Tek değerlikli katyonların Hyb oligo konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1765"/>
        <source>Hyb oligo conc of divalent cations</source>
        <translation>İki değerlikli katyonların Hyb oligo konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1865"/>
        <source>Hyb oligo max 3&apos; self complementarity</source>
        <translation>Hyb oligo max 3 &apos;kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1872"/>
        <source>Hyb oligo max poly-X</source>
        <translation>Hyb oligo max poli-X</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1879"/>
        <source>Hyb oligo max mishyb</source>
        <translation>Hyb oligo max mishyb</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1934"/>
        <source>Hyb oligo [dNTP]</source>
        <translation>Hyb oligo [dNTP]</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1941"/>
        <source>Hyb oligo DNA concentration</source>
        <translation>Hyb oligo DNA konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2192"/>
        <source>Self complementarity</source>
        <translation>Kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2199"/>
        <source>3&apos; self complementarity</source>
        <translation>3 &apos;kendini tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3039"/>
        <source>RT-PCR</source>
        <translation>RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3051"/>
        <source>Design primers for RT-PCR analysis</source>
        <translation>RT-PCR analizi için tasarım primerleri</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3071"/>
        <source>Exon annotation name:</source>
        <translation>Ekson ek açıklama adı:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3081"/>
        <source>exon</source>
        <translation>ekson</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3106"/>
        <source>Minimum exon junction overlap size</source>
        <translation>Minimum ekson bağlantı üst üste binme boyutu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3170"/>
        <source>Exon range:</source>
        <translation>Ekson aralığı:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3217"/>
        <source>Max number of pairs to query:</source>
        <translation>Sorgulanacak maksimum çift sayısı:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3304"/>
        <source>Min sequence quality:</source>
        <translation>Minimum sıra kalitesi:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3327"/>
        <source>Min 3&apos; sequence quality:</source>
        <translation>Min 3 &apos;sekans kalitesi:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3350"/>
        <source>Sequence quality range min:</source>
        <translation>Sıra kalite aralığı min:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3373"/>
        <source>Sequence quality range max:</source>
        <translation>Sıra kalite aralığı maks:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3457"/>
        <source>Help</source>
        <translation>Yardım</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3477"/>
        <source>Save settings</source>
        <translation>Ayarları kaydet</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3484"/>
        <source>Load settings</source>
        <translation>Yükleme Ayarları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3491"/>
        <source>Reset form</source>
        <translation>Formu Sıfırla</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3498"/>
        <source>Pick primers</source>
        <translation>Astarları seçin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1043"/>
        <source>Concentration of monovalent cations</source>
        <translation>Tek değerlikli katyonların konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="987"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies details of melting temperature calculation.&lt;/p&gt;&lt;p&gt;First method uses the table of thermodynamic parameters from the paper [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83:4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Second method (recommended) was suggested in the paper [SantaLucia JR (1998), Proc Natl Acad Sci 95:1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Erime sıcaklığı hesaplamasının ayrıntılarını belirtir.&lt;/p&gt;&lt;p&gt;İlk yöntem, kağıttaki termodinamik parametreler tablosunu kullanır [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83: 4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Makalede ikinci yöntem (önerilen) önerilmiştir [SantaLucia JR (1998), Proc Natl Acad Sci 95: 1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1050"/>
        <source>Max #N&apos;s</source>
        <translation>Max #N&apos;s</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1078"/>
        <source>Concentration of divalent cations</source>
        <translation>İki değerlikli katyonların konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1085"/>
        <source>Salt correction formula</source>
        <translation>Tuz düzeltme formülü</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1104"/>
        <source>Schildkraut and Lifson 1965</source>
        <translation>Schildkraut ve Lifson 1965</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1109"/>
        <source>Owczarzy et. 2004</source>
        <translation>Owczarzy et. 2004</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1117"/>
        <source>Concentration of dNTPs</source>
        <translation>DNTP&apos;lerin konsantrasyonu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1254"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity, and the maximum allowable 3&apos;-anchored global alignment score when testing for complementarity between left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Kendini tamamlayıcılık için tek bir primer test edilirken izin verilen maksimum 3&apos;-bağlantılı global hizalama skoru ve sol ve sağ primerler arasında tamamlayıcılık için test edilirken maksimum izin verilebilir 3&apos;-bağlantılı global hizalama skoru.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1270"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable local alignment score when testing a single primer for (local) self-complementarity and the maximum allowable local alignment score when testing for complementarity between left and right primers.&lt;/p&gt;&lt;p&gt;Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;(Yerel) kendi kendini tamamlayıcılık için tek bir primer test edilirken izin verilen maksimum yerel hizalama puanı ve sol ve sağ primerler arasında tamamlayıcılık için test edilirken izin verilen maksimum yerel hizalama puanı.&lt;/p&gt;&lt;p&gt;PCR&apos;de zorunlu olarak kendinden hazırlamaya neden olmadan primerlerin birbirine tavlanma eğilimini tahmin etmek için yerel kendi kendini tamamlayıcılık alınır. Puanlama sistemi, tamamlayıcı bazlar için 1.00, herhangi bir taban (veya N) ile bir N ile eşleşme için -0.25, uyumsuzluk için -1.00 ve bir boşluk için -2.00 verir. Yalnızca tek baz çifti boşluklarına izin verilir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1286"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum number of unknown bases (N) allowable in any primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Herhangi bir astarda izin verilen maksimum bilinmeyen baz sayısı (N).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1302"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable length of a mononucleotide repeat, for example AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir mononükleotid tekrarının izin verilen maksimum uzunluğu, örneğin AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1318"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Varsayılan olmayan değerler yalnızca 0 veya 1 hedef bölgeli diziler için geçerlidir.&lt;/p&gt;&lt;p&gt;Primer, bir hedefi kapsayan ve hedefle örtüşmeyen bir çiftin parçasıysa, bu değeri 3 &apos;ucundan (benzersiz) hedefe kadar olan nükleotid konumlarının sayısıyla çarparak&apos; pozisyon cezası &apos;elde edin. Bu parametrenin etkisi, Primer3&apos;ün hedefe yakınlığı amaç fonksiyonunda bir terim olarak dahil etmesine izin vermektir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1334"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter is the index of the first base in the input sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu parametre, giriş dizisindeki ilk tabanın dizinidir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1350"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Require the specified number of consecutive Gs and Cs at the 3&apos; end of both the left and right primer.&lt;/p&gt;&lt;p&gt;This parameter has no effect on the internal oligo if one is requested.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Hem sol hem de sağ primerin 3 &apos;ucunda belirtilen sayıda ardışık G ve C gerektir.&lt;/p&gt;&lt;p&gt;Bu parametrenin, istenmesi durumunda dahili oligo üzerinde hiçbir etkisi yoktur.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1366"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar (mM) concentration of monovalent salt cations (usually KCl) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to calculate oligo and primer melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;PCR&apos;de monovalent tuz katyonlarının (genellikle KCl) milimolar (mM) konsantrasyonu.&lt;/p&gt;&lt;p&gt;Primer3 bu argümanı oligo ve primer erime sıcaklıklarını hesaplamak için kullanır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1382"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of divalent salt cations (usually MgCl^(2+)) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 converts concentration of divalent cations to concentration of monovalent cations using following formula:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Monovalent cations] = [Monovalent cations] + 120*(([divalent cations] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In addition, if the specified concentration of dNTPs is larger than the concentration of divalent cations then the effect of the divalent cations is not considered.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;PCR&apos;deki iki değerlikli tuz katyonlarının (genellikle MgCl ^ (2+)) milimolar konsantrasyonu.&lt;/p&gt;&lt;p&gt;Primer3, aşağıdaki formülü kullanarak iki değerlikli katyonların konsantrasyonunu tek değerlikli katyonların konsantrasyonuna dönüştürür:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Tek değerlikli katyonlar] = [Tek değerlikli katyonlar] + 120 * (([iki değerlikli katyonlar] - [dNTP]) ^ 0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Ek olarak, belirtilen dNTP konsantrasyonu iki değerlikli katyonların konsantrasyonundan daha büyükse, iki değerlikli katyonların etkisi dikkate alınmaz.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1395"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of the sum of all deoxyribonucleotide triphosphates.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tüm deoksiribonükleotid trifosfatların toplamının milimolar konsantrasyonu.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1408"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A value to use as nanomolar (nM) concentration of each annealing oligo over the course the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to esimate oligo melting temperatures.&lt;/p&gt;&lt;p&gt;The default (50nM) works well with the standard protocol used at the Whitehead/MIT Center for Genome Research --0.5 microliters of 20 micromolar concentration for each primer in a 20 microliter reaction with 10 nanograms template, 0.025 units/microliter Taq polymerase in 0.1 mM each dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) using 35 cycles with an annealing temperature of 56 degrees Celsius.&lt;/p&gt;&lt;p&gt;The value of this parameter is less than the actual concentration of oligos in the initial reaction mix because  it is the concentration of annealing oligos, which in turn depends on the amount of template (including PCR product) in a given cycle. This concentration increases a great deal during a PCR; fortunately PCR seems quite robust for a variety of oligo melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;PCR boyunca her tavlama oligo&apos;nun nanomolar (nM) konsantrasyonu olarak kullanılacak bir değer&lt;/p&gt;&lt;p&gt;Primer3 bu argümanı oligo erime sıcaklıklarını tahmin etmek için kullanır.&lt;/p&gt;&lt;p&gt;Varsayılan (50nM), Whitehead / MIT Genom Araştırma Merkezi&apos;nde kullanılan standart protokolle iyi çalışır - 10 nanogram şablonlu 20 mikrolitre reaksiyonda her primer için 20 mikromolar konsantrasyonda 0,5 mikrolitre, 0,1&apos;de 0,025 birim / mikrolitre Taq polimeraz 56 derece Celsius tavlama sıcaklığı ile 35 döngü kullanılarak her bir dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) mM.&lt;/p&gt;&lt;p&gt;Bu parametrenin değeri, başlangıç reaksiyon karışımındaki gerçek oligo konsantrasyonundan daha azdır çünkü  bu, belirli bir döngüdeki şablon miktarına (PCR ürünü dahil) bağlı olan tavlama oligolarının konsantrasyonudur. Bu konsantrasyon, bir PCR sırasında büyük ölçüde artar; Neyse ki PCR, çeşitli oligo eritme sıcaklıkları için oldukça sağlam görünmektedir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1415"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;.&lt;/p&gt;&lt;p&gt;The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Varsayılan olmayan değerler yalnızca 0 veya 1 hedef bölgeli diziler için geçerlidir.&lt;/p&gt;&lt;p&gt;Primer, bir hedefi kapsayan ve hedefle örtüşen bir çiftin parçasıysa, bu değeri, &apos;pozisyon cezasını&apos; elde etmek için primerin (benzersiz) hedefle örtüştüğü nükleotid pozisyonlarının sayısıyla çarpın.&lt;/p&gt;&lt;p&gt;Bu parametrenin etkisi, Primer3&apos;ün amaç işlevinde bir terim olarak hedefle örtüşmeyi içermesine izin vermektir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1453"/>
        <source>Do not treat ambiguity codes in libraries as consensus</source>
        <translation>Kitaplıklardaki belirsizlik kodlarını fikir birliği olarak değerlendirmeyin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1463"/>
        <source>Lowercase masking</source>
        <translation>Küçük harf maskeleme</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1473"/>
        <source>Internal Oligo</source>
        <translation>Dahili Oligo</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1476"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameters governing choice of internal oligos are analogous to the parameters governing choice of primer pairs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligoların seçimini yöneten parametreler, primer çiftlerinin seçimini yöneten parametrelere benzer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1491"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&amp;lt;start&amp;gt;,&amp;lt;length&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &amp;lt;start&amp;gt; is the index of the first base of an excluded region, and &amp;lt;length&amp;gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Ortadaki oligolar, bu etiketle belirtilen herhangi bir bölgeyle çakışamaz. İlişkili değer boşlukla ayrılmış bir liste olmalıdır&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&amp;lt;başlangıç&amp;gt;,&amp;lt;uzunluk&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;çiftler, burada &amp;lt;başlangıç&amp;gt; hariç tutulan bir bölgenin ilk tabanının dizinidir ve &amp;lt;uzunluk&amp;gt; uzunluğu. Genellikle iç oligolar için Hedef bölgeler hariç tutulan bölgeler yapılır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1530"/>
        <location filename="../src/Primer3Dialog.ui" line="1537"/>
        <location filename="../src/Primer3Dialog.ui" line="1544"/>
        <source>Min:</source>
        <translation>Asgari:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1599"/>
        <location filename="../src/Primer3Dialog.ui" line="1606"/>
        <location filename="../src/Primer3Dialog.ui" line="1613"/>
        <source>Opt:</source>
        <translation>Opt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1659"/>
        <location filename="../src/Primer3Dialog.ui" line="1666"/>
        <location filename="../src/Primer3Dialog.ui" line="1673"/>
        <source>Max:</source>
        <translation>Azami:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1737"/>
        <source>Max #Ns</source>
        <translation>Max #Ns</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1778"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max self complemntarity for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer maksimum kendi kendine uyumluluğunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1794"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max #Ns for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer max #Ns&apos;nin eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1810"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to primer mispriming library, except that the event we seek to avoid is hybridization of the internal oligo to sequences in this library rather than priming from them.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer yanlış priming kitaplığına benzer, ancak kaçınmaya çalıştığımız olay, dahili oligo&apos;nun, bunlardan hazırlanmak yerine bu kitaplıktaki dizilere melezleştirilmesidir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1823"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer minimum quality for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer minimum kalite eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1839"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of monovalent cations for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için tek değerlikli katyonların primer konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1855"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of divalent cations for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için iki değerlikli katyonların primer konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1892"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max 3&apos; self complementarity for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer max 3 &apos;kendi kendini tamamlama eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1908"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max poly-X for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer max poly-X&apos;in eşdeğer parametresi&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1924"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to primer max library mispriming.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer max kitaplık yanlış priming&apos;e benzer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1954"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of the dNTPs for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için dNTP&apos;lerin primer konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1970"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer DNA concentration for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Dahili oligo için primer DNA konsantrasyonunun eşdeğer parametresi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1996"/>
        <source>Penalty Weights</source>
        <translation>Ceza Ağırlıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2022"/>
        <source>Tm</source>
        <translation>Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2029"/>
        <source>Size  </source>
        <translation>Boyut  </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2036"/>
        <source>GC%</source>
        <translation>GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2043"/>
        <location filename="../src/Primer3Dialog.ui" line="2050"/>
        <location filename="../src/Primer3Dialog.ui" line="2057"/>
        <location filename="../src/Primer3Dialog.ui" line="2486"/>
        <location filename="../src/Primer3Dialog.ui" line="2493"/>
        <location filename="../src/Primer3Dialog.ui" line="2780"/>
        <location filename="../src/Primer3Dialog.ui" line="2787"/>
        <location filename="../src/Primer3Dialog.ui" line="2794"/>
        <source>Lt:</source>
        <translation>Lt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2112"/>
        <location filename="../src/Primer3Dialog.ui" line="2119"/>
        <location filename="../src/Primer3Dialog.ui" line="2126"/>
        <location filename="../src/Primer3Dialog.ui" line="2500"/>
        <location filename="../src/Primer3Dialog.ui" line="2507"/>
        <location filename="../src/Primer3Dialog.ui" line="2849"/>
        <location filename="../src/Primer3Dialog.ui" line="2856"/>
        <location filename="../src/Primer3Dialog.ui" line="2863"/>
        <source>Gt:</source>
        <translation>Gt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2206"/>
        <source>#N&apos;s</source>
        <translation>#N&apos;s</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2213"/>
        <source>Mispriming</source>
        <translation>Yanlış astarlama</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2220"/>
        <location filename="../src/Primer3Dialog.ui" line="3288"/>
        <source>Sequence quality</source>
        <translation>Sıra kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2227"/>
        <source>End sequence quality</source>
        <translation>Bitiş sırası kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2234"/>
        <source>Position penalty</source>
        <translation>Pozisyon cezası</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2241"/>
        <source>End stability</source>
        <translation>Son kararlılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2248"/>
        <source>Template mispriming</source>
        <translation>Şablon yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2440"/>
        <source>Product size</source>
        <translation>Ürün boyutu</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2557"/>
        <source>Tm difference</source>
        <translation>Tm farkı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2564"/>
        <source>Any complementarity</source>
        <translation>Herhangi bir tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2571"/>
        <source>3&apos; complementarity</source>
        <translation>3 &apos;tamamlayıcılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2578"/>
        <source>Pair mispriming</source>
        <translation>Çift yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2585"/>
        <source>Primer penalty weight</source>
        <translation>Astar ceza ağırlığı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2592"/>
        <source>Hyb oligo penalty weight</source>
        <translation>Hyb oligo ceza ağırlığı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2599"/>
        <source>Primer pair template mispriming weight</source>
        <translation>Astar çifti şablonu yanlış astarlama ağırlığı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2936"/>
        <source>Hyb oligo #N&apos;s</source>
        <translation>Hyb oligo #N&apos;s</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2943"/>
        <source>Hyb oligo mishybing</source>
        <translation>Hyb oligo mishybing</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2950"/>
        <source>Hyb oligo sequence quality</source>
        <translation>Hyb oligo sekans kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3063"/>
        <source>mRNA sequence</source>
        <translation>mRNA dizisi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3118"/>
        <source>At 5&apos; side (bp):</source>
        <translation>5 &apos;tarafında (bp):</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3148"/>
        <source>At 3&apos;side (bp)</source>
        <translation>3&apos;ün yanında (bp)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3205"/>
        <source>Primer product must span at least one intron on the corresponding genomic DNA</source>
        <translation>Primer ürün, karşılık gelen genomik DNA üzerinde en az bir intronu kapsamalıdır</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3279"/>
        <source>Sequence Quality</source>
        <translation>Sıra Kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1999"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölüm açıklar &amp;quot;ceza ağırlıkları&amp;quot;,bu, kullanıcının Primer3&apos;ün seçtiği kriterleri değiştirmesine izin verir.&amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;İki ağırlık sınıfı vardır: bazı parametreler için bir &apos;Lt&apos; (küçüktür) ve bir &apos;Gt&apos; (büyüktür) ağırlık vardır. Bunlar, Primer3&apos;ün, değer belirtilen optimumdan (sırasıyla) küçük veya büyük olduğunda kullandığı ağırlıklardır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2070"/>
        <location filename="../src/Primer3Dialog.ui" line="2086"/>
        <location filename="../src/Primer3Dialog.ui" line="2102"/>
        <location filename="../src/Primer3Dialog.ui" line="2139"/>
        <location filename="../src/Primer3Dialog.ui" line="2155"/>
        <location filename="../src/Primer3Dialog.ui" line="2171"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimal değerden (Lt) küçük veya (Gt) büyük bir primerin karşılık gelen parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2261"/>
        <location filename="../src/Primer3Dialog.ui" line="2277"/>
        <location filename="../src/Primer3Dialog.ui" line="2293"/>
        <location filename="../src/Primer3Dialog.ui" line="2309"/>
        <location filename="../src/Primer3Dialog.ui" line="2325"/>
        <location filename="../src/Primer3Dialog.ui" line="2341"/>
        <location filename="../src/Primer3Dialog.ui" line="2373"/>
        <location filename="../src/Primer3Dialog.ui" line="2389"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Önceden tanımlanmış optimum değerden farklı bir primer parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2357"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Determines the overall weight of the position penalty in calculating the penalty for a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir astar cezasının hesaplanmasında pozisyon cezasının toplam ağırlığını belirler.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2460"/>
        <location filename="../src/Primer3Dialog.ui" line="2476"/>
        <location filename="../src/Primer3Dialog.ui" line="2520"/>
        <location filename="../src/Primer3Dialog.ui" line="2536"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer pair less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimum değerden (Lt) küçük veya (Gt) büyük bir primer çiftinin karşılık gelen parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2612"/>
        <location filename="../src/Primer3Dialog.ui" line="2628"/>
        <location filename="../src/Primer3Dialog.ui" line="2644"/>
        <location filename="../src/Primer3Dialog.ui" line="2660"/>
        <location filename="../src/Primer3Dialog.ui" line="2676"/>
        <location filename="../src/Primer3Dialog.ui" line="2692"/>
        <location filename="../src/Primer3Dialog.ui" line="2708"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer pair parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Önceden tanımlanmış optimum değerden farklı bir primer çifti parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2807"/>
        <location filename="../src/Primer3Dialog.ui" line="2823"/>
        <location filename="../src/Primer3Dialog.ui" line="2839"/>
        <location filename="../src/Primer3Dialog.ui" line="2876"/>
        <location filename="../src/Primer3Dialog.ui" line="2892"/>
        <location filename="../src/Primer3Dialog.ui" line="2908"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a internal oligo less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimal değerden (Lt) daha küçük veya (Gt) daha büyük bir dahili oligo için karşılık gelen parametre için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2963"/>
        <location filename="../src/Primer3Dialog.ui" line="2979"/>
        <location filename="../src/Primer3Dialog.ui" line="2995"/>
        <location filename="../src/Primer3Dialog.ui" line="3011"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for an internal oligo parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Önceden tanımlanmış optimum değerden farklı bir dahili oligo parametresi için ceza ağırlığı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3042"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölümde kullanıcı, primer ürününün ekson-ekson birleşimiyle örtüşmesi veya intron yayılması gerektiğini belirleyebilir. Bu sadece açıklamalı eksonlarla bir cDNA (mRNA) dizisi için primerler tasarlarken geçerlidir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3048"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This feature allows to specify if primer product must span intron-exon boundaries.&lt;/p&gt;&lt;p&gt;Checking this option will result in ignoring &lt;span style=&quot; font-style:italic;&quot;&gt;Excluded&lt;/span&gt; and &lt;span style=&quot; font-style:italic;&quot;&gt;Target Regions&lt;/span&gt; from Main section.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu özellik, primer ürününün intron-ekson sınırlarını kapsaması gerekip gerekmediğini belirlemeye izin verir.&lt;/p&gt;&lt;p&gt;Bu seçeneğin işaretlenmesi, göz ardı edilmesine neden olacaktır. &lt;span style=&quot; font-style:italic;&quot;&gt;Hariç tutuldu&lt;/span&gt; ve &lt;span style=&quot; font-style:italic;&quot;&gt;Hedef Bölgeler&lt;/span&gt; Ana bölümden.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3078"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The name of the annotation which defines the exons in the mRNA sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;MRNA dizisindeki eksonları tanımlayan ek açıklamanın adı.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3183"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Aralık ayarlanmışsa, primer araması seçilen eksonlarla sınırlı olacaktır. Örneğin:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Aralık, gerçek ekson aralığından daha büyükse veya başlangıç ekson sayısı ekson sayısını aşarsa, hata mesajı görüntülenir. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3224"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This number of found primer pairs wil bel queried to check if they fullfill the requirements for RTPCR.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bulunan bu sayıda primer çifti, RTPCR gereksinimlerini karşılayıp karşılamadıklarını kontrol etmek için sorgulanacaktır.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3282"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bu bölümde kullanıcı, hedef sıranın ve ilgili parametrelerin sıra kalitesini belirleyebilir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3295"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of space separated integers. There must be exactly one integer for each base in the Source Sequence if this argument is non-empty. High numbers indicate high confidence in the base call at that position and low numbers indicate low confidence in the base call at that position.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Boşlukla ayrılmış tam sayıların listesi. Bu bağımsız değişken boş değilse, Kaynak Sırasındaki her taban için tam olarak bir tamsayı olmalıdır. Yüksek sayılar, o konumda baz aramasında yüksek güveni gösterir ve düşük sayılar, bu konumdaki baz aramasına düşük güveni gösterir.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3317"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primer içinde izin verilen minimum sıra kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3340"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within the 3&apos; pentamer of a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bir primerin 3 &apos;pentameri içinde izin verilen minimum sekans kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3363"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum yasal sıra kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3386"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maksimum yasal sıralama kalitesi.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3399"/>
        <source>Result Settings</source>
        <translation>Sonuç Ayarları</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="171"/>
        <source>Primer</source>
        <translation>Astar</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="172"/>
        <source>PCR primer design</source>
        <translation>PCR primer tasarımı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="174"/>
        <source>Excluded regions</source>
        <translation>Hariç tutulan bölgeler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="175"/>
        <source>Targets</source>
        <translation>Hedefler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="176"/>
        <source>Product size ranges</source>
        <translation>Ürün boyutu aralıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="177"/>
        <source>Number to return</source>
        <translation>Döndürülecek numara</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="179"/>
        <source>Max repeat mispriming</source>
        <translation>Max yanlış priming işlemini tekrarlayın</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="180"/>
        <source>Max template mispriming</source>
        <translation>Maks şablon yanlış priming</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="181"/>
        <source>Max 3&apos; stability</source>
        <translation>Max 3 &apos;kararlılık</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="182"/>
        <source>Pair max repeat mispriming</source>
        <translation>Çift maks. Yanlış priming tekrarı</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="183"/>
        <source>Pair max template mispriming</source>
        <translation>Çift maks. Şablonda yanlış priming</translation>
    </message>
</context>
<context>
    <name>U2::FindExonRegionsTask</name>
    <message>
        <location filename="../src/FindExonRegionsTask.cpp" line="97"/>
        <source>Failed to search for exon annotations. The sequence %1 doesn&apos;t have any related annotations.</source>
        <translation>Ekson notları aranamadı. %1 dizisinin ilgili ek açıklaması yok.</translation>
    </message>
</context>
<context>
    <name>U2::GTest</name>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="211"/>
        <source>Illegal TARGET value: %1</source>
        <translation>Geçersiz HEDEF değeri: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="230"/>
        <location filename="../src/Primer3Tests.cpp" line="299"/>
        <source>Illegal PRIMER_DEFAULT_PRODUCT value: %1</source>
        <translation>Geçersiz PRIMER_VARSAYILAN_ÜRÜN değeri: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="248"/>
        <source>Illegal PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION value: %1</source>
        <translation>Yasadışı PRIMER_DAHİLİ_OLIGO_HARİÇ_BÖLGE değeri: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="263"/>
        <source>Illegal INCLUDED_REGION value: %1</source>
        <translation>Yasadışı INCLUDED_REGION değeri: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="317"/>
        <source>Illegal EXCLUDED_REGION value: %1</source>
        <translation>Yasadışı EXCLUDED_REGION değeri: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="348"/>
        <source>Unrecognized PRIMER_TASK</source>
        <translation>Tanınmayan PRIMER_GÖREV</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="360"/>
        <source>Contradiction in primer_task definition</source>
        <translation>Birincil görev tanımında çelişki</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="418"/>
        <source>Missing SEQUENCE tag</source>
        <translation>Eksik SEQUENCE etiketi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="425"/>
        <source>Error in sequence quality data</source>
        <translation>Sıra kalitesi verilerinde hata</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="427"/>
        <source>Sequence quality data missing</source>
        <translation>Sıra kalitesi verileri eksik</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="450"/>
        <source>PRIMER_PAIRS_NUMBER is incorrect. Expected:%2, but Actual:%3</source>
        <translation>ASTAR_ÇİFTLERİ_NUMARASI yanlış. Beklenen: %2, ancak Gerçek: %3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="552"/>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:NULL</source>
        <translation>%1 yanlış. Beklenen:%2,%3, ancak Gerçek:BOŞ</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="561"/>
        <source>%1 is incorrect. Expected:NULL, but Actual:%2,%3</source>
        <translation>%1 yanlış. Beklenen: BOŞ, ancak Gerçek:%2,%3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="571"/>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:%4,%5</source>
        <translation>%1 yanlış. Beklenen:%2,%3, ancak Gerçek:%4,%5</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="610"/>
        <location filename="../src/Primer3Tests.cpp" line="620"/>
        <location filename="../src/Primer3Tests.cpp" line="630"/>
        <source>%1 is incorrect. Expected:%2, but Actual:%3</source>
        <translation>%1 yanlış. Beklenen:%2, ancak Gerçekleşen:%3</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ADVContext</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="93"/>
        <source>Primer3...</source>
        <translation>Primer3...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="147"/>
        <source>Error</source>
        <translation>Hata</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="147"/>
        <source>Cannot create an annotation object. Please check settings</source>
        <translation>Ek açıklama nesnesi oluşturulamaz. Lütfen ayarları kontrol edin</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="83"/>
        <source>NONE</source>
        <translation>YOK</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="84"/>
        <source>HUMAN</source>
        <translation>İNSAN</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="85"/>
        <source>RODENT_AND_SIMPLE</source>
        <translation>KÜÇÜK_VE_BASİT</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="86"/>
        <source>RODENT</source>
        <translation>KEMİRGEN</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="87"/>
        <source>DROSOPHILA</source>
        <translation>MEYVE SİNEĞİ</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="439"/>
        <source>Start Codon Position</source>
        <translation>Codon Pozisyonunu Başlat</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="453"/>
        <source>Product Min Tm</source>
        <translation>Ürün Min Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="467"/>
        <source>Product Opt Tm</source>
        <translation>Ürün Seçimi Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="481"/>
        <source>Product Max Tm</source>
        <translation>Ürün Max Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="495"/>
        <source>Opt GC%</source>
        <translation>Opt GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="509"/>
        <source>Inside Penalty</source>
        <translation>İç Ceza</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="523"/>
        <source>Internal Oligo Opt Tm</source>
        <translation>Dahili Oligo Opt Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="535"/>
        <source>Excluded Regions</source>
        <translation>Hariç Tutulan Bölgeler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="547"/>
        <source>Internal Oligo Excluded Regions</source>
        <translation>Dahili Oligo Hariç Tutulan Bölgeler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="559"/>
        <source>Targets</source>
        <translation>Hedefler</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="665"/>
        <source>Included region is too small for current product size ranges</source>
        <translation>Dahil edilen bölge, mevcut ürün boyutu aralıkları için çok küçük</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="672"/>
        <source>Product Size Ranges</source>
        <translation>Ürün Boyut Aralıkları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="596"/>
        <source>Sequence Quality</source>
        <translation>Sıra Kalitesi</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="603"/>
        <source>Sequence quality list length must be equal to the sequence length</source>
        <translation>Sıra kalitesi liste uzunluğu, sıra uzunluğuna eşit olmalıdır</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="625"/>
        <source>Can&apos;t pick hyb oligo and only one primer</source>
        <translation>Hyb oligo ve sadece bir primer seçemiyorum</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="645"/>
        <source>Nothing to pick</source>
        <translation>Seçecek bir şey yok</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="683"/>
        <source>The field &apos;%1&apos; has invalid value</source>
        <translation>&apos;%1&apos; alanı geçersiz değere sahip</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="698"/>
        <source>Cannot create an annotation object. Please check settings.</source>
        <translation>Ek açıklama nesnesi oluşturulamaz. Lütfen ayarları kontrol edin.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="712"/>
        <source>Save primer settings</source>
        <translation>Astar ayarlarını kaydedin</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="794"/>
        <source>Load settings</source>
        <translation>Yükleme Ayarları</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="807"/>
        <source>Can not load settings file: invalid format.</source>
        <translation>Ayarlar dosyası yüklenemiyor: geçersiz format.</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Plugin</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="56"/>
        <source>Primer3</source>
        <translation>Primer3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="56"/>
        <source>Integrated tool for PCR primers design.</source>
        <translation>PCR primerleri tasarımı için entegre araç.</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Task</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="320"/>
        <source>Pick primers task</source>
        <translation>Astar görevi seçin</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ToAnnotationsTask</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="702"/>
        <source>Search primers to annotations</source>
        <translation>Ek açıklamalara giden primerleri ara</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="731"/>
        <source>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</source>
        <translation>%1. dizisiyle ilişkili herhangi bir ekson ek açıklaması bulunamadı Sağlanan dizinin cDNA olduğundan ve ek açıklamalı eksonik yapıya sahip olduğundan emin olun</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="742"/>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation>Seçilen aralıktaki [%1,%2] ilk ekson, ekson sayısından (%2) daha büyük. Lütfen doğru ekson aralığını ayarlayın.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="750"/>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation>Seçilen ekson aralığı [%1,%2] ekson sayısından (%2) daha büyük. Lütfen doğru ekson aralığını ayarlayın.</translation>
    </message>
</context>
<context>
    <name>U2::QDPrimerActor</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="72"/>
        <source>%1 invalid input. Excluded regions.</source>
        <translation>%1 geçersiz girdi. Hariç tutulan bölgeler.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="81"/>
        <source>%1 invalid input. Targets.</source>
        <translation>%1 geçersiz girdi. Hedefler.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="90"/>
        <source>%1 invalid input. Product size ranges.</source>
        <translation>%1 geçersiz girdi. Ürün boyutu aralıkları.</translation>
    </message>
</context>
</TS>
