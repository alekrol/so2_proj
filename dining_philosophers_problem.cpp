#include <stdio.h>
#include <pthread.h>
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

std::vector<Philosopher> philosophers;

void think(Philosopher *philosopher)
{
    philosopher->state = THINKING;
    std::cout << "Philosopher number " << philosopher->philosopherID << " is thinking\n";
}

bool test(Philosopher *philosopher, int numberOfPhilosophers)
{
    // Philosopher can only eat if their neighbors are not eating
    int left = (philosopher->philosopherID + numberOfPhilosophers - 1) % numberOfPhilosophers;
    int right = (philosopher->philosopherID + 1) % numberOfPhilosophers;

    return philosophers[left].state != EATING && philosophers[right].state != EATING;
}

void pickUpChopsticks(Philosopher *philosopher, int numberOfPhilosophers)
{
    std::cout << "Philosopher number " << philosopher->philosopherID << " picks up chopsticks\n";
}

void putDownChopsticks(Philosopher *philosopher, int numberOfPhilosophers)
{
    std::cout << "Philosopher number " << philosopher->philosopherID << " puts down chopsticks\n";
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "No philosophers!" << "\n";
        return 1;
    }

    int numOfPhilosophers = std::stoi(argv[1]);
    std::cout << "Number of philosophers: " << numOfPhilosophers << "\n";

    philosophers.resize(numOfPhilosophers);
    for (int i = 0; i < numOfPhilosophers; ++i)
    {
        philosophers[i].philosopherID = i;
        philosophers[i].timeHungry = 0;
        philosophers[i].state = THINKING;
    }

    for (int i = 0; i < numOfPhilosophers; ++i)
    {
        think(&philosophers[i]);
        pickUpChopsticks(&philosophers[i], numOfPhilosophers);
        putDownChopsticks(&philosophers[i], numOfPhilosophers);
    }

    return 0;
}