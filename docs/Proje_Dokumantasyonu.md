# C++ Ağ Programlama ile Çok Oyunculu Go Oyunu (Go Arena)

**Hazırlayanlar:**
- Kübra Gökçeoğlu (230206401)
- Şeyma Nur Temel (230206062)

**GitHub Proje Linki:**
🔗 [https://github.com/kubragokceoglu/go-oyunu-proje](https://github.com/kubragokceoglu/go-oyunu-proje)

---

## 1. Projenin Amacı ve Kapsamı
Bu proje, BİL314 Bilgisayar Ağları dersi kapsamında geliştirilmiş, iki oyuncunun yerel ağ (LAN) veya internet üzerinden karşılıklı olarak oynayabileceği, C++ tabanlı bir **Çok Oyunculu (Multiplayer) Go Oyunu** sunucusudur. 

Projede harici hiçbir web framework (Node.js, Django vb.) kullanılmamış olup, tüm HTTP haberleşmesi, paket alıp-verme (socket) ve eşzamanlılık (multithreading) işlemleri sıfırdan C++ (POSIX Sockets) ile geliştirilmiştir. İstemci (Client) tarafında ise oyunculara görsel bir arayüz sunabilmek adına Web Tarayıcıları (HTML/CSS/JS) kullanılmıştır.

## 2. Teknik Mimari

### A. Sunucu (Backend - C++)
- **Soket Programlama (POSIX Sockets):** Sunucu `8080` portunu dinleyerek ağ üzerinden gelen HTTP isteklerini (`read`) okur ve istemciye JSON/HTML formatında yanıt (`send`) döner.
- **Çoklu İş Parçacığı (Multithreading):** Her yeni bağlanan istemci için C++ `std::thread` kullanılarak bağımsız bir iş parçacığı oluşturulur. Böylece sunucu donmadan aynı anda birden fazla oyuncuya hizmet verebilir.
- **Eşzamanlılık Kontrolü (Mutex):** İki oyuncunun aynı anda hamle yapıp verileri bozmasını (Race Condition) engellemek için `std::mutex` ile tahta durumuna kilit (lock) konulmuştur.
- **Oyun Motoru (Go Logic):** Tahtadaki taşların nefeslerinin hesaplanması (DFS - Derinlik Öncelikli Arama algoritması), Ko kuralı, intihar kuralı ve esir alınan taşların silinmesi gibi karmaşık oyun mekanikleri C++ sınıfı içerisinde işlenir.
- **Kimlik Doğrulama:** Basit dosya tabanlı (File I/O) bir veritabanı (`users.txt`) üzerinden oyuncuların giriş ve kayıt işlemleri yönetilir.

### B. İstemci (Frontend - JS/HTML/CSS)
- **Dinamik Ağ Bağlantısı:** JavaScript kodu, tarayıcının bulunduğu IP adresini (`window.location.origin`) otomatik algılayarak sunucuya istek atar. Bu sayede ağdaki herhangi bir bilgisayardan oyuna erişilebilir.
- **Sürekli Yoklama (Polling):** WebSocket yerine HTTP üzerinden Polling tekniği kullanılmıştır. İstemci her saniye arka planda sunucuya `fetchStatus` isteği atarak rakibin hamlesinin ekrana gecikmesiz (Real-time hissiyle) düşmesini sağlar.

## 3. Kurulum ve Çalıştırma

Projenin derlenmesi ve çalıştırılması macOS/Linux sistemleri için tamamen otomatikleştirilmiştir.

**1. Derleme:**
Terminalde proje dizinine giderek şu komutu çalıştırın:
```bash
make clean
make
```
Bu komut, tüm C++ kaynak kodlarını derleyip `server` adlı çalıştırılabilir bir dosya oluşturacaktır.

**2. Sunucuyu Başlatma:**
```bash
./server
```
Sunucu `HTTP Server is listening on port 8080...` çıktısını verdiğinde çalışmaya başlamıştır.

**3. Oyuna Giriş:**
- **Sunucu Bilgisayarı (Oyuncu 1):** Tarayıcıdan `http://localhost:8080` adresine girin.
- **Ağdaki Diğer Bilgisayar (Oyuncu 2):** Sunucunun yerel IP adresini kullanarak (Örn: `http://192.168.1.5:8080`) tarayıcıdan girin.

Oyuncular farklı isimlerle giriş yaptıklarında oyun otomatik olarak başlar. Eğer oda doluysa ekrandaki **"Odayı Sıfırla"** butonuna basılarak sunucu hafızası temizlenebilir ve yeni oyuna geçilebilir.

---
*Bu dokümantasyon, projenin kaynak kodlarının bulunduğu GitHub dizinini ve projenin teknik yetkinliklerini akademik değerlendirme amacıyla özetlemektedir.*
