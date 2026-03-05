#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct Process {
    // Biến tĩnh(nạp từ input.txt)
    string PID;
    int arrivalTime;
    int burstTime;
    string queueID;
    // Biến động trong quá trình thực thi
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
        getline(input, Q[i].schedulingPolicy);
    }
    P.resize(5);
    for(int i = 0; i < 5; i++){
        getline(input, P[i].PID, ' ');
        input >> P[i].arrivalTime;
        input >> P[i].burstTime;
        getline(input, P[i].queueID);
    }
    input.close();
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
    return 0;
}