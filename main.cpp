#include <string>
#include <vector>

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
    
    Queue(string id, int slice, string policy){
        QID = id;
        timeSlice = slice;
        schedulingPolicy = policy;
    }
};





