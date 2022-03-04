#include <iostream>
#include "Scheduler.h"
#include <string>
#include <list>
#include <time.h>
#include <vector>
#include <fstream>

using namespace T;
using namespace std;

TaskControlBlock::TaskControlBlock(int taskNum, string name, int timeCPU, int p, int bT) {
	taskNumber = taskNum;
	taskName = name;
	CPU_time = timeCPU;
	priority = p;
	burstTime = bT;
	isDone = false;
}

void TaskControlBlock::setTaskNumber(int taskNum) {
	taskNumber = taskNum;
}

void TaskControlBlock::setTaskName(string name) {
	taskName = name;
}

void TaskControlBlock::setCPUTime(int timeCPU) {
	CPU_time = timeCPU;
}

void TaskControlBlock::setTurnaroundTime(int TT) {
	Turnaround_Time = TT;
}

void TaskControlBlock::setPriority(int p) {
	priority = p;
}

void TaskControlBlock::setStartTick(int st) {
	start_tick = st;
}

void TaskControlBlock::setEndTick(int et) {
	end_tick = et;
}

void TaskControlBlock::setBurstTime(int bT) {
	burstTime = bT;
}

int TaskControlBlock::getTaskNumber() {
	return taskNumber;
}

string TaskControlBlock::getTaskName() {
	return taskName;
}

int TaskControlBlock::getCPUTime() {
	return CPU_time;
}

int TaskControlBlock::getTurnaroundTime() {
	return Turnaround_Time;
}

int TaskControlBlock::getPriority() {
	return priority;
}

int TaskControlBlock::getStartTick() {
	return start_tick;
}

int TaskControlBlock::getEndTick() {
	return end_tick;
}

int TaskControlBlock::getBurstTime() {
	return burstTime;
}

bool TaskControlBlock::isProcessDone() {
	isDone = CPU_time <= 0;
	return isDone;
}

TaskScheduler::TaskScheduler() {
	cout << "Input File Number to output data to (1 - 10): ";
	cin >> fileNumber;

	list<TaskControlBlock*> * k_readyQueue = new list<TaskControlBlock*>;
	vector<int> randomPriority;
	srand(time(NULL));
	for (int i = 0; i < 10; i++) {
		int tmpPriority = rand() % 10 + 1;
		if (i == 0) {
			randomPriority.push_back(tmpPriority);
			continue;
		}
		else {
			auto repeat = find(randomPriority.begin(), randomPriority.end(), tmpPriority);
			while (repeat != randomPriority.end()) {
				tmpPriority = rand() % 10 + 1;
				repeat = find(randomPriority.begin(), randomPriority.end(), tmpPriority);
			}
			randomPriority.push_back(tmpPriority);
		}
	}

	for (int i = 1; i <= 10; i++) {
		
		//Random CPU Time generated from 10ms to 100ms
		int randomCPUTime = rand() % 120  + 10;
		int randomBurstTime = rand() % 50 + 10;
		int randPriority = randomPriority.front();
		randomPriority.erase(randomPriority.begin());
		string taskName = "T" + to_string(i);
		TaskControlBlock * task = new TaskControlBlock(i, taskName, randomCPUTime, randPriority, randomBurstTime);
		k_readyQueue->push_back(task);
	}
	readyQueue.assign(k_readyQueue->begin(), k_readyQueue->end());
}

int TaskScheduler::getFileNumber() {
	return fileNumber;
}

void TaskScheduler::FCFS() {
	list<TaskControlBlock*> tempQueue;
	tempQueue.assign(readyQueue.begin(), readyQueue.end());
	int ticks = 0;
	for (auto TCB : tempQueue) {
		TCB->setStartTick(ticks);
		ticks += TCB->getCPUTime();
		TCB->setEndTick(ticks - 1);
		TCB->setTurnaroundTime(ticks);
	}
	double averageTT = calculateAvgTT(tempQueue);
	int totalCPUTime = calculateTotalCPUTime(tempQueue);
	
	printTasks("FCFS", tempQueue, averageTT, totalCPUTime);
}

void TaskScheduler::roundRobin() {
	list<TaskControlBlock*> tempQueue;
	tempQueue.assign(readyQueue.begin(), readyQueue.end());
	const int quantum = 10;
	bool isAllDone = false;
	int ticks = 0;
	int timeLeft = calculateTotalCPUTime(tempQueue);
	int originalTotalTime = timeLeft;
	cout << "Method: RR (q = 10)" << endl;
	cout << "TaskNumber	TaskName	CPU_Time	Turnaround_Time	Start_Tick	End_Tick" << endl;
	while (!isAllDone) {
		for (auto TCB : tempQueue) {
			if (!TCB->isProcessDone()) {
				TCB->setStartTick(ticks);
				TCB->setCPUTime(TCB->getCPUTime() - quantum);
				ticks += quantum;
				TCB->setTurnaroundTime(ticks);
				TCB->setEndTick(TCB->getTurnaroundTime() - 1);
				cout << to_string(TCB->getTaskNumber()) << "\t\t" << TCB->getTaskName() << "\t\t" << to_string(TCB->getCPUTime()) << "\t\t" << to_string(TCB->getTurnaroundTime()) << "\t\t" << to_string(TCB->getStartTick()) << "\t\t" << to_string(TCB->getEndTick()) << endl;
			}
		}
		timeLeft = calculateTotalCPUTime(tempQueue);
		if (timeLeft <= 0) {
			isAllDone = true;
		}
	}
	cout << endl;

	double avgTT = calculateAvgTT(tempQueue);

	printTasks("RR", tempQueue, avgTT, originalTotalTime);
}

void TaskScheduler::runPriority() {
	list<TaskControlBlock*> tempQueue;
	for (int i = 1; i <= 10; i++) {
		for (auto TCB : readyQueue) {
			if (TCB->getPriority() == i) {
				tempQueue.push_back(TCB);
				break;
			}
		}
	}
	int ticks = 0;

	for (auto TCB : tempQueue) {
		TCB->setStartTick(ticks);
		ticks += TCB->getCPUTime();
		TCB->setTurnaroundTime(ticks);
		TCB->setEndTick(ticks - 1);
	}

	double avgTT = calculateAvgTT(tempQueue);
	int totalCPUTime = calculateTotalCPUTime(tempQueue);

	printTasks("Priority", tempQueue, avgTT, totalCPUTime);
}

void TaskScheduler::SJF() {
	list<TaskControlBlock*> tempQueue;
	list<int> sortedCPUTimes;
	for (auto TCB : readyQueue) {
		sortedCPUTimes.push_back(TCB->getCPUTime());
	}
	
	sortedCPUTimes.sort();
	
	for (int cpuTime : sortedCPUTimes) {
		for (auto TCB : readyQueue) {
			if (TCB->getCPUTime() == cpuTime) {
				tempQueue.push_back(TCB);
				break;
			}
		}
	}
	int ticks = 0;
	for (auto TCB : tempQueue) {
		TCB->setStartTick(ticks);
		ticks += TCB->getCPUTime();
		TCB->setTurnaroundTime(ticks);
		TCB->setEndTick(ticks - 1);
	}

	double avgTT = calculateAvgTT(tempQueue);
	int totalCPUTime = calculateTotalCPUTime(tempQueue);

	printTasks("SJF", tempQueue, avgTT, totalCPUTime);
}

double TaskScheduler::calculateAvgTT(list<TaskControlBlock*> Queue) {
	double avgTT = 0;
	for (auto TCB : Queue) {
		avgTT += TCB->getTurnaroundTime();
	}
	avgTT /= 10;

	return avgTT;
}

int TaskScheduler::calculateTotalCPUTime(list<TaskControlBlock*> Queue) {
	int totalCPUTime = 0;

	for (auto TCB : Queue) {
		totalCPUTime += TCB->getCPUTime();
	}
	return totalCPUTime;
}

void TaskScheduler::printTasks(string method, list<TaskControlBlock*> Queue, double avgTT, int totalCPUTime) {
	list<string> lineStrings;
	string currentLine = "Method: " + method;
	cout << "Method: " + method << endl;
	lineStrings.push_back(currentLine);
	currentLine = "TaskNumber	TaskName	CPU_Time	Turnaround_Time	Start_Tick	End_Tick";
	cout << currentLine << endl;
	lineStrings.push_back(currentLine);
	for (auto TCB : Queue) {
		cout << TCB->getTaskNumber() << "\t\t" << TCB->getTaskName() << "\t\t" << TCB->getCPUTime() << "\t\t" << TCB->getTurnaroundTime() << "\t\t" << TCB->getStartTick() << "\t\t" << TCB->getEndTick() << endl;
		currentLine = to_string(TCB->getTaskNumber()) + "\t\t" + TCB->getTaskName() + "\t\t" + to_string(TCB->getCPUTime()) + "\t\t" + to_string(TCB->getTurnaroundTime()) + "\t\t" + to_string(TCB->getStartTick()) + "\t\t" + to_string(TCB->getEndTick());
		lineStrings.push_back(currentLine);
	}
	currentLine = "Average turnaround time for " + method + " = " + to_string(avgTT);
	cout << "Average turnaround time for " << method << " = " << to_string(avgTT) << endl;
	lineStrings.push_back(currentLine);
	currentLine = "Total CPU Time for " + method + " = " + to_string(totalCPUTime);
	cout << "Total CPU Time for " << method << " = " << to_string(totalCPUTime) << endl;
	lineStrings.push_back(currentLine);
	
	cout << endl;

	writeToFile(lineStrings);
}

void TaskScheduler::writeToFile(list<string> lineStrings) {

	string path = ".\\OutputFile\\output_" + to_string(fileNumber) + ".txt";
	
	ofstream outputFile(path, ofstream::app);
		
	if (outputFile.is_open()) {
		for (string line : lineStrings) {
			outputFile << line << endl;
		}
		outputFile.close();
	}
}

int main() {
	TaskScheduler* taskScheduler = new TaskScheduler();

	cout << "This is a task scheduler program that simulates CPU scheduling algorithms. When prompted choose a number to label the output file. This will happen multiple times, so please be consistent with the number" << endl;
	cout << endl;

	taskScheduler->FCFS();
	taskScheduler->roundRobin();
	taskScheduler->runPriority();
	taskScheduler->SJF();
	
	

	return 0;
}