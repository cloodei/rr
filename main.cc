#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <iomanip>
#include <thread>
#include <algorithm>
using namespace std;

struct Process {
    uint32_t id;
    uint32_t arv;
    uint32_t cpu;
    uint32_t remaining;
};

struct GanttSegment {
    uint32_t process_id;
    uint32_t end_time;
};

inline void sleep(const uint32_t ms) noexcept {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void printGanttChart(const vector<GanttSegment>& chart) noexcept {
    if(chart.empty())
        return;

    cout << "----- So do Gantt -----\n+";

    int last_time = 0;
    for(const auto& segment : chart) {
        int duration = (segment.end_time - last_time) << 1;
        for(int i = 0; i < duration; ++i)
            cout << "-";

        cout << "+";
        last_time = segment.end_time;
    }

    cout << "\n|";
    last_time = 0;

    for(const auto& segment : chart) {
        int duration = ((segment.end_time - last_time) << 1) - 2;
        cout << "P" << segment.process_id;
        for(int i = 0; i < duration; ++i)
            cout << " ";

        cout << "|";
        last_time = segment.end_time;
    }

    cout << "\n+";
    last_time = 0;

    for(const auto& segment : chart) {
        int duration = (segment.end_time - last_time) << 1;
        for(int i = 0; i < duration; ++i)
            cout << "-";

        cout << "+";
        last_time = segment.end_time;
    }

    cout << "\n0";
    last_time = 0;

    for(const auto& segment : chart) {
        int duration = ((segment.end_time - last_time) << 1) - 2;
        for(int i = 0; i < duration; ++i)
            cout << " ";

        if(segment.end_time > 99)
            cout << segment.end_time;
        else if(segment.end_time > 9)
            cout << " " << segment.end_time;
        else
            cout << "  " << segment.end_time;

        last_time = segment.end_time;
    }

    cout << endl;
}


int main() noexcept {
    constexpr uint32_t quantum = 4, n = 4;
    vector<Process> processes = {
        {1, 0, 12, 12},
        {2, 1, 3, 3},
        {3, 2, 4, 4},
        {4, 3, 2, 2}
    };

    queue<uint32_t> rq;
    vector<uint32_t> completion_time(n);
    vector<GanttSegment> gantt_chart;

    uint32_t now = 0, done = 0, idx = 0;
    while(done < n) {
        while(idx < n && processes[idx].arv <= now)
            rq.push(idx++);

        if(!rq.empty()) {
            const uint32_t pidx = rq.front();
            rq.pop();
            Process& proc = processes[pidx];

            const uint32_t time_to_run = min(quantum, proc.remaining);
            now += time_to_run;
            proc.remaining -= time_to_run;

            gantt_chart.push_back({ proc.id, now });

            while(idx < n && processes[idx].arv <= now)
                rq.push(idx++);

            if(proc.remaining != 0)
                rq.push(pidx);
            else
                completion_time[pidx] = now, ++done;
        }
        else {
            if(idx < n)
                now = processes[idx].arv;
        }
    }

    printGanttChart(gantt_chart);

    cout << "\n----------------------------------------------------\n" << left
         << setw(14) << "Tien trinh" << setw(24) << "| TG luu lai he thong" << "| TG cho\n"
         << "--------------|-----------------------|-------------\n";

    double sum = 0.0;
    for(uint32_t i = 0; i < n; ++i) {
        const uint32_t alive = completion_time[i] - processes[i].arv;
        const uint32_t wait = alive - processes[i].cpu;
        sum += wait;

        cout << left << setw(14) << "P" + to_string(processes[i].id)
             << "| " << setw(22) << to_string(completion_time[i]) + " - " + to_string(processes[i].arv) + " = " + to_string(alive)
             << "| " << to_string(alive) + " - " + to_string(processes[i].cpu) + " = " + to_string(wait) + "\n";
    }

    cout << "\nThoi gian cho trung binh: " << fixed << setprecision(2) << sum / n << endl;
}
