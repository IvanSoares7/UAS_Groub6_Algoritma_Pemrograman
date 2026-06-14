# UAS-PROG-KelompokXX

## Anggota Kelompok
1. Nama - NIM
2. Nama - NIM
3. Nama - NIM

## Deskripsi Proyek
Analisis Smart Sustainability Campus menggunakan C++ dan Python.

Program ini membaca dataset monitoring lingkungan kampus (`sustainability_data.csv`)
yang mencatat konsumsi **energi, air, emisi CO₂, dan produksi sampah** dari
20 gedung kampus selama tahun 2025 (29.200 record total). Program melakukan:
1. Pembacaan & parsing CSV
2. Ringkasan data (jumlah record, gedung, parameter)
3. Analisis statistik (rata-rata, maksimum, minimum)
4. Analisis kampus per gedung
5. Klasifikasi sustainability rating (Green / Warning / Critical)
6. Ekspor seluruh hasil ke file `.txt`

## Struktur Folder
```
sustainability_project/
├── cpp/
│   └── main.cpp          ← Program C++
├── python/
│   └── main.py           ← Program Python
├── dataset/
│   └── sustainability_data.csv
├── output/
│   ├── hasil_analisis_cpp.txt
│   └── hasil_analisis_python.txt
└── README.md
```

## Cara Menjalankan Program

### C++
```bash
cd cpp
g++ -std=c++17 -O2 -o main main.cpp
./main
```

### Python
```bash
cd python
python main.py
```

## Skema Klasifikasi Sustainability Rating
| Rentang Skor | Kategori | Keterangan |
|---|---|---|
| 0 – 40% | 🟢 Green | Penggunaan sumber daya rendah |
| 40 – 70% | 🟡 Warning | Penggunaan sumber daya sedang |
| > 70% | 🔴 Critical | Penggunaan sumber daya tinggi |

> Skor dihitung dengan normalisasi min-max setiap parameter, kemudian dirata-rata.

## Link Video Presentasi
https://youtu.be/xxxxxxxx
