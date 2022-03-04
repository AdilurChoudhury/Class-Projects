#pragma once
#include <list>
#include <string>

namespace T {
	class TaskControlBlock {
	private:
		int taskNumber;
		std::string taskName;
		int CPU_time;
		int Turnaround_Time;
		int priority;
		int start_tick;
		int end_tick;
		int burstTime;
		int fileNumber;
		bool isDone;
	public:
		TaskControlBlock(int, std::string, int, int, int);
		
		void setTaskNumber(int);
		void setTaskName(std::string);
		void setCPUTime(int);
		void setTurnaroundTime(int);
		void setPriority(int);
		void setStartTick(int);
		void setEndTick(int);
		void setBurstTime(int);

		int getTaskNumber();
		std::string getTaskName();
		int getCPUTime();
		int getTurnaroundTime();
		int getPriority();
		int getStartTick();
		int getEndTick();
		int getBurstTime();
		

		bool isProcessDone();
	};

	class TaskScheduler {
	private:
		std::list<TaskControlBlock*> readyQueue;
		int fileNumber;
	public:
		TaskScheduler();
		double calculateAvgTT(std::list<TaskControlBlock*>);
		int calculateTotalCPUTime(std::list<TaskControlBlock*>);
		int getFileNumber();
		void FCFS();
		void roundRobin();
		void runPriority();
		void SJF();
		void printTasks(std::string, std::list<TaskControlBlock*>, double, int);
		void writeToFile(std::list<std::string>);
	};
}