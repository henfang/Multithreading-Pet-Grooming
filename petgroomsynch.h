#ifndef PETGROOMSYNCH_H_
#define PETGROOMSYNCH_H_

// pet_t type
typedef enum {None, cat, dog, other} pet_t;

// Thread argument passing struct
typedef struct
{
    pet_t pet_type;
    int sleep_sec;
    char * thread_string;
} myarg_t;

// Thread return struct
typedef struct
{
    pet_t pet;
} myret_t;

// Function definitions
int petgroom_init(int numstations);
int newpet(pet_t pet);
int petdone(pet_t pet);
int petgroom_done();
void print_stations_state();
int open_station();
int check_change_pet_type(pet_t pet);

#endif