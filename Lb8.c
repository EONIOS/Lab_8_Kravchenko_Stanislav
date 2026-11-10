#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <omp.h>
#include <windows.h>

using namespace std;

int main() {
    SetConsoleOutputCP(65001);

 
    int m = 4;
    int n = 5;

    vector<vector<int>> A(m, vector<int>(n));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(-50, 50);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = dis(gen);
        }
    }

    cout << "Згенерована матриця A (" << m << "x" << n << "):\n";
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << A[i][j] << "\t";
        }
        cout << "\n";
    }
    cout << "----------------------------------------\n";

    ofstream outFile("result_var1.txt");
    if (!outFile.is_open()) {
        cerr << "Помилка відкриття файлу!\n";
        return 1;
    }

    omp_lock_t lock_file, lock_section;
    omp_init_lock(&lock_file);
    omp_init_lock(&lock_section);

    omp_set_num_threads(m);

#pragma shared(A, m, n, outFile, lock_file, lock_section)
    {
#pragma omp single
        {
            string header = "Номер лабораторної роботи: 8\n"
                "Назва лабораторної роботи: Використання функцій блокування та синхронізації технології OpenMP\n"
                "Група студента: Комп'ютерна інженерія\n"
                "ПІБ студента: Кравченко Станіслав Олександрович\n"
                "Номер варіанта: 11\n"
                "Номер завдання: Знайти мінімальне значення кожного рядка матриці\n"
                "========================================\n\n";
            outFile << header;
            cout << header;
        }

        int thread_id = omp_get_thread_num();

        if (thread_id < m) {
            while (!omp_test_lock(&lock_section)) {
                omp_set_lock(&lock_file);
                string msg = "[Потік " + to_string(thread_id) + "] Невдала спроба увійти до закритої секції, очікування...\n";
                outFile << msg;
                cout << msg;
                omp_unset_lock(&lock_file);

                Sleep(20);
            }

            omp_set_lock(&lock_file);
            string start_msg = "Початок закритої секції (потік " + to_string(thread_id) + ")\n";
            outFile << start_msg;
            cout << start_msg;
            omp_unset_lock(&lock_file);

            int min_val = A[thread_id][0];
            for (int j = 1; j < n; ++j) {
                if (A[thread_id][j] < min_val) {
                    min_val = A[thread_id][j];
                }
            }

            omp_set_lock(&lock_file);
            string res_msg = "Потік " + to_string(thread_id) + " обробив рядок " + to_string(thread_id) + ". Мінімальне значення = " + to_string(min_val) + "\n";
            outFile << res_msg;
            cout << res_msg;

            string end_msg = "Кінець закритої секції (потік " + to_string(thread_id) + ")\n\n";
            outFile << end_msg;
            cout << end_msg;
            omp_unset_lock(&lock_file);

            omp_unset_lock(&lock_section);
        }
    }

    omp_destroy_lock(&lock_file);
    omp_destroy_lock(&lock_section);
    outFile.close();

    cout << "Виконання завершено! Перевір файл 'result_var1.txt'.\n";

    return 0;
}
