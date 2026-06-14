/*
 * Smart Sustainability Campus Analysis
 * Program analisis data keberlanjutan kampus menggunakan C++
 * Dataset: sustainability_data.csv
 *
 * Compile : g++ -std=c++17 -o main main.cpp
 * Jalankan: ./main
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <ctime>
#include <stdexcept>
#include <filesystem>

// ============================================================
// KONSTANTA
// ============================================================
const std::string DATASET_PATH = "../dataset/sustainability_data.csv";
const std::string OUTPUT_PATH  = "../output/hasil_analisis_cpp.txt";
const double RATING_GREEN   = 0.40;   // 0–40%  → Green
const double RATING_WARNING = 0.70;   // 40–70% → Warning
                                       // >70%   → Critical

// ============================================================
// STRUKTUR DATA
// ============================================================
struct Record {
    std::string tanggal;
    std::string lokasi;
    std::string parameter;
    double      nilai;
    std::string satuan;
};

struct StatParam {
    double rata_rata;
    double maksimum;
    double minimum;
    double total;
    int    count;
};

struct RatingInfo {
    double      skor;
    double      persen;
    std::string kategori;
};

// ============================================================
// UTILITAS STRING
// ============================================================
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string tok;
    while (std::getline(ss, tok, ','))
        result.push_back(trim(tok));
    return result;
}

// ============================================================
// FORMATTER GARIS
// ============================================================
std::string judul(const std::string& teks) {
    std::string baris(62, '=');
    return "\n" + baris + "\n  " + teks + "\n" + baris;
}

std::string subjudul(const std::string& teks) {
    std::string baris(62, '-');
    return "\n" + baris + "\n  " + teks + "\n" + baris;
}

// ============================================================
// 1. MEMBACA DATASET
// ============================================================
std::vector<Record> baca_dataset(const std::string& filepath) {
    std::vector<Record> records;
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Tidak bisa membuka file: " + filepath);

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto cols = split_csv(line);
        if (cols.size() < 5) continue;
        Record r;
        r.tanggal   = cols[0];
        r.lokasi    = cols[1];
        r.parameter = cols[2];
        r.nilai     = std::stod(cols[3]);
        r.satuan    = cols[4];
        records.push_back(r);
    }
    return records;
}

// ============================================================
// 2. RINGKASAN DATA
// ============================================================
struct Ringkasan {
    int         total_record;
    int         jumlah_gedung;
    int         jumlah_param;
    std::vector<std::string> daftar_gedung;
    std::vector<std::string> daftar_param;
    std::string tanggal_awal;
    std::string tanggal_akhir;
};

Ringkasan hitung_ringkasan(const std::vector<Record>& records) {
    std::set<std::string> gedung_set, param_set;
    std::string t_min = "9999-99-99", t_max = "0000-00-00";

    for (const auto& r : records) {
        gedung_set.insert(r.lokasi);
        param_set.insert(r.parameter);
        if (r.tanggal < t_min) t_min = r.tanggal;
        if (r.tanggal > t_max) t_max = r.tanggal;
    }

    Ringkasan ring;
    ring.total_record   = (int)records.size();
    ring.jumlah_gedung  = (int)gedung_set.size();
    ring.jumlah_param   = (int)param_set.size();
    ring.daftar_gedung  = std::vector<std::string>(gedung_set.begin(), gedung_set.end());
    ring.daftar_param   = std::vector<std::string>(param_set.begin(),  param_set.end());
    ring.tanggal_awal   = t_min;
    ring.tanggal_akhir  = t_max;
    return ring;
}

// ============================================================
// 3. ANALISIS STATISTIK
// ============================================================
std::map<std::string, StatParam> analisis_statistik(const std::vector<Record>& records) {
    std::map<std::string, std::vector<double>> data_per_param;
    for (const auto& r : records)
        data_per_param[r.parameter].push_back(r.nilai);

    std::map<std::string, StatParam> stats;
    for (auto& [param, vals] : data_per_param) {
        StatParam s;
        s.total    = std::accumulate(vals.begin(), vals.end(), 0.0);
        s.count    = (int)vals.size();
        s.rata_rata= s.total / s.count;
        s.maksimum = *std::max_element(vals.begin(), vals.end());
        s.minimum  = *std::min_element(vals.begin(), vals.end());
        stats[param] = s;
    }
    return stats;
}

// ============================================================
// 4. ANALISIS KAMPUS
// ============================================================
struct KampusInfo {
    // gedung → (parameter → rata-rata)
    std::map<std::string, std::map<std::string, double>> rata_per_gedung;
    std::pair<std::string, double> gedung_energy_tertinggi;
    std::pair<std::string, double> gedung_water_tertinggi;
    std::pair<std::string, double> gedung_co2_tertinggi;
    std::pair<std::string, double> gedung_waste_tertinggi;
};

KampusInfo analisis_kampus(const std::vector<Record>& records) {
    // Akumulasi nilai
    std::map<std::string, std::map<std::string, std::vector<double>>> agg;
    for (const auto& r : records)
        agg[r.lokasi][r.parameter].push_back(r.nilai);

    KampusInfo ki;
    for (auto& [gedung, params] : agg) {
        for (auto& [param, vals] : params) {
            double avg = std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
            ki.rata_per_gedung[gedung][param] = avg;
        }
    }

    // Cari tertinggi
    auto gedung_max = [&](const std::string& param_key) -> std::pair<std::string, double> {
        std::string best_g;
        double      best_v = -1.0;
        for (auto& [g, p] : ki.rata_per_gedung) {
            double v = (p.count(param_key) ? p.at(param_key) : 0.0);
            if (v > best_v) { best_v = v; best_g = g; }
        }
        return {best_g, best_v};
    };

    ki.gedung_energy_tertinggi = gedung_max("Energy");
    ki.gedung_water_tertinggi  = gedung_max("Water");
    ki.gedung_co2_tertinggi    = gedung_max("CO2");
    ki.gedung_waste_tertinggi  = gedung_max("Waste");
    return ki;
}

// ============================================================
// 5. KLASIFIKASI SUSTAINABILITY RATING
// ============================================================
std::map<std::string, RatingInfo> klasifikasi_rating(const std::vector<Record>& records) {
    // Global min/max per parameter
    std::map<std::string, double> g_min, g_max;
    std::map<std::string, std::vector<double>> data_per_param;
    for (const auto& r : records)
        data_per_param[r.parameter].push_back(r.nilai);

    for (auto& [p, v] : data_per_param) {
        g_min[p] = *std::min_element(v.begin(), v.end());
        g_max[p] = *std::max_element(v.begin(), v.end());
    }

    // Rata-rata per gedung per parameter
    std::map<std::string, std::map<std::string, std::vector<double>>> agg;
    for (const auto& r : records)
        agg[r.lokasi][r.parameter].push_back(r.nilai);

    std::map<std::string, RatingInfo> ratings;
    for (auto& [gedung, params] : agg) {
        double total_skor = 0.0;
        int    n_params   = 0;
        for (auto& [param, vals] : params) {
            double avg = std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
            double mn  = g_min[param], mx = g_max[param];
            double skor = (mx != mn) ? (avg - mn) / (mx - mn) : 0.0;
            total_skor += skor;
            n_params++;
        }
        double skor_akhir = (n_params > 0) ? total_skor / n_params : 0.0;

        RatingInfo ri;
        ri.skor   = skor_akhir;
        ri.persen = skor_akhir * 100.0;
        if      (skor_akhir <= RATING_GREEN)   ri.kategori = "Green    [0-40%]";
        else if (skor_akhir <= RATING_WARNING)  ri.kategori = "Warning  [40-70%]";
        else                                    ri.kategori = "Critical [>70%]";
        ratings[gedung] = ri;
    }
    return ratings;
}

// ============================================================
// SUSUN LAPORAN
// ============================================================
std::string buat_laporan(
    const std::vector<Record>&                        records,
    const Ringkasan&                                  ring,
    const std::map<std::string, StatParam>&           stats,
    const KampusInfo&                                 ki,
    const std::map<std::string, RatingInfo>&          ratings)
{
    std::ostringstream out;
    out << std::fixed;

    // Waktu sekarang
    time_t now = time(nullptr);
    char   buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    out << "\n";
    out << "+============================================================+\n";
    out << "|     LAPORAN ANALISIS SMART SUSTAINABILITY CAMPUS           |\n";
    out << "|              Program Analisis Data - C++                   |\n";
    out << "+============================================================+\n";
    out << "  Tanggal Laporan : " << buf << "\n";

    // ── Ringkasan ──────────────────────────────────────────────
    out << judul("1. RINGKASAN DATA") << "\n";
    out << "  Jumlah Record          : " << ring.total_record << "\n";
    out << "  Jumlah Gedung          : " << ring.jumlah_gedung << "\n";
    out << "  Jumlah Parameter       : " << ring.jumlah_param  << "\n";
    out << "  Periode Data           : " << ring.tanggal_awal
        << " s/d " << ring.tanggal_akhir << "\n";
    out << "\n  Daftar Gedung          :\n";
    for (int i = 0; i < (int)ring.daftar_gedung.size(); i++)
        out << "    " << std::setw(2) << (i+1) << ". " << ring.daftar_gedung[i] << "\n";
    out << "\n  Daftar Parameter       :\n";
    for (const auto& p : ring.daftar_param)
        out << "    - " << p << "\n";

    // ── Statistik ─────────────────────────────────────────────
    out << judul("2. ANALISIS STATISTIK (Per Parameter)") << "\n";
    std::map<std::string, std::string> satuan_map = {
        {"Energy","kWh"}, {"Water","L"}, {"CO2","ppm"}, {"Waste","kg"}
    };
    for (const auto& [param, s] : stats) {
        std::string sat = satuan_map.count(param) ? satuan_map.at(param) : "";
        out << subjudul("Parameter: " + param + " (" + sat + ")") << "\n";
        out << "  Rata-rata  : " << std::setw(12) << std::setprecision(2) << s.rata_rata  << " " << sat << "\n";
        out << "  Maksimum   : " << std::setw(12) << std::setprecision(2) << s.maksimum   << " " << sat << "\n";
        out << "  Minimum    : " << std::setw(12) << std::setprecision(2) << s.minimum    << " " << sat << "\n";
        out << "  Total      : " << std::setw(15) << std::setprecision(2) << s.total      << " " << sat << "\n";
        out << "  Jumlah Data: " << std::setw(12) << s.count << "\n";
    }

    // ── Analisis Kampus ────────────────────────────────────────
    out << judul("3. ANALISIS KAMPUS") << "\n";
    auto fw = [](int n){ return std::setw(n); };

    out << "  " << std::left << fw(35) << "Kategori"
        << fw(15) << "Gedung"
        << std::right << fw(12) << "Rata-rata" << "\n";
    out << "  " << std::string(35,'-') << " " << std::string(15,'-') << " " << std::string(12,'-') << "\n";

    out << std::left << "  " << fw(35) << "Konsumsi Energi Tertinggi"
        << fw(15) << ki.gedung_energy_tertinggi.first
        << std::right << std::setw(10) << std::setprecision(2) << ki.gedung_energy_tertinggi.second << " kWh\n";
    out << std::left << "  " << fw(35) << "Penggunaan Air Tertinggi"
        << fw(15) << ki.gedung_water_tertinggi.first
        << std::right << std::setw(10) << std::setprecision(2) << ki.gedung_water_tertinggi.second  << " L\n";
    out << std::left << "  " << fw(35) << "Emisi CO2 Tertinggi"
        << fw(15) << ki.gedung_co2_tertinggi.first
        << std::right << std::setw(10) << std::setprecision(2) << ki.gedung_co2_tertinggi.second    << " ppm\n";
    out << std::left << "  " << fw(35) << "Produksi Sampah Tertinggi"
        << fw(15) << ki.gedung_waste_tertinggi.first
        << std::right << std::setw(10) << std::setprecision(2) << ki.gedung_waste_tertinggi.second  << " kg\n";

    out << subjudul("Rata-rata Konsumsi Per Gedung (Semua Parameter)") << "\n";
    out << std::left
        << "  " << fw(15) << "Gedung"
        << std::right
        << fw(13) << "Energy(kWh)"
        << fw(11) << "Water(L)"
        << fw(11) << "CO2(ppm)"
        << fw(11) << "Waste(kg)" << "\n";
    out << "  " << std::string(15,'-') << " "
        << std::string(13,'-') << " "
        << std::string(10,'-') << " "
        << std::string(10,'-') << " "
        << std::string(10,'-') << "\n";
    for (const auto& [gedung, params] : ki.rata_per_gedung) {
        auto get = [&](const std::string& k) {
            return params.count(k) ? params.at(k) : 0.0;
        };
        out << std::left << "  " << fw(15) << gedung
            << std::right << std::setprecision(2)
            << fw(13) << get("Energy")
            << fw(11) << get("Water")
            << fw(11) << get("CO2")
            << fw(11) << get("Waste") << "\n";
    }

    // ── Sustainability Rating ──────────────────────────────────
    out << judul("4. KLASIFIKASI SUSTAINABILITY RATING") << "\n";
    out << "  Skema Klasifikasi:\n";
    out << "    0 - 40%  -> Green    (Penggunaan sumber daya rendah)\n";
    out << "   40 - 70%  -> Warning  (Penggunaan sumber daya sedang)\n";
    out << "   > 70%     -> Critical (Penggunaan sumber daya tinggi)\n\n";

    // Urutkan berdasarkan skor (tertinggi dulu)
    std::vector<std::pair<std::string, RatingInfo>> sorted_ratings(ratings.begin(), ratings.end());
    std::sort(sorted_ratings.begin(), sorted_ratings.end(),
              [](const auto& a, const auto& b){ return a.second.skor > b.second.skor; });

    out << std::left
        << "  " << fw(15) << "Gedung"
        << std::right
        << fw(8)  << "Skor"
        << fw(11) << "Persentil"
        << "  Kategori\n";
    out << "  " << std::string(15,'-') << " "
        << std::string(8,'-')  << " "
        << std::string(10,'-') << " "
        << std::string(20,'-') << "\n";
    for (const auto& [gedung, ri] : sorted_ratings) {
        out << std::left << "  " << fw(15) << gedung
            << std::right << std::setprecision(4) << fw(8) << ri.skor
            << std::setprecision(2) << fw(10) << ri.persen << "%"
            << "  " << ri.kategori << "\n";
    }

    int cnt_green = 0, cnt_warn = 0, cnt_crit = 0;
    for (const auto& [g, ri] : ratings) {
        if (ri.kategori.find("Green")    != std::string::npos) cnt_green++;
        else if (ri.kategori.find("Warning") != std::string::npos) cnt_warn++;
        else cnt_crit++;
    }

    out << subjudul("Rekap Kategori") << "\n";
    out << "  Green    (0-40%)  : " << cnt_green << " gedung\n";
    out << "  Warning  (40-70%) : " << cnt_warn  << " gedung\n";
    out << "  Critical (>70%)   : " << cnt_crit  << " gedung\n";

    out << "\n" << std::string(62,'=') << "\n";
    out << "  ANALISIS SELESAI\n";
    out << std::string(62,'=') << "\n\n";

    return out.str();
}

// ============================================================
// MAIN
// ============================================================
int main() {
    std::cout << std::string(62, '=') << "\n";
    std::cout << "  Smart Sustainability Campus - C++ Analysis\n";
    std::cout << std::string(62, '=') << "\n";

    try {
        // 1. Baca dataset
        std::cout << "\n[1] Membaca dataset...\n";
        auto records = baca_dataset(DATASET_PATH);
        std::cout << "  Berhasil membaca " << records.size() << " record.\n";

        // 2. Ringkasan
        std::cout << "\n[2] Menghitung ringkasan data...\n";
        auto ring = hitung_ringkasan(records);

        // 3. Statistik
        std::cout << "\n[3] Menghitung analisis statistik...\n";
        auto stats = analisis_statistik(records);

        // 4. Analisis Kampus
        std::cout << "\n[4] Menganalisis data per gedung...\n";
        auto ki = analisis_kampus(records);

        // 5. Rating
        std::cout << "\n[5] Mengklasifikasikan sustainability rating...\n";
        auto ratings = klasifikasi_rating(records);

        // Susun laporan
        std::cout << "\n[6] Menyusun laporan...\n";
        std::string laporan = buat_laporan(records, ring, stats, ki, ratings);

        // Tampilkan ke layar
        std::cout << laporan;

        // Simpan ke file
        // Buat folder output jika belum ada
        std::filesystem::create_directories("../output");
        std::ofstream ofs(OUTPUT_PATH);
        if (!ofs.is_open())
            throw std::runtime_error("Tidak bisa membuat file output: " + OUTPUT_PATH);
        ofs << laporan;
        ofs.close();

        std::cout << "[✓] Hasil analisis disimpan ke: " << OUTPUT_PATH << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
