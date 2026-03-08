#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

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
    ifstream input;
    input.open(filename);
    if(!input){
        cout << "Cannot open file!\n";
        return;
    }
    int n;
    input >> n;
    input.ignore();
    Q.resize(3);
    for(int i = 0; i < 3; i++){
        getline(input, Q[i].QID, ' ');
        input >> Q[i].timeSlice;
        input.ignore();
        getline(input, Q[i].schedulingPolicy);
    }
    P.resize(5);
    for(int i = 0; i < 5; i++){
        getline(input, P[i].PID, ' ');
        input >> P[i].arrivalTime;
        input >> P[i].burstTime;
        input.ignore();
        getline(input, P[i].queueID);
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

void runSRTN(vector<Process>& P, Queue& Q) {
    int currentTime = 0;
    int completedCount = 0;
    int n = P.size();

    int expectedProcesses = 0;
    for (int i = 0; i < n; i++) {
        if (P[i].queueID == Q.QID) {
            expectedProcesses++;
        }
    }

    while (completedCount < expectedProcesses) {
        
        for (int i = 0; i < n; i++) {
            if (P[i].arrivalTime == currentTime && P[i].queueID == Q.QID) {
                Q.readyQueue.push_back(&P[i]);
            }
        }

        int bestIndex = findShortestProcess(Q.readyQueue);
        if (bestIndex != -1) {
            Process* currentProcess = Q.readyQueue[bestIndex];
            currentProcess->remainingTime--;

            if (currentProcess->remainingTime == 0) {
                currentProcess->completionTime = currentTime + 1;
                currentProcess->turnaroundTime = currentProcess->completionTime - currentProcess->arrivalTime;
                currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->burstTime;
                currentProcess->isCompleted = true;
                
                completedCount++;
                Q.readyQueue.erase(Q.readyQueue.begin() + bestIndex);
            }
        }
        currentTime++;
    }
}

void runSJF(vector<Process>& P, Queue& Q) {
    int currentTime = 0;
    int completedCount = 0;
    int n = P.size();
    Process* activeProcess = nullptr;

    int expectedProcesses = 0;
    for (int i = 0; i < n; i++) {
        if (P[i].queueID == Q.QID) {
            expectedProcesses++;
        }
    }

    while (completedCount < expectedProcesses) {
        
        for (int i = 0; i < n; i++) {
            if (P[i].arrivalTime == currentTime && P[i].queueID == Q.QID) {
                Q.readyQueue.push_back(&P[i]);
            }
        }

        if (activeProcess == nullptr && !Q.readyQueue.empty()) {
            int bestIndex = findShortestProcess(Q.readyQueue);
            if (bestIndex != -1) {
                activeProcess = Q.readyQueue[bestIndex];
                Q.readyQueue.erase(Q.readyQueue.begin() + bestIndex);
            }
        }


        if (activeProcess != nullptr) {
            activeProcess->remainingTime--;

            if (activeProcess->remainingTime == 0) {
                activeProcess->completionTime = currentTime + 1;
                activeProcess->turnaroundTime = activeProcess->completionTime - activeProcess->arrivalTime;
                activeProcess->waitingTime = activeProcess->turnaroundTime - activeProcess->burstTime;
                activeProcess->isCompleted = true;
                
                completedCount++;
                activeProcess = nullptr;
            }
        }
        currentTime++;
    }
}

int main(){
    vector<Queue> Q;
    vector<Process> P;

    ReadFile("input.txt", Q, P);

    for(int i = 0; i < 3; i++){
        cout << Q[i].QID << " " << Q[i].timeSlice << " " << Q[i].schedulingPolicy << endl;
    }
    for(int i = 0; i < 5; i++){
        cout << P[i].PID << " " << P[i].arrivalTime << " " << P[i].burstTime << " " << P[i].queueID << endl;
    }
    for(int i = 0; i < Q.size(); i++) {
        cout << Q[i].QID << " " << Q[i].schedulingPolicy << endl;
        
        if (Q[i].schedulingPolicy == "SRTN") {
            runSRTN(P, Q[i]);
        } 
        else if (Q[i].schedulingPolicy == "SJF") {
            runSJF(P, Q[i]);
        }
        else {
            cout << "Loi: Khong chon duoc" << endl;
        }
    }

    return 0;
}
