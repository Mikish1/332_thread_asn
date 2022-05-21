// Michael Kish

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "petgroomsynch.h"


int num_stations;                   // Total number of stations
int cats;                           // Number of cats being groomed
int dogs;                           // Number of dogs being groomed
int dog_yield;
int cat_yield;
int initialized;
pthread_mutex_t *mutex;              // First lock
pthread_mutex_t *mutex2;             // Second lock
pthread_cond_t *avail_station;       // Condition for number of stations
pthread_cond_t *num_cats;            // Condition for number of cats
pthread_cond_t *num_dogs;            // Condition for number of dogs
pthread_cond_t *stop_dogs;
pthread_cond_t *stop_cats;


int petgroom_init(int numstations){
    // Check for a valid number of stations.
    if (numstations <= 0){
        printf("Invalid number of stations.\n");
        return -1;
    }

    // Check if already initialized.
    if (initialized == 1){
        printf("Already initialized.\n");
        return -1;
    }

    // initialize all mutexes, condition variables, and other variables.
    initialized = 1;
    num_stations = numstations;
    cats = 0;
    dogs = 0;
    dog_yield = 0;
    cat_yield = 0;
    pthread_mutex_init(mutex, NULL);
    pthread_mutex_init(mutex2, NULL);
    pthread_cond_init(avail_station, NULL);
    pthread_cond_init(num_cats, NULL);
    pthread_cond_init(num_dogs, NULL);
    pthread_cond_init(stop_dogs, NULL);
    pthread_cond_init(stop_cats, NULL);
    return 0;
}


int newpet(pet_t pet){
    if (initialized != 1){
        printf("Stations not initialized.\n");
        return -1;
    }

    pthread_mutex_lock(mutex2);
    // If the pet is a cat, check for any dogs and wait until there are none.
    if (pet == cat){
        if (cat_yield >= 2){
            pthread_cond_wait(stop_cats, mutex2);
        }
        while (dogs > 0){
            pthread_cond_wait(num_cats, mutex2);
        }
        if (dog_yield >= 2){
            dog_yield = 0;
            pthread_cond_signal(stop_dogs);
        }
        cats = cats + 1;
        cat_yield++;
    }

    // If the pet is a dog, check for any cats and wait until there are none.
    if (pet == dog){
        if (dog_yield >= 2){
            pthread_cond_wait(stop_dogs, mutex2);
        }
        while (cats > 0){
            pthread_cond_wait(num_dogs, mutex2);
        }
        if (cat_yield >= 2){
            cat_yield = 0;
            pthread_cond_signal(stop_cats);
        }
        dogs = dogs + 1;
        dog_yield++;
    }
    pthread_mutex_unlock(mutex2);

    // Check if there are any available stations, wait until one is if not.
    pthread_mutex_lock(mutex);
    while (num_stations == 0){
        pthread_cond_wait(avail_station, mutex);
    }

    num_stations--;
    pthread_mutex_unlock(mutex);
    return 0;
}


int petdone(pet_t pet){
    if (initialized != 1){
        printf("Stations not initialized.\n");
        return -1;
    }

    pthread_mutex_lock(mutex2);
    // check if the pet is a cat and update the number of cats.
    if (pet == cat){
        cats = cats - 1;
        // If there are no cats, signal any waiting dogs.
        if (cats == 0){
            pthread_cond_signal(num_dogs);
        }
    }

    // check if the pet is a dog and update the number of dogs.
    if (pet == dog){
        dogs = dogs - 1;
        // If there are no dogs, signal any waiting cats.
        if (dogs == 0){
            pthread_cond_signal(num_cats);
        }
    }
    pthread_mutex_unlock(mutex2);

    // Update the number of stations and signal any waiting threads.
    pthread_mutex_lock(mutex);
    num_stations++;
    pthread_cond_signal(avail_station);
    pthread_mutex_unlock(mutex);
    return 0;
}


int petgroom_done() {
    if (initialized != 1) {
        printf("Invalid call to pet_groom_done.\n");
        return -1;
    }

    // Destroy all mutexes and condition variables, reset other variables to default values.
    pthread_cond_destroy(avail_station);
    pthread_cond_destroy(num_dogs);
    pthread_cond_destroy(num_cats);
    pthread_cond_destroy(stop_dogs);
    pthread_cond_destroy(stop_cats);
    pthread_mutex_destroy(mutex);
    pthread_mutex_destroy(mutex2);

    num_stations = 0;
    cats = 0;
    dogs = 0;
    initialized = 0;
    return 0;
}
