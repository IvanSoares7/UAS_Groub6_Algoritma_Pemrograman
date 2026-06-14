/*
 * Smart Sustainability Campus Analysis
 * Program analisis data keberlanjutan kampus menggunakan Python
 * Dataset: sustainability_data.csv
 *
 * Compile : g++ -std=c++17 -o main main.py
 * Jalankan: ./main
 */

import csv
import random
from datetime import datetime, timedelta

# Daftar gedung
buildings = [
    "Gedung_A", "Gedung_B", "Gedung_C", "Gedung_D", "Gedung_E",
    "Gedung_F", "Gedung_G", "Gedung_H", "Gedung_I", "Gedung_J",
    "Gedung_K", "Gedung_L", "Gedung_M", "Gedung_N", "Gedung_O",
    "Gedung_P", "Gedung_Q", "Gedung_R", "Gedung_S", "Gedung_T"
]

start_date = datetime(2025, 1, 1)
jumlah_hari = 365

with open("dataset/sustainability_data.csv", "w", newline="") as file:
    writer = csv.writer(file)

    writer.writerow(["Date", "Building", "Parameter", "Value"])

    for i in range(jumlah_hari):
        tanggal = start_date + timedelta(days=i)

        for gedung in buildings:

            energy = round(random.uniform(80, 250), 2)
            water = round(random.uniform(200, 1200), 2)
            co2 = round(random.uniform(350, 900), 2)
            waste = round(random.uniform(5, 60), 2)

            writer.writerow([tanggal.date(), gedung, "Energy", energy])
            writer.writerow([tanggal.date(), gedung, "Water", water])
            writer.writerow([tanggal.date(), gedung, "CO2", co2])
            writer.writerow([tanggal.date(), gedung, "Waste", waste])

print("Dataset berhasil dibuat!")
print("Jumlah record = 29200")
