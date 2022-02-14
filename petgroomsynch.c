#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "petgroomsynch.h"

// enum translation to string
const char* pet_types[] = {"None", "cat", "dog", "other"};
// Remembers how many stations we have
int nstations;
// Remembers if initialization has occurred
int grooming_initialized = 0;
// Lock to avoid multiple pets being processed together
pthread_mutex_t lock;
// Conditional variable to wait for cat_or_dog type, station opening, and continue_accept_current_pet
pthread_cond_t cond;
// Track what animal is at each station in an array
pet_t *station_occupant;
// Cat, dog, or other (neutral) grooming currently being done
pet_t cat_or_dog = other;
// Don't let more of opposing type in tracking variable
// If it is 1 means we haven't seen pet of other type arrive: Keep accepting current type of pet
// If it is 0 means we have seen a pet of the opposing type arrive: No longer accept pets of current type
int continue_accept_current_pet = 1;

/* Initialize the grooming facility with numstations amount of grooming stations.
   Also initializes lock and conditional variable
   Returns -1 on failure and 0 on success 
   param numstations: Number of grooming station we want to have*/
int petgroom_init(int numstations)
{
    if (grooming_initialized == 1)
    {
        printf("Grooming facility already initialized!\n");
        return -1;
    }
    if (numstations <= 0) {
        printf("Cannot initialize empty or negative facility\n");
        return -1;
    }
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Lock init failed\n");
        return -1;
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        printf("Cond init failed\n");
        return -1;
    }
    nstations = numstations;

    station_occupant = (pet_t *) malloc(sizeof(pet_t) * (numstations + 1));

    int i;
    for (i = 0; i < nstations; i++) {
        station_occupant[i] = None;
    }
    

    grooming_initialized = 1;
    printf("Grooming facility initialization success with %d stations!\n", numstations);
    return 0;
}

/* Handles a new pet thread arriving at the facility. Checks for various states that the facility could be in
   and blocks or allows pets to be groomed. Prints different messages for different handling of pets for testing.
   param pet: The type of pet that has just arrived to be groomed*/
int newpet(pet_t pet)
{
    if (grooming_initialized == 0) {
        printf("Grooming facility is uninitialized!\n");
        return -1;
    }
    // Attempt to acquire lock
    pthread_mutex_lock(&lock);
    // There are no cats or dogs and the new pet is a cat or dog we set the current pet type and start grooming
    if (cat_or_dog == other && pet != other)
    {
        // Message to alert when thread arrives
        printf("Pet of type: %s has arrived for grooming. Currently only other types are in\n", pet_types[pet]);

        // Check for station opening
        while (!open_station()) {
            pthread_cond_wait(&cond, &lock);
        }

        // Message to alert when unblocked
        printf("Pet of type: %s has been sent for grooming. Currrently only other types are in\n", pet_types[pet]);

        // Set new pet type to be groomed
        cat_or_dog = pet;

        // Find station for pet
        int i;
        for (i = 0; i < nstations; i++)
        {
            if (station_occupant[i] == None)
            {
                station_occupant[i] = pet;
                pthread_mutex_unlock(&lock);
                return 0;
            }
        } 
    }
    // When the pet is of "other" type we simply look for a open spot
    else if (pet == other) {

        // Message to alert when thread arrives
        printf("Pet of type: %s has arrived for grooming\n", pet_types[pet]);

        // Check for station opening
        while (!open_station()) {
            pthread_cond_wait(&cond, &lock);
        }

        // Message to alert when unblocked
        printf("Pet of type: %s has been sent for grooming\n", pet_types[pet]);

        // Find station for pet
        int i;
        for (i = 0; i < nstations; i++)
        {
            if (station_occupant[i] == None)
            {
                station_occupant[i] = pet;
                pthread_mutex_unlock(&lock);
                return 0;
            }
        } 
    }
    // If there are others of the same species we can groom on station opening and if there are no pets of the other type waiting
    else if (cat_or_dog == pet && continue_accept_current_pet)
    {
        // Message to alert when thread arrives
        printf("Pet of type: %s has arrived for grooming. We are currently grooming this type\n", pet_types[pet]);

        // Check for station opening and continue_accept_current_pet
        while (!open_station()) {
            pthread_cond_wait(&cond, &lock);
        }

        // Set new pet type
        cat_or_dog = pet;

        // Message to alert when unblocked
        printf("Pet of type: %s has been sent for grooming. We are currently grooming this type\n", pet_types[pet]);

        // Find station for pet
        int i;
        for (i = 0; i < nstations; i++)
        {
            if (station_occupant[i] == None)
            {
                station_occupant[i] = pet;
                pthread_mutex_unlock(&lock);
                return 0;
            }
        }
    }
    // If we are currently not grooming any new pets of type cat_or_dog because a pet of the other type is waiting
    else if (cat_or_dog == pet && !continue_accept_current_pet) {

        // Message to alert when thread arrives
        printf("Pet of type: %s has arrived for grooming. However pet of opposite type is waiting\n", pet_types[pet]);

        while (!open_station() || !continue_accept_current_pet || (cat_or_dog != other && cat_or_dog != pet)) {
            pthread_cond_wait(&cond, &lock);
        }

        // Set new pet type
        cat_or_dog = pet;

        // Message to alert when unblocked
        printf("Pet of type: %s has been sent for grooming. Done waiting for opposite type\n", pet_types[pet]);

        // Find station for pet
        int i;
        for (i = 0; i < nstations; i++)
        {
            if (station_occupant[i] == None)
            {
                station_occupant[i] = pet;
                pthread_mutex_unlock(&lock);
                return 0;
            }
        }
    }
    // Wait for all the pets of the opposing species to leave
    // Set continue_accept_current_pet to 0
    // If we see a opposing type of pet arrive we stop accepting the current kind being groomed to satisfy RULE 3
    else
    {
        // Message to alert when thread arrives
        printf("Pet of type: %s has arrived for grooming. This is the opposite type of what is currently being groomed\n", pet_types[pet]);

        continue_accept_current_pet = 0;
        // // Attempt to acquire lock
        // pthread_mutex_lock(&lock);
        // Check for neutral cat_or_dog value and for an open station
        while (cat_or_dog != other || !open_station()) {
            pthread_cond_wait(&cond, &lock);
        }

        // Message to alert when unblocked
        printf("Pet of type: %s has been sent for grooming. The is the opposite type of what was being groomed\n", pet_types[pet]);

        // Set new pet type
        cat_or_dog = pet;

        // Find station for pet
        int i;
        for (i = 0; i < nstations; i++)
        {
            if (station_occupant[i] == None)
            {
                station_occupant[i] = pet;
                // We can now consider accepting the current pet type again
                continue_accept_current_pet = 1;
                pthread_mutex_unlock(&lock);
                return 0;
            }
        }
        return 0;
    }
    return -1;
}

/* Called when a pet has finished grooming. Removes the pet from the facility and checks to see if
   we need to change what kind of pet can now be groomed by calling check_change_pet_type().
   Also prints out messages for visual inspection. Returns 0 on success and -1 on failure.
   param pet: The type of pet that has finished grooming*/
int petdone(pet_t pet)
{
    pthread_mutex_lock(&lock);
    if (grooming_initialized == 0) {
        printf("Grooming facility is uninitialized!\n");
        pthread_mutex_unlock(&lock);
        return -1;
    }
    // Find first station occuppied by pet of type pet and clear it
    // Does not maintain the locations of particular pets, but that shouldn't matter
    int i;
    for (i = 0; i < nstations; i++) {
        if (station_occupant[i] == pet) {
            // Clear station of occupant
            station_occupant[i] = None;
            // Check to see if the current type of pet (dog or cat) needs to be changed
            check_change_pet_type(pet);
            // Message to alert when finished grooming
            printf("Pet of type: %s has returned from grooming\n", pet_types[pet]);
            printf("%s's being groomed\n", pet_types[cat_or_dog]);
            pthread_mutex_unlock(&lock);
            return 0;
        }
    }
    // Couldn't find pet of type pet
    printf("Couldn't find pet\n");
    pthread_mutex_unlock(&lock);
    return -1;
}

/* Uninitializes the grooming facility. If successful returns 0. Otherwise returns -1 */
int petgroom_done()
{
    // If the facility is initialized we can uninitialize it
    if (grooming_initialized == 1)
    {
        grooming_initialized = 0;
        nstations = 0;
        cat_or_dog = other;

        free(station_occupant);
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
        printf("Grooming deinitialization success!\n");
        return 0;
    }
    // The facility is uninitialized
    printf("Grooming facility is already uninitialized.\n");
    return -1;
}

/* A function that prints the current state of the grooming facility. To be used in the thread for error checking.
   param thread: A string that indicates which thread we are running on */
void print_stations_state(char * thread) {
    pthread_mutex_lock(&lock);
    printf("\n-------------\n");
    // Print thread identifier
    printf("On Thread: %s\n", thread);

    // Print grooming initialized or uninitialized
    if (grooming_initialized) {
        printf("Grooming Initialized\n");
    }
    else {
        printf("Grooming Uninitialized\n");
    }

    // Print all station states
    printf("Currently grooming: %s\n\n", pet_types[cat_or_dog]);
    int i;
    for (i = 0; i < nstations; i++) {
        printf("Station %d occupied by %s\n", i+1, pet_types[station_occupant[i]]);
    }
    printf("-------------\n");
    pthread_mutex_unlock(&lock);
}

// Returns 1 if a open grooming station exists. 0 otherwise
int open_station() {
    int i;
    for (i = 0; i < nstations; i++) {
        if (station_occupant[i] == None) {
            pthread_cond_signal(&cond);
            return 1;
        }
    }
    return 0;
}

/* Checks grooming facility for pets of type pet. If the pet is of type other we do nothing and return 0.
   If there exists a pet of type pet we also do nothing and return 0. 
   If there isn't a pet of type pet we set cat_or_dog variable to the neutral state of other and return 1*/
   
int check_change_pet_type(pet_t pet) {
    // If pet is of other type we don't care
    if (pet == other) {
        return 0;
    }

    // Check all stations for pets of type pet
    // If there remains any pets of type pet we do nothing
    int i;
    for (i = 0; i < nstations; i++) {
        if (station_occupant[i] == pet) {
            return 0;
        }
    }
    // If we reach here there are no pets of type pet remaining in any station
    // Since we only call this in petdone this means that we can now switch back to a neutral state
    cat_or_dog = other;
    pthread_cond_signal(&cond);
    return 1;
}