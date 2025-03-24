#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <unistd.h>

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
pthread_mutex_t mMutex;
std::vector<pthread_cond_t> conditions;

void think(Philosopher *philosopher)
{
    philosopher->state = THINKING;
    // std::cout << "Philosopher number " << philosopher->philosopherID << " is thinking\n";
    // sleep(1);
}

bool test(Philosopher *philosopher, int numberOfPhilosophers)
{
    int left = (philosopher->philosopherID + numberOfPhilosophers - 1) % numberOfPhilosophers;
    int right = (philosopher->philosopherID + 1) % numberOfPhilosophers;

    time_t timeNow = time(0);
    const int ms = 1000;

    // Philosopher can only eat if their neighbors are not eating
    if (philosophers[left].state == EATING || philosophers[right].state == EATING)
        return false;

    // Philosopher can only eat if their neighbors are not hungry for more than 5 ms
    if ((timeNow - philosophers[left].timeHungry) >= 5 && philosophers[left].state == HUNGRY)
        return false;
    if ((timeNow - philosophers[right].timeHungry) >= 5 && philosophers[right].state == HUNGRY)
        return false;

    return true;
}

void pickUpChopsticks(Philosopher *philosopher, int numberOfPhilosophers)
{
    int id = philosopher->philosopherID;

    pthread_mutex_lock(&mMutex);
    philosopher->state = HUNGRY;
    philosopher->timeHungry = time(0);

    int left = (id + numberOfPhilosophers - 1) % numberOfPhilosophers;
    int right = (id + 1) % numberOfPhilosophers;

    while (!test(philosopher, numberOfPhilosophers))
    {
        pthread_cond_wait(&conditions[id], &mMutex);
    }

    philosopher->state = EATING;
    std::cout << "Philosopher number " << philosopher->philosopherID << " is eating\n";
    pthread_mutex_unlock(&mMutex);
}

void putDownChopsticks(Philosopher *philosopher, int numberOfPhilosophers)
{
    int id = philosopher->philosopherID;

    pthread_mutex_lock(&mMutex);
    think(philosopher);
    std::cout << "Philosopher number " << philosopher->philosopherID << " is thinking\n";

    int left = (id + numberOfPhilosophers - 1) % numberOfPhilosophers;
    int right = (id + 1) % numberOfPhilosophers;

    if (test(&philosophers[left], numberOfPhilosophers))
        pthread_cond_signal(&conditions[left]);
    if (test(&philosophers[right], numberOfPhilosophers))
        pthread_cond_signal(&conditions[right]);

    pthread_mutex_unlock(&mMutex);
}

void *philosopherThread(void *arg)
{
    Philosopher *philosopher = (Philosopher *)arg;

    while (true)
    {
        think(philosopher);
        sleep(1);
        pickUpChopsticks(philosopher, philosophers.size());
        sleep(1);
        putDownChopsticks(philosopher, philosophers.size());
    }

    return NULL;
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
    conditions.resize(numOfPhilosophers);
    pthread_mutex_init(&mMutex, NULL);
    std::vector<pthread_t> threads(numOfPhilosophers);

    for (int i = 0; i < numOfPhilosophers; ++i)
    {
        philosophers[i].philosopherID = i;
        philosophers[i].timeHungry = 0;
        philosophers[i].state = THINKING;
        pthread_cond_init(&conditions[i], NULL);
        pthread_create(&threads[i], NULL, philosopherThread, &philosophers[i]);
    }

    for (int i = 0; i < numOfPhilosophers; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mMutex);
    for (int i = 0; i < numOfPhilosophers; ++i)
    {
        pthread_cond_destroy(&conditions[i]);
    }

    return 0;
}
