# BİL314 Bilgisayar Ağları - Çevrimiçi Go Oyunu

Bu proje, BİL314 Bilgisayar Ağları dersi kapsamında C++ ve POSIX soketleri kullanılarak geliştirilmiş çok oyunculu, istemci-sunucu (client-server) mimarisine sahip çevrimiçi bir Go oyunudur.

## Proje İsterleri
- İstemci ve sunucu haberleşmesi
- Kullanıcı tanımlama ve yetkilendirme (Authentication) menüsü
- C/C++ dilinde geliştirme
- Go oyunu kurallarının eksiksiz çalışması (Nefes hesabı, esir alma, vs.)

## Kurulum ve Derleme (Linux / Mac)

Projenin derlenebilmesi için sisteminizde `g++` derleyicisi ve `make` aracının kurulu olması gerekmektedir.

```bash
# Proje dizinine gidin
cd go-oyunu-proje

# Tüm projeyi derlemek için (sunucu ve istemci)
make

# Sadece sunucuyu derlemek için
make server

# Sadece istemciyi derlemek için
make client
```

## Nasıl Oynanır?

1. **Sunucuyu Başlatın:** İlk olarak bir terminal penceresi açın ve sunucuyu çalıştırın. Sunucu varsayılan olarak `8080` portunu dinleyecektir.
   ```bash
   ./server
   ```
2. **Oyuncuların Bağlanması:** İki farklı terminal penceresi (veya farklı bilgisayarlar) açarak istemci uygulamasını çalıştırın.
   ```bash
   ./client
   ```
3. **Kayıt ve Giriş:** İstemci ekranında karşınıza çıkan menüden sisteme kayıt olabilir veya giriş yapabilirsiniz. Kullanıcı bilgileri `data/users.txt` dosyasında saklanmaktadır.
4. **Oyun İçi:** İki oyuncu giriş yaptığında oyun otomatik olarak başlar. Hamle yapmak için tahtanın yanlarındaki harf ve rakamları kullanarak koordinat girin (Örn: `A 5`, `D 4`). Hamle yapmak istemiyorsanız `pass` yazarak sıranızı geçebilirsiniz.
