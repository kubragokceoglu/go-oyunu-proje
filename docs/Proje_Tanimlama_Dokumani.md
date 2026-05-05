# BİL314 Bilgisayar Ağları - Proje Tanımlama Dokümanı ve Dönem Sonu Raporu

## a. Projeyi Geliştirenler
| Adı Soyadı | Öğrenci Numarası |
| :--- | :--- |
| **Kübra Gökçeoğlu** | 230206401 |
| **Şeyma Nur Temel** | 230206062 |

---

## b. Projenin Konusu
Bu projenin temel konusu; C++ programlama dili ve işletim sistemlerinin temel haberleşme arayüzü olan Soket Programlama (Socket Programming) teknikleri kullanılarak, istemci-sunucu (client-server) mimarisinde çalışan, çok oyunculu (multiplayer) ve ağ destekli bir masa oyunu geliştirmektir. Oyun olarak, kurallarının matematiksel ve algoritmik yapısı nedeniyle uygulaması oldukça zorlayıcı olan, ancak bir o kadar da öğretici olan tarihi **Go** oyunu (9x9 tahta boyutunda) seçilmiştir. 

Proje kapsamında sadece bir oyun mantığı kodlanmamış, aynı zamanda istemcilerin (oyuncuların) internet üzerinden birbirleriyle konuşabilmesi için sıfırdan bir HTTP sunucusu (web server) inşa edilmiştir. Standart kütüphaneler dışında hazır bir ağ (network) veya web çatısı (framework) kullanılmamış, soket dinleme işlemlerinden gelen HTTP metin paketlerinin ayrıştırılmasına (parsing) kadar her adım C++ ile tamamen tarafımızca kodlanmıştır.

---

## c. Projenin Hedefleri
Projeye başlarken kendimize hem dersin gerekliliklerini sağlayan hem de bizi mühendislik anlamında zorlayacak bazı temel hedefler belirledik. Bu hedefleri şu şekilde sıralayabiliriz:

1. **İnternet Üzerinde Çevrimiçi (Online) Ortamda Çalışabilirlik:** Oyuncuların aynı bilgisayar üzerinde değil, ağ üzerinden IP adresleri aracılığıyla karşılıklı olarak birbirine bağlanıp veri kaybı yaşamadan gerçek zamanlı oyun oynayabileceği bir yapı kurmak.
2. **Kendi HTTP Sunucumuzu (Server) Geliştirmek:** Bilgisayar ağları dersinde öğrendiğimiz düşük seviyeli (low-level) TCP bağlantılarını kullanarak, tarayıcıların (Chrome, Safari vb.) anlayabileceği standartlarda yanıt veren bağımsız bir HTTP sunucusu tasarlamak.
3. **Senkronizasyon ve Eşzamanlılık:** İki farklı ağ ucundan aynı anda gelen hamle veya durum isteklerinin sunucuyu kilitlemesini veya verileri bozmasını (Race Condition) engellemek; eşzamanlı, kesintisiz bir çoklu görev (multithreading) ortamı yaratmak.
4. **Platform Bağımsız, Kullanıcı Dostu İstemci (Client) Arayüzü:** Ağ üzerinden gönderilen verilerin oyuncuya anlamlı gelmesi için siyah-beyaz bir komut satırı ekranı yerine, herkesin kolayca erişebileceği tarayıcı tabanlı (HTML, CSS, JavaScript) renkli ve tıklanabilir modern bir arayüz sunmak.
5. **Güvenli Oturum (Session) Yönetimi:** Oyuncuların sunucuya ilk bağlandıklarında kullanıcı adı ve şifreleriyle doğrulama yapıp benzersiz bir jeton (Token) almalarını sağlamak ve yetkisiz hamle gönderimlerini reddetmek.
6. **Karmaşık Oyun Mantığını (Go) Hatasız Simüle Etmek:** Ağ üzerinden gelen koordinat verilerini alarak nefes (liberty) hesaplama, esir alma (capture), ko (sonsuz döngü engelleme) ve intihar (suicide) gibi zorlayıcı kuralları sunucu tarafında doğrulamak.

---

## d. Kullandığımız Metot ve Metodolojiler

Bu projenin mimarisi "Sunucu-İstemci (Client-Server)" modeline dayanmaktadır. Tüm metodolojimizi arka plan (Backend/Server) ve ön yüz (Frontend/Client) olmak üzere ikiye ayırarak tasarladık.

### 1. Arka Plan (Server) Mimarisi ve TCP Soket Programlama
Ağ üzerinden iletişim kurmak için C++ dilini ve POSIX standartlarındaki `<sys/socket.h>` kütüphanesini tercih ettik. Oyun hamlelerinin kaybolması veya sırasının değişmesi oyunun bütünlüğünü tamamen bozacağı için, UDP yerine güvenilirlik ve sıralı paket iletimi garantisi veren **TCP (Transmission Control Protocol)** kullandık. 
*   **Bind ve Listen:** Sunucumuzu `8080` numaralı porta bağladık (bind) ve gelen bağlantıları dinlemeye (listen) aldık.
*   **Multithreading (Çoklu İş Parçacığı):** Gelen her yeni bağlantı isteğini `accept()` fonksiyonuyla yakaladıktan sonra, sunucunun diğer istekleri dinlemeyi bırakmaması için bu isteği anında yeni bir Thread'e (`std::thread`) atadık. Böylece aynı anda onlarca istemci sunucuya istek yollayabilir hale geldi.
*   **HTTP Protokolü ve REST-Vari API:** TCP soketlerimizin içine rastgele byte'lar göndermek yerine, evrensel HTTP protokolü standartlarını uyguladık. İstemciden gelen `GET /move?token=XYZ&r=3&c=4 HTTP/1.1` şeklindeki ham metin (string) paketlerini C++ içinde parçalayıp (parsing) anlamlandırdık.
*   **JSON ile Veri Serileştirme:** C++'taki karmaşık tahta matrisimizi ağ üzerinden istemciye gönderirken, verilerin tarayıcı tarafından kolayca işlenebilmesi için evrensel bir format olan JSON (JavaScript Object Notation) formatına dönüştürerek yolladık.

### 2. Oyun Mantığı ve Algoritmalar (Go Logic)
Oyunun kurallarını uygulamak, ağ programlamadan sonraki en büyük algoritmik mücadelemizdi.
*   **Veri Yapısı:** Tahtayı 9x9 boyutlarında `std::vector<std::vector<Stone>>` yapısıyla iki boyutlu bir uzay olarak modeledik.
*   **Flood Fill ve DFS:** Go oyununda bir taşın veya taş grubunun etrafı sarıldığında (nefesi kalmadığında) ölmesi gerekir. Bu "nefes" (liberty) hesaplamasını yapabilmek için Derinlik Öncelikli Arama (DFS - Depth First Search) mantığıyla çalışan özyinelemeli (recursive) bir fonksiyon yazdık. Bu fonksiyon, taşa bitişik tüm aynı renk taşları tarayarak herhangi bir açık nokta olup olmadığını ağaç (tree) yapısı gibi taramaktadır.

### 3. İstemci (Client) ve Durum Senkronizasyonu
Kullanıcı arayüzünü standart Web teknolojileri (HTML, CSS, Vanilla JS) ile hazırladık. Arayüzün sunucuyla konuşması için **Polling (Sürekli Yoklama)** metodunu kullandık. JavaScript kodumuz, saniyede 1 kez `fetch()` fonksiyonunu kullanarak sunucuya `GET /status` paketi gönderir. Sunucu o anki tahta durumunu ve sıranın kimde olduğunu paketleyip geri yollar. Böylece karşı taraf ağ üzerinden hamle yaptığında, bizim ekranımız en geç 1 saniye içinde kendiliğinden güncellenerek gerçek zamanlı bir akış (online hissiyatı) sağlar.

---

## e. Sonuç, Yorumlar ve Kazanımlarımız

Projeyi tamamladığımızda dönüp arkamıza baktığımızda, ilk başta gözümüze çok zor görünen bir sistemi çalışır halde görmenin inanılmaz bir tatmini olduğunu fark ettik. Bu süreç, sadece kod yazmaktan ibaret değildi; aynı zamanda karmaşık ağ mimarilerini, düşük seviye bellek yönetimini ve algoritmik düşünceyi bir araya getiren tam bir mühendislik deneyimiydi. Öğrendiklerimizi, faydalandığımız kaynakları ve zorlandığımız kısımları aşağıdaki başlıklarda detaylandırdık.

### Neler Öğrendik?
*   **Soket Programlamanın İçyüzü:** Bugün sıklıkla kullandığımız Node.js, Python Flask veya Apache gibi hazır sunucuların arka planda işletim sistemiyle nasıl konuştuğunu öğrendik. "Soket", "Port", "Bind" kavramlarının teoriden çıkıp gerçek hayatta nasıl veri taşıdığını uygulamalı olarak tecrübe ettik.
*   **HTTP Protokolünün Anatomisi:** İnternet tarayıcımıza bir adres yazdığımızda giden paketin aslında sıradan bir metin (string) dosyasından farksız olduğunu, başlıkların (Headers) ve gövdelerin (Body) `\r\n` karakterleriyle nasıl ayrıldığını kendimiz kodlayarak keşfettik.
*   **C++'ın Gücü:** Nesne yönelimli programlama, referanslar (pointers) ve standart şablon kütüphanesi (STL) konularındaki yetkinliğimizi artırdık.
*   **İstemci-Sunucu İletişimi:** Modern web uygulamalarındaki REST API mantığını ve JSON formatının sistemler arası haberleşmede neden endüstri standardı olduğunu tecrübe ettik.

### Nerelerde Zorlandık ve Zorlukları Nasıl Aştık?
Geliştirme sürecinde karşılaştığımız engeller ve ürettiğimiz mühendislik çözümleri şu şekildeydi:

1.  **Zorluk: Ağ Üzerinde Eşzamanlılık ve Veri Bozulmaları (Race Condition)**
    Oyunumuz ağ üzerinde aynı anda birden fazla isteği yanıtlamak üzere `Thread`'ler ile çalışıyordu. Ancak testlerimiz sırasında, iki oyuncu tam olarak aynı milisaniyede hamle yolladığında veya biri hamle yollarken diğeri tahtanın durumunu çekmek istediğinde (Polling) sunucumuz çöküyor veya oyun tahtasının (board matrisinin) verisi bozuluyordu. İki farklı iş parçacığı aynı hafıza bölgesine (memory) yazmaya çalışıyordu.
    **Nasıl Aştık:** Bu kritik problemi çözmek için C++'ın eşzamanlılık (concurrency) kütüphanesinden faydalanarak projeye **`std::mutex` (Mutual Exclusion - Karşılıklı Dışlama)** kilit mekanizmasını ekledik. `std::lock_guard<std::mutex> lock(gameMutex);` satırı sayesinde, bir ağ paketi tahtaya müdahale ediyorsa diğer paketlerin işlem bitene kadar kapıda (kilitte) beklemesini sağladık. Böylece Race Condition hatasını tamamen ortadan kaldırdık.

2.  **Zorluk: C++ Sunucusunu Tarayıcı ile Konuşturmak (CORS Hatası)**
    C++ sunucumuzu çalıştırıp tarayıcıdan (JavaScript) paket göndermeye kalktığımızda tarayıcının güvenlik sebebiyle bağlantıyı reddettiğini (CORS - Cross-Origin Resource Sharing hatası) gördük. Başta bunun soket kodlarımızdaki bir hata olduğunu sandık.
    **Nasıl Aştık:** Sorunun ağ paketimizin eksik olmasından kaynaklandığını, ağ ve güvenlik standartları gereği tarayıcıya özel bir izin pakedi göndermemiz gerektiğini fark ettik. C++ üzerinden yolladığımız HTTP yanıtlarına `Access-Control-Allow-Origin: *` başlığını (header) ekleyerek farklı kaynakların sunucumuzla güvenle konuşabilmesini sağladık.

3.  **Zorluk: Go Oyunundaki Karmaşık "Nefes (Liberty)" Algoritması**
    Go oyununda bir taş tek başına da esir alınabilir, birbirine yapışık 20 taştan oluşan bir zincir de tek bir hamleyle esir alınabilir. Bu bağlantılı zincirleri kodla tespit etmek çok karmaşıktı.
    **Nasıl Aştık:** Graph (Çizge) teorisindeki algoritmaları araştırdık. `hasLiberty` ismini verdiğimiz, derinlik öncelikli arama (DFS) yapan ve tahtayı rekürsif (özyinelemeli) olarak dolaşan bir fonksiyon yazdık. Sonsuz döngüye girmemek için daha önce baktığımız taşları kaydettiğimiz (`visited` matrisi) bir algoritma tasarladık.

4.  **Zorluk: WebSockets Olmadan Gerçek Zamanlı Oyun Yapmak**
    Normalde online oyunlar WebSockets kullanır. Ancak biz TCP soketlerini HTTP gibi tasarladığımız için sunucudan istemciye anlık paket yollamamız (Push) mümkün değildi. 
    **Nasıl Aştık:** Bu durumu **Polling (Yoklama)** tekniği ile aştık. JavaScript tarafında `setInterval` kurarak saniyede bir sunucuya "Yeni bir gelişme var mı?" minvalinde durum yoklama paketi (`/status`) attırdık. Böylece oyunun akıcılığından ödün vermeden asenkron ve gerçek zamanlı iletişimi sağladık.

### Faydalandığımız Kaynaklar (Referanslar)
Bu zorlu ama eğitici süreçte bilgi edindiğimiz ve tıkandığımız noktalarda başvurduğumuz temel kaynaklar şunlardır:
1.  **Beej's Guide to Network Programming:** Soket programlamanın, TCP/IP protokollerinin çalışma mantığının ve `bind`, `listen`, `accept` gibi C sistem çağrılarının nasıl kullanılacağının anlatıldığı temel başvuru kaynağımız oldu.
2.  **Bilgisayar Ağları Ders Slaytları ve Notları:** İnternet protokollerinin yapısı ve İstemci-Sunucu (Client-Server) mimarisinin temelleri konusunda referans aldık.
3.  **cppreference.com (C++ Standart Kütüphane Referansı):** Özellikle multithreading (`<thread>`) ve kilit mekanizmaları (`<mutex>`) gibi bellek yönetimi konularında C++'ın resmi dokümantasyonundan faydalandık.
4.  **Sensei's Library (Go Kaynakları):** Algoritmayı kurarken Go oyununun kurallarını (özellikle Ko kuralı ve İntihar yasakları) dijital ortama aktarabilmek adına oyun kuralları ansiklopedisinden yararlandık.

---
*(Bu doküman projenin akademik dökümantasyonu olup, uygulamanın ağ programlama prensiplerine nasıl entegre edildiğini detaylıca açıklamaktadır.)*
