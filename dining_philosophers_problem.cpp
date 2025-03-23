#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <ctime>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

struct Philosopher
{
    int philosopherID;
    time_t timeHungry; // Time when philosopher became HUNGRY
    int state;         // THINKING, HUNGRY, EATING
};

void think(Philosopher *philosopher)
{
    philosopher->state = THINKING;
    std::cout << "Philosopher number " << philosopher->philosopherID << " is thinking\n";
}

bool test(Philosopher *philosopher, int numberOfPhilosophers, Philosopher *philosophers)
{
    // Philosopher can only eat if their neighbors are not eating
    int left = (philosopher->philosopherID + numberOfPhilosophers - 1) % numberOfPhilosophers;
    int right = (philosopher->philosopherID + 1) % numberOfPhilosophers;

    return philosophers[left].state != EATING && philosophers[right].state != EATING;
}

void pickUpChopsticks(Philosopher *philosopher, int numberOfPhilosophers, Philosopher *philosophers)
{
    std::cout << "Philosopher number " << philosopher->philosopherID << " picks up chopsticks\n";
}

void putDownChopsticks(Philosopher *philosopher, int numberOfPhilosophers, Philosopher *philosophers)
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

    Philosopher philosophers[numOfPhilosophers];
    Philosopher p1 = {1, time(0), THINKING};
    think(&p1);
    pickUpChopsticks(&p1, numOfPhilosophers, philosophers);
    putDownChopsticks(&p1, numOfPhilosophers, philosophers);

    return 0;
}