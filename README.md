# EasyLang
EasyLang ilk olarak Türkçe yazılım geliştirmek için başlatılan bir programlama dilidir. Kurallar ve kontroller Türkçe kelimeler kullanılarak yazılmakta ve çalıştırılmaktadır. Şu anda Lexer ve AST kısımları üzerinde çalışmaktayım. Dil yapısı oturduktan sonra interpreator olarak çalışacak sonrasında LLVM entegrasyonu yapılması planlanmaktadır. Bu tamamen deneyse ve programlama dillerinin iç dinamiklerini öğrenmek için başlattığım bir proje.

Örnak Kullanım şekilleri aşağıdadır.
Basit atama işlemlemi.
```
atama degisken1'a 15
atama degisken2'a 'test data'
atama degisken3'a 5 artı 80
```

Fonksiyon çağrımı.
```
yaz degisken1
yaz 90
yaz 'yazma işlemi'
```

Koşul durumu.
```
eğer degisken1 büyükise degisken3 sonra
    yaz "büyük"
değilse
    yaz "küçük"
```

Şu an için emekle aşamasında bile olmasada Lexer ve AST kısımları çalışmakta fakat ayrıntılandırılması gerekmektedir.