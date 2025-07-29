#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <iomanip>
#include <thread>
#include <algorithm>
using namespace std;
using namespace chrono;
using namespace this_thread;

struct Process {
    int id;
    int arv;
    int cpu;
    int remaining;
};

int main() noexcept {
    constexpr int quantum = 2, n = 4;
    vector<Process> processes = {
        {1, 0, 12, 12},
        {2, 1, 3, 3},
        {3, 2, 4, 4},
        {4, 3, 2, 2},
        // {5, 4, 2, 2}
    };

    queue<int> rq;
    vector<int> completion_time(n, 0);

    int current_time = 0, done = 0, idx = 0;
    while(done < n) {
        while(idx < n && processes[idx].arv <= current_time) {
            rq.push(idx);
            ++idx;
        }

        if(!rq.empty()) {
            const int pidx = rq.front();
            rq.pop();
            Process& proc = processes[pidx];

            const int time_to_run = min(quantum, proc.remaining);
            current_time += time_to_run;
            proc.remaining -= time_to_run;

            sleep_for(milliseconds(time_to_run * 250));
            cout << "Tien trinh " << proc.id << " vua chay duoc " << time_to_run / 4.0 << "s\n";

            while(idx < n && processes[idx].arv <= current_time) {
                rq.push(idx);
                idx++;
            }

            if(proc.remaining > 0) {
                rq.push(pidx);
            }
            else {
            	  cout << "Tien trinh " << proc.id << " da chay xong\n";
                completion_time[pidx] = current_time;
                ++done;
            }
        }
        else {
            if(idx < n)
                current_time = processes[idx].arv;
        }
    }

    cout << "\n--------------------------------------------------\n" << std::left
    	 << setw(14) << "ID Tien trinh" << setw(22) << "| TG luu lai he thong" << "| TG cho\n"
    	 << "--------------|---------------------|-------------\n";

    double total_waiting_time = 0.0;
    for(size_t i = 0; i < n; ++i) {
        const int alive = completion_time[i] - processes[i].arv;
        const int wait = alive - processes[i].cpu;
        total_waiting_time += wait;

        cout << std::left << setw(14) << processes[i].id
             << "| " << setw(20) << alive
             << "| " << wait << endl;
    }

    cout << "\nThoi gian cho trung binh: " << fixed << setprecision(2) << total_waiting_time / n << endl;
}
