# BİL314 Bilgisayar Ağları Proje Tanımlama Dokümanı

## 1. Projeyi Geliştirenler
| Adı Soyadı | Öğrenci Numarası |
| :--- | :--- |
| Kübra Gökçeoğlu | 230206401 |
| Şeyma Nur Temel | 230206062 |

## 2. Projenin Konusu
Bu projede, C++ dili ve Soket Programlama (Socket Programming) kullanarak çok oyunculu ve çevrimiçi oynanabilen bir "Go" masa oyunu geliştirdik. Temel amacımız ağ üzerinden iki oyuncunun karşılıklı hamle yapabilmesini sağlamaktı.

## 3. Projenin Hedefleri
- İki farklı kullanıcının internet üzerinden birbirine bağlanıp gecikmesiz olarak Go oynayabileceği bir ortam kurmak.
- Kullanıcıların sisteme isimleriyle kayıt olup güvenli şekilde giriş yapmalarını sağlamak.
- Go oyununun o meşhur zor kurallarını (nefes hesaplama, esir alma, intihar ve ko kuralları) C++ ile sunucuda (server) hatasız bir şekilde simüle etmek.
- Kullanıcıları siyah beyaz terminal ekranına mahkum etmeyip, tarayıcı üzerinden erişebilecekleri renkli, tıklanabilir ve modern bir web arayüzü sunmak.

## 4. Kullanılan Metot ve Metodolojiler
- **Mimarî:** Sunucu-İstemci (Client-Server) modeli kullandık. Sunucuyu oyunun beyni olarak tasarladık; hamlelerin doğruluğunu ve kuralları sunucu kontrol ediyor. İstemci (tarayıcı) ise sadece ekrana tahtayı çiziyor ve kullanıcının tıkladığı koordinatı sunucuya iletiyor.
- **Dil ve Standartlar:** Arka plandaki tüm ağ işlemlerini (socket, bind, listen) ve oyun mantığını C++ ile yazdık. Ön yüz (frontend) için ise HTML, CSS ve saf JavaScript (Vanilla JS) kullandık.
- **İletişim Protokolü:** Veri kayıplarını göze alamayacağımız için TCP soketleri üzerinden kendi HTTP tabanlı iletişim yapımızı kurduk. JavaScript'in anlayabilmesi için verileri sunucudan JSON formatında gönderdik.
- **Veri Yapıları ve Algoritmalar:** Tahtayı 2 boyutlu bir matris (vektör) olarak tuttuk. Esir alınan taşların nefesini (liberty) hesaplamak için özyinelemeli (recursive) bir arama algoritması tasarladık. Kullanıcı bilgilerini ise basitçe `users.txt` dosyasına yazıp okuyarak dosya tabanlı bir veritabanı simüle ettik.

## 5. Sonuç, Yorumlar ve Kazanımlar

### Neler Öğrendik?
Bu proje gerçekten ufkumuzu açtı diyebiliriz. Sadece hazır kütüphaneler kullanmak yerine, TCP/IP soket programlamanın en temeline indik. Paketlerin nasıl gönderilip alındığını, bir HTTP isteğinin arka planda aslında nasıl bir metin (string) olduğunu kendi gözlerimizle gördük ve ayrıştırdık. Aynı zamanda C++ tarafında nesne yönelimli programlama tecrübemizi artırdık.

### Nerelerde Zorlandık ve Nasıl Aştık?
- **Zorluk:** İki oyuncu arka arkaya saniyede bir sürü paket yolladığında sunucunun çökmesi veya oyun tahtasının aynı anda değiştirilmeye çalışılması.
  **Çözüm:** Bunu çözmek için C++'ın çoklu işlem (thread) yeteneklerini kullandık. Ancak thread'ler aynı anda tahtaya yazmaya kalkışınca işler karıştı (Race Condition). Bu yüzden `std::mutex` kilidini devreye soktuk. Bir oyuncunun hamlesi işlenirken diğerini kilit sayesinde sıraya aldık.
- **Zorluk:** Go oyununda karmaşık taş gruplarının (zincirlerin) esir alınıp alınmadığını hesaplamak.
  **Çözüm:** Komşu taşları birbirine bağlayan ve boşluk kalıp kalmadığını kontrol eden recursive (özyinelemeli) bir derinlik öncelikli arama algoritması (DFS tarzı) kurarak bu sorunu da çözdük.
- **Zorluk:** Gerçek zamanlı (online) oyun hissiyatını vermek.
  **Çözüm:** JavaScript tarafında saniyede bir sunucuya `/status` paketi yollayarak (Polling tekniği) tahtayı sürekli güncel tutmayı başardık.

### Faydalanılan Kaynaklar
1. Beej's Guide to Network Programming
2. C++ Reference (`cppreference.com`)
3. Go Oyunu Kuralları (Sensei's Library)
4. Bilgisayar Ağları Ders Notları ve Slaytları
