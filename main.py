import pandas as pd
import matplotlib.pyplot as plt

# Membuka file output
file = open("output/hasil_analisis_python.txt", "w")
# Membaca dataset
df = pd.read_csv("dataset/sustainability_data.csv")

print("=" * 50)
print("LAPORAN ANALISIS SMART SUSTAINABILITY CAMPUS")
print("=" * 50)

file.write("\n")
file.write("    LAPORAN ANALISIS SMART SUSTAINABILITY CAMPUS\n")
file.write("          Program Analisis Data - Python\n\n")

file.write("# UAS-PROG-Kelompok 6\n\n")

file.write("# Anggota Kelompok\n")
file.write("1. Muhammad Purnama Adi Putra - 25050874215\n")
file.write("2. Ivan Febrian Soares - 25050874024\n\n")

# Ringkasan data
print("\n1. RINGKASAN DATA")
print("Jumlah Record :", len(df))
print("Jumlah Gedung :", df["Building"].nunique())
print("Jumlah Parameter :", df["Parameter"].nunique())

file.write("\n1 . RINGKASAN DATA\n")
file.write("Jumlah Record : " + str(len(df)) + "\n")
file.write("Jumlah Gedung : " + str(df["Building"].nunique()) + "\n")
file.write("Jumlah Parameter : " + str(df["Parameter"].nunique()) + "\n")

# Statistik tiap parameter
print("\n2. ANALISIS STATISTIK")

for parameter in df["Parameter"].unique():

    data_parameter = df[df["Parameter"] == parameter]

    rata = round(data_parameter["Value"].mean(), 2)
    maksimum = round(data_parameter["Value"].max(), 2)
    minimum = round(data_parameter["Value"].min(), 2)
    total = round(data_parameter["Value"].sum(), 2)

    print("\nParameter :", parameter)
    print("Rata-rata :", rata)
    print("Maksimum :", maksimum)
    print("Minimum :", minimum)
    print("Total :", total)

# Analisis gedung
print("\n3. ANALISIS GEDUNG")

file.write("\n3. ANALISIS GEDUNG\n")

for parameter in df["Parameter"].unique():

    data_parameter = df[df["Parameter"] == parameter]

    rata_gedung = data_parameter.groupby("Building")["Value"].mean()

    gedung_tertinggi = rata_gedung.idxmax()

    nilai_tertinggi = round(rata_gedung.max(), 2)

    print("\nParameter :", parameter)
    print("Gedung tertinggi =", gedung_tertinggi)
    print("Nilai =", nilai_tertinggi)

    file.write("\nParameter : " + parameter + "\n")
    file.write("Gedung tertinggi = " + gedung_tertinggi + "\n")
    file.write("Nilai = " + str(nilai_tertinggi) + "\n")
# Sustainability rating
print("\n4. SUSTAINABILITY RATING")

file.write("\n4. SUSTAINABILITY RATING\n")

for gedung in df["Building"].unique():

    data_gedung = df[df["Building"] == gedung]

    skor = data_gedung["Value"].mean()

    if skor < 300:
        kategori = "Green"
    elif skor < 500:
        kategori = "Warning"
    else:
        kategori = "Critical"

    print(gedung, ":", kategori)

    file.write(gedung + " : " + kategori + "\n")
# Ranking gedung
print("\n5. RANKING GEDUNG")

file.write("\n5. RANKING GEDUNG\n")

rata_semua_gedung = df.groupby("Building")["Value"].mean()

ranking = rata_semua_gedung.sort_values(ascending=False)

nomor = 1

for gedung, nilai in ranking.items():

    nilai = round(nilai, 2)

    print(str(nomor) + ".", gedung, "-", nilai)

    file.write(str(nomor) + ". " +
               gedung +
               " - " +
               str(nilai) +
               "\n")

    nomor += 1

# Grafik
print("\n6. MEMBUAT GRAFIK")

nama_gedung = ranking.index
nilai_gedung = ranking.values

plt.figure(figsize=(12, 6))

plt.bar(nama_gedung, nilai_gedung)

plt.xlabel("Gedung")
plt.ylabel("Rata-rata Nilai")
plt.title("Ranking Gedung Berdasarkan Rata-rata Nilai")

plt.xticks(rotation=45)

plt.tight_layout()

plt.savefig("output/grafik_ranking_gedung.png")

print("Grafik berhasil disimpan!")
file.close()
