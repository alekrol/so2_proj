#ifndef DINING_PHILOSOPHERS_PROBLEM_H
#define DINING_PHILOSOPHERS_PROBLEM_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <vector>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

struct Philosopher
{
    int philosopherID;
    time_t timeHungry; // Time when philosopher became HUNGRY
    int state;         // THINKING, HUNGRY, EATING
};

extern std::vector<Philosopher> philosophers;
extern pthread_mutex_t mMutex;
extern std::vector<pthread_cond_t> conditions;
extern std::ofstream testFile;

void think(Philosopher *philosopher);
bool test(Philosopher *philosopher, int numberOfPhilosophers);
void pickUpChopsticks(Philosopher *philosopher, int numberOfPhilosophers);
void putDownChopsticks(Philosopher *philosopher, int numberOfPhilosophers);
void *philosopherThread(void *arg);

#endif