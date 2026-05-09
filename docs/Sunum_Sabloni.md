# BİL314 Proje Sunum Şablonu (Maksimum 10 Slayt)

*(Not: Bu metni PowerPoint veya benzeri bir sunum aracına aktararak yansılarınızı oluşturabilirsiniz.)*

## Yansı 1: Kapak
- **Başlık:** Bilgisayar Ağları Dersi Projesi: Çevrimiçi Go Oyunu
- **Hazırlayanlar:** [Öğrenci 1] & [Öğrenci 2]
- **Öğrenci Numaraları:** [Numara 1] & [Numara 2]
- **Danışman:** Dr. Cemal GEMCİ

## Yansı 2: Projenin Amacı ve Hedefleri
- İstemci-sunucu (Client-Server) mimarisi ile çalışan çevrimiçi bir oyun tasarlamak.
- TCP/IP soket programlama konseptlerini gerçek bir senaryoda uygulamak.
- Kimlik doğrulama (Kullanıcı Girişi/Kayıt) sistemi oluşturmak.
- Karmaşık bir masa oyunu olan Go'nun algoritmalarını (nefes hesabı, esir alma) C++ dili ile geliştirmek.

## Yansı 3: Kullanılan Teknolojiler
- **Programlama Dili:** C++
- **Ağ Programlama:** POSIX Sockets (TCP/IP)
- **Veri Saklama:** Dosya Sistemleri (File I/O) ile kullanıcı veritabanı simülasyonu
- **Versiyon Kontrol:** Git & Github
- **Kullanıcı Arayüzü:** Renkli ve dinamik Terminal Arayüzü (CLI)

## Yansı 4: Mimari Tasarım (Sunucu)
- Aynı anda istemci bağlantılarını dinleyen yapı.
- Oyun seanslarının (Game Session) yönetimi.
- Kuralların doğrulanması (Hile engelleme mekanizmaları).
- Kullanıcı girişlerinin kontrolü (Authentication).

## Yansı 5: Mimari Tasarım (İstemci)
- Sunucuyla TCP üzerinden sürekli haberleşme.
- Kullanıcıdan klavye girdilerinin alınması (Örn: `E 4`, `pass`, `login`).
- ANSI kaçış dizileriyle (ANSI escape codes) tahtanın kullanıcı dostu ve renkli biçimde çizdirilmesi.

## Yansı 6: Go Oyunu Algoritmaları
- **Tahta Modeli:** 2 boyutlu `enum` dizileri (Boş, Siyah, Beyaz).
- **Nefes (Liberty) Hesaplama:** Flood Fill algoritması ile birbirine bağlı taş gruplarının nefeslerinin sayılması.
- **Esir Alma (Capture):** Nefesi 0'a düşen düşman gruplarının tespit edilip tahtadan silinmesi.
- **İntihar Kuralı:** Geçersiz hamlelerin tespiti.

## Yansı 7: Demo Ekran Görüntüleri - 1 (Giriş)
- *(Buraya projenin çalıştırıldığı ve Kullanıcı Girişi/Kayıt olma ekranının bir ekran görüntüsünü koyunuz.)*

## Yansı 8: Demo Ekran Görüntüleri - 2 (Oyun Tahtası)
- *(Buraya iki istemcinin karşılıklı oyun oynarken çekilmiş bir ekran görüntüsünü koyunuz.)*

## Yansı 9: Kazanımlar
- TCP soketlerinin çalışma mantığının kavranması.
- Uygulama katmanında kendi protokolümüzü geliştirme tecrübesi.
- Takım çalışması, Github kullanımı ve C++'ta veri yapıları pratiği.

## Yansı 10: Teşekkür
- Bizi dinlediğiniz için teşekkür ederiz.
- Sorularınız?
