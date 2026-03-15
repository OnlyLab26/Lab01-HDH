#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <iomanip>
#include <fstream>

using namespace std;

typedef struct TimeLog{
    int start, end;
    string QueueID, ProID;
} TimeLog;

struct Process {
    string PID;
    int arrivalTime;
    int burstTime;
    string queueID;

    int remainingTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    bool isCompleted;
    Process(){
        PID = "";
        arrivalTime = 0;
        burstTime = 0;
        queueID = "";

        remainingTime = 0;
        completionTime = 0;
        turnaroundTime = 0;
        waitingTime = 0;
        isCompleted = false;
    }

    Process(string id, int arrival, int burst, string qid) {
        PID = id;
        arrivalTime = arrival;
        burstTime = burst;
        queueID = qid;

        remainingTime = burst;
        completionTime = 0;
        turnaroundTime = 0;
        waitingTime = 0;
        isCompleted = false;
    }   
};

struct Queue {
    string QID;
    int timeSlice;
    string schedulingPolicy;
    vector <Process*> readyQueue;
    Queue(){
        QID = "";
        timeSlice = 0;
        schedulingPolicy = "";
    }
    
    Queue(string id, int slice, string policy){
        QID = id;
        timeSlice = slice;
        schedulingPolicy = policy;
    }
};

typedef struct Queue Queue;
typedef struct Process Process;

void ReadFile(string filename, vector<Queue> &Q, vector<Process> &P){
    ifstream input(filename);
    if(!input){
        cout << "Cannot open file!\n";
        return;
    }
    
    int numQueues;
    
    input >> numQueues;
    Q.resize(numQueues);
    for(int i = 0; i < numQueues; i++){
        input >> Q[i].QID >> Q[i].timeSlice >> Q[i].schedulingPolicy;
    }

    Process tempP;
    while (input >> tempP.PID >> tempP.arrivalTime >> tempP.burstTime >> tempP.queueID) {
        tempP.remainingTime = tempP.burstTime;
        tempP.isCompleted = false;
        
        P.push_back(tempP); 
    }
    
    input.close();
}

int findShortestProcess(const vector<Process*>& readyQueue) {
    if (readyQueue.empty()) return -1; 
    int minIndex = 0; 
    for (int i = 1; i < readyQueue.size(); i++) {
        if (readyQueue[i]->remainingTime < readyQueue[minIndex]->remainingTime) {
            minIndex = i;
        } else if (readyQueue[i]->remainingTime == readyQueue[minIndex]->remainingTime) {
            if (readyQueue[i]->arrivalTime < readyQueue[minIndex]->arrivalTime) {
                minIndex = i;
            }
        }
    }
    return minIndex;
}

void runSRTN(vector<Process>& P, Queue& Q, int& globalTime, vector<TimeLog>& Chart) {
    int timeSpent = 0;
    int n = P.size();

    while (timeSpent < Q.timeSlice) {

        int remaining = 0;
        for(int i = 0; i < P.size(); i++) if(P[i].queueID == Q.QID && !P[i].isCompleted) remaining++;
        if(remaining == 0) break;

        for (int i = 0; i < n; i++) {
            if (P[i].arrivalTime <= globalTime && P[i].queueID == Q.QID && !P[i].isCompleted) {
                bool existed = false;
                for(int j = 0; j < Q.readyQueue.size(); j++) if(P[i].PID == Q.readyQueue[j]->PID) existed = true;
                if(!existed) Q.readyQueue.push_back(&P[i]);
            }
        }
        int bestIndex = findShortestProcess(Q.readyQueue);
        if (bestIndex == -1) {
            break; 
        }

        Process* currentProcess = Q.readyQueue[bestIndex];

        if(Chart.empty() || (Chart.back().ProID != currentProcess->PID)){
            Chart.push_back({globalTime, globalTime + 1, Q.QID, currentProcess->PID});
        }
        else Chart.back().end = globalTime + 1;
        
        currentProcess->remainingTime--;
        globalTime++;
        timeSpent++;
        if (currentProcess->remainingTime == 0) {
            currentProcess->completionTime = globalTime;
            currentProcess->turnaroundTime = currentProcess->completionTime - currentProcess->arrivalTime;
            currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->burstTime;
            currentProcess->isCompleted = true;
            Q.readyQueue.erase(Q.readyQueue.begin() + bestIndex);
        }
    }
}

void runSJF(vector<Process>& P, Queue& Q, int& globalTime, vector<TimeLog>& Chart) {
    int timeSpent = 0;
    int n = P.size();

    Process* activeProcess = nullptr;
    while (timeSpent < Q.timeSlice) {

        int remaining = 0;
        for(int i = 0; i < P.size(); i++) {
            if(P[i].queueID == Q.QID && !P[i].isCompleted) remaining++;
        }
        if(remaining == 0) break;
        
        for (int i = 0; i < n; i++) {
            if (P[i].arrivalTime <= globalTime && P[i].queueID == Q.QID && !P[i].isCompleted) {
                bool existed = false;
                for(int j = 0; j < Q.readyQueue.size(); j++) if(P[i].PID == Q.readyQueue[j]->PID) existed = true;
                if (activeProcess != nullptr && activeProcess->PID == P[i].PID) existed = true;
                if(!existed) Q.readyQueue.push_back(&P[i]);
            }
        }

        if (activeProcess == nullptr && !Q.readyQueue.empty()) {
            int bestIndex = findShortestProcess(Q.readyQueue);
            if (bestIndex != -1) {
                activeProcess = Q.readyQueue[bestIndex];
                Q.readyQueue.erase(Q.readyQueue.begin() + bestIndex);
            }
        }
    
        if (activeProcess == nullptr) {
            break; 
        }

        if(Chart.empty() || (Chart.back().ProID != activeProcess->PID)){
            Chart.push_back({globalTime, globalTime + 1, Q.QID, activeProcess->PID});
        }
        else Chart.back().end = globalTime + 1;

        activeProcess->remainingTime--;
        globalTime++;
        timeSpent++;

        if (activeProcess->remainingTime == 0) {
                activeProcess->completionTime = globalTime;
                activeProcess->turnaroundTime = activeProcess->completionTime - activeProcess->arrivalTime;
                activeProcess->waitingTime = activeProcess->turnaroundTime - activeProcess->burstTime;
                activeProcess->isCompleted = true;
                activeProcess = nullptr;
            }
        }
    if(activeProcess != nullptr && !activeProcess->isCompleted){
        Q.readyQueue.push_back(activeProcess);
    }
}

void WriteFile(string fileName, vector<TimeLog> Chart, vector<Process> P){
    ofstream output;
    output.open(fileName);
    output << "================== CPU SCHEDULING DIAGRAM ==================\n\n";
    output << "[Start - End]\t" << "Queue\t" << "Process\n";
    output << "------------------------------------\n";
    for(int i = 0; i < Chart.size(); i++){
        if(i < 3) output << "[" << Chart[i].start << " - " << Chart[i].end << "]\t\t\t" << Chart[i].QueueID << "\t\t" << Chart[i].ProID << endl;
        else output << "[" << Chart[i].start << " - " << Chart[i].end << "]\t\t" << Chart[i].QueueID << "\t\t" << Chart[i].ProID << endl;
    }
    output << endl;
    output << "================ PROCESS STATISTICS ================\n\n";
    output << "Process\t\tArrival\t\tBurst\t\tCompletion\t\tTurnaround\t\tWaiting\n";
    output << "------------------------------------------------------------------------------\n";
    float turnAverage = 0, waitAverage = 0;
    for(int i = 0; i < P.size(); i++){
        output << P[i].PID << "\t\t\t" << P[i].arrivalTime << "\t\t\t" << P[i].burstTime << "\t\t\t" << P[i].completionTime << "\t\t\t\t" << P[i].turnaroundTime << "\t\t\t\t" << P[i].waitingTime << endl;
        turnAverage += P[i].turnaroundTime;
        waitAverage += P[i].waitingTime;
    }
    output << "------------------------------------------------------------------------------\n";
    output << "Average Turnaround Time : " << fixed << setprecision(1) << turnAverage / P.size() << endl;
    output << "Average Waiting Time    : " << fixed << setprecision(1) << waitAverage / P.size() << endl << endl << endl;
    output << "====================================================";
    output.close();
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        cout << "Sai cu phap" << endl;
        return 1;
    }

    vector<Queue> Q;
    vector<Process> P;

    string inputFile = argv[1];
    string outputFile = argv[2];

    ReadFile(inputFile, Q, P);
    
    vector<TimeLog> Chart;
    int globalTime = 0;
    
    while (true) {
        bool allCompleted = true;
        for (const auto& p : P) {
            if (!p.isCompleted) {
                allCompleted = false;
                break;
            }
        }
        if (allCompleted) break;

        bool cpuIdleThisCycle = true;

        for (int i = 0; i < Q.size(); i++) {
            int timeBeforeQueue = globalTime; 
            
            if (Q[i].schedulingPolicy == "SRTN") {
                runSRTN(P, Q[i], globalTime, Chart);
            } 
            else if (Q[i].schedulingPolicy == "SJF") {
                runSJF(P, Q[i], globalTime, Chart);
            }

            if (globalTime > timeBeforeQueue) {
                cpuIdleThisCycle = false;
            }
        }

        if (cpuIdleThisCycle) {
            if(Chart.empty() || Chart.back().ProID != "IDLE"){
                Chart.push_back({globalTime, globalTime + 1, "SYS", "IDLE"});
            } else {
                Chart.back().end = globalTime + 1;
            }
            globalTime++;
        }
    }
    
    WriteFile(outputFile, Chart, P);
    
    return 0;
}