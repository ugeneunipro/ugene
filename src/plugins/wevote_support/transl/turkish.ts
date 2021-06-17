<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>QObject</name>
    <message>
        <source>WEVOTE external tool support</source>
        <translation>WEVOTE harici araç desteği</translation>
    </message>
    <message>
        <source>The plugin supports WEVOTE (WEighted VOting Taxonomic idEntification) - a tool that implements an algorithm to improve the reads classification. (https://github.com/aametwally/WEVOTE)</source>
        <translation>Eklenti, okuma sınıflandırmasını iyileştirmek için bir algoritma uygulayan bir araç olan WEVOTE&apos;u (WEighted VOting Taxonomic idEntification) destekler. (https://github.com/aametwally/WEVOTE)</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::WevotePrompter</name>
    <message>
        <source>Ensemble classification data, produced by other tools.</source>
        <translation>Diğer araçlar tarafından üretilen toplu sınıflandırma verileri.</translation>
    </message>
    <message>
        <source>Input URL</source>
        <translation>URL girin</translation>
    </message>
    <message>
        <source>Input URL.</source>
        <translation>URL girin.</translation>
    </message>
    <message>
        <source>Input classification CSV file</source>
        <translation>Giriş sınıflandırması CSV dosyası</translation>
    </message>
    <message>
        <source>Input a CSV file in the following format:
1) a sequence name
2) taxID from the first tool
3) taxID from the second tool
4) etc.</source>
        <translation>Aşağıdaki formatta bir CSV dosyası girin:
1) bir sıra adı
2) ilk araçtaki taxID
3) ikinci araçtan taxID
4) vb.</translation>
    </message>
    <message>
        <source>WEVOTE Classification</source>
        <translation>WEVOTE Sınıflandırması</translation>
    </message>
    <message>
        <source>A map of sequence names with the associated taxonomy IDs.</source>
        <translation>İlişkili taksonomi kimlikleriyle birlikte sıra adlarının bir haritası.</translation>
    </message>
    <message>
        <source>Penalty</source>
        <translation>Ceza</translation>
    </message>
    <message>
        <source>Score penalty for disagreements (-k)</source>
        <translation>Anlaşmazlıklar için puan cezası (-k)</translation>
    </message>
    <message>
        <source>Number of agreed tools</source>
        <translation>Kabul edilen araçların sayısı</translation>
    </message>
    <message>
        <source>Specify the minimum number of tools agreed on WEVOTE decision (-a).</source>
        <translation>WEVOTE kararı (-a) üzerinde mutabık kalınan minimum araç sayısını belirtin.</translation>
    </message>
    <message>
        <source>Score threshold</source>
        <translation>Puan eşiği</translation>
    </message>
    <message>
        <source>Score threshold (-s)</source>
        <translation>Puan eşiği (-s)</translation>
    </message>
    <message>
        <source>Number of threads</source>
        <translation>Konu sayısı</translation>
    </message>
    <message>
        <source>Use multiple threads (-n).</source>
        <translation>Birden çok iş parçacığı (-n) kullanın.</translation>
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
        <source>Auto</source>
        <translation>Otomatik</translation>
    </message>
    <message>
        <source>Improve Classification with WEVOTE</source>
        <translation>WEVOTE ile Sınıflandırmayı Geliştirin</translation>
    </message>
    <message>
        <source>WEVOTE (WEighted VOting Taxonomic idEntification) is a metagenome shortgun sequencing DNA reads classifier based on an ensemble of other classification methods (Kraken, CLARK, etc.).</source>
        <translation>WEVOTE (WEVOTE (WEighted VOting Taxonomic idEntification), diğer sınıflandırma yöntemlerinin (Kraken, CLARK, vb.) Bir topluluğuna dayanan bir metagenom kısa tabanca dizileme DNA okuma sınıflandırıcısıdır.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::WevoteWorker</name>
    <message>
        <source>Empty input file URL in the message</source>
        <translation>Mesajdaki giriş dosyası URL&apos;si boş</translation>
    </message>
    <message>
        <source>There were %1 input reads, %2 reads were classified.</source>
        <translation>%1 girdisi okundu, %2 okuma sınıflandırıldı.</translation>
    </message>
</context>
<context>
    <name>U2::PrepareWevoteTaxonomyDataTask</name>
    <message>
        <source>Prepare taxonomy data for WEVOTE</source>
        <translation>WEVOTE için sınıflandırma verilerini hazırlayın</translation>
    </message>
    <message>
        <source>Taxonomy data are missed</source>
        <translation>Sınıflandırma verileri eksik</translation>
    </message>
    <message>
        <source>Taxonomy file &apos;%1&apos; is not found.</source>
        <translation>&apos;%1&apos; sınıflandırma dosyası bulunamadı.</translation>
    </message>
</context>
<context>
    <name>U2::WevoteSupport</name>
    <message>
        <source>WEVOTE (WEighted VOting Taxonomic idEntification) is a metagenome shortgun sequencing DNA reads classifier based on an ensemble of other classification methods. In UGENE one can use the following methods: Kraken, CLARK, DIAMOND.</source>
        <translation>WEVOTE (WEVOTE (WEighted VOting Taxonomic idEntification), diğer sınıflandırma yöntemlerinden oluşan bir gruba dayalı bir metagenom kısa tabancalı dizileme DNA okuma sınıflandırıcısıdır. UGENE&apos;de şu yöntemler kullanılabilir: Kraken, CLARK, DIAMOND.</translation>
    </message>
</context>
<context>
    <name>U2::WevoteTask</name>
    <message>
        <source>Improve classification with WEVOTE</source>
        <translation>WEVOTE ile sınıflandırmayı iyileştirin</translation>
    </message>
    <message>
        <source>Wevote output file not found</source>
        <translation>Wevote çıktı dosyası bulunamadı</translation>
    </message>
    <message>
        <source>Number of agreed tools is less than zero</source>
        <translation>Mutabık kalınan araçların sayısı sıfırdan az</translation>
    </message>
    <message>
        <source>Score threshold is less than zero</source>
        <translation>Puan eşiği sıfırdan küçük</translation>
    </message>
    <message>
        <source>Number of threads is less or equal zero</source>
        <translation>Konu sayısı sıfıra eşit veya daha az</translation>
    </message>
    <message>
        <source>Input file URL is empty</source>
        <translation>Girdi dosyası URL&apos;si boş</translation>
    </message>
    <message>
        <source>Output file URL is empty</source>
        <translation>Çıktı dosyası URL&apos;si boş</translation>
    </message>
    <message>
        <source>Can&apos;t find prepared taxonomy for WEVOTE</source>
        <translation>WEVOTE için hazırlanmış sınıflandırma bulunamıyor</translation>
    </message>
    <message>
        <source>Can&apos;t overwrite the file &quot;%1&quot;</source>
        <translation>&quot;%1&quot; dosyasının üzerine yazılamıyor</translation>
    </message>
    <message>
        <source>Too few columns in the result file.</source>
        <translation>Sonuç dosyasında çok az sütun var.</translation>
    </message>
    <message>
        <source>Can&apos;t parse the taxID: &quot;%1&quot;</source>
        <translation>TaxID: &quot;%1&quot; ayrıştırılamıyor</translation>
    </message>
</context>
<context>
    <name>WevoteValidator</name>
    <message>
        <source>Taxonomy classification data from NCBI are not available.</source>
        <translation>NCBI&apos;dan taksonomi sınıflandırma verileri mevcut değildir.</translation>
    </message>
    <message>
        <source>Taxonomy classification data from NCBI are not full: file &apos;%1&apos; is missing.</source>
        <translation>NCBI&apos;daki sınıflandırma sınıflandırma verileri dolu değil: &apos;%1&apos; dosyası eksik.</translation>
    </message>
</context>
</TS>
