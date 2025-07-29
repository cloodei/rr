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
    uint32_t id;
    uint32_t arv;
    uint32_t cpu;
    uint32_t remaining;
};

void sleep(const uint32_t ms) noexcept {
    sleep_for(milliseconds(ms));
}

int main() noexcept {
    constexpr uint32_t quantum = 2, n = 4;
    vector<Process> processes = {
        {1, 0, 12, 12},
        {2, 1, 3, 3},
        {3, 2, 4, 4},
        {4, 3, 2, 2},
        // {5, 4, 2, 2}
    };

    queue<uint32_t> rq;
    vector<uint32_t> completion_time(n);

    uint32_t now = 0, done = 0, idx = 0;
    while(done < n) {
        while(idx < n && processes[idx].arv <= now) {
            rq.push(idx);
            ++idx;
        }

        if(!rq.empty()) {
            const uint32_t pidx = rq.front();
            rq.pop();
            Process& proc = processes[pidx];

            const uint32_t time_to_run = min(quantum, proc.remaining);
            now += time_to_run;
            proc.remaining -= time_to_run;

            sleep(time_to_run * 250);
            cout << "Tien trinh " << proc.id << " vua chay duoc " << time_to_run / 4.0 << "s\n";

            while(idx < n && processes[idx].arv <= now) {
                rq.push(idx);
                idx++;
            }

            if(proc.remaining > 0) {
                rq.push(pidx);
            }
            else {
                cout << "Tien trinh " << proc.id << " da chay xong\n";
                completion_time[pidx] = now;
                ++done;
            }
        }
        else {
            if(idx < n)
                now = processes[idx].arv;
        }
    }

    cout << "\n--------------------------------------------------\n" << std::left
         << setw(14) << "ID Tien trinh" << setw(22) << "| TG luu lai he thong" << "| TG cho\n"
         << "--------------|---------------------|-------------\n";

    double sum = 0.0;
    for(uint32_t i = 0; i < n; ++i) {
        const uint32_t alive = completion_time[i] - processes[i].arv;
        const uint32_t wait = alive - processes[i].cpu;
        sum += wait;

        cout << std::left << setw(14) << processes[i].id
             << "| " << setw(20) << alive
             << "| " << wait << endl;
    }

    cout << "\nThoi gian cho trung binh: " << fixed << setprecision(2) << sum / n << endl;
}
