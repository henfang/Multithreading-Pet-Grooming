#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "petgroomsynch.h"

/* A thread that represents a single pet that should be groomed 
   param arg: a struct of type myarg_t that holds the pet type of pet_t, how long to sleep for, and a thread identifier*/

void *pet_thread(void *arg)
{
    myret_t *rvals = malloc(sizeof(myret_t));

    rvals->pet = ((myarg_t*) arg)->pet_type;

    // Sends pet for grooming
    newpet(rvals->pet);

    // Checks state of grooming stations
    print_stations_state(((myarg_t*) arg)->thread_string);
    
    // Sleep for amount of time
    // Simulates grooming time
    sleep(((myarg_t*) arg)->sleep_sec);

    // Finish grooming
    petdone(rvals->pet);

    return (void *)rvals;
}

/* Testing program that checks some return values and also has visual inspection for testing grooming functionality*/
int main(int argc, char *argv[])
{
    // Tests for initialization and de-initialization
    // *****************
    printf("**********************************************************************\n");
    int tests_passed = 0;
    printf("TESTS FOR INITIALIZATION AND DEINITIALIZATION\n\n");
    printf("Test1: Testing simple initialization and deinitialization\n");

    print_stations_state("master");
    int result1 = petgroom_init(6);
    if (result1 != 0) {
        printf("Error: Initialization failed\n");
    }

    print_stations_state("master");

    int result2 = petgroom_done();
    if (result2 != 0) {
        printf("Error: De-initialization failed\n");
    }

    int result2_1 = petgroom_init(0);
    if (result2_1 != -1) {
        printf("Error: Initialization with bad input success\n");
    }
    
    if (result1 == 0 && result2 == 0 && result2_1 == -1) {
        tests_passed += 1;
        printf("Test1 Success\n");
    }

    print_stations_state("master");

    printf("Test1 Complete\n");
    printf("***********************************\n\n");
    // *****************
    printf("***********************************\n");
    printf("Test2: Testing re-initialization after de-initialization\n");

    int result3 = petgroom_init(6);
    if (result3 != 0) {
        printf("Error: Re-initialization failed\n");
    }
    else {
        tests_passed += 1;
        printf("Test2 Success\n");
    }
    
    print_stations_state("master");
    printf("Test2 Complete\n");
    printf("***********************************\n\n");
    // *****************
    printf("***********************************\n");
    printf("Test3: Testing initialization when already initialized\n");
    int result4 = petgroom_init(6);
    if (result4 != -1) {
        printf("Error: Double initialization success\n");
    }
    else {
        tests_passed += 1;
        printf("Test3 Success\n");
    }

    printf("Test3 Complete\n");
    printf("***********************************\n\n");
    // *****************
    printf("***********************************\n");
    printf("Test4: De-initialization when already de-initialized\n");
    petgroom_done();
    int result5 = petgroom_done();
    if (result5 != -1) {
        printf("Error: Double de-initialization success\n");
    }
    else {
        tests_passed += 1;
        printf("Test4 Success\n");
    }

    printf("Test4 Complete\n");
    printf("***********************************\n\n");
    printf("TESTS FOR INITIALIZATION AND DE-INITIALIZATION COMPLETE\n");
    printf("%d OUT OF 4 TESTS PASSED\n", tests_passed);
    printf("**********************************************************************\n\n\n");


    // Tests for error checking on newpet() and petdone()
    printf("**********************************************************************\n");
    printf("TESTS FOR ERROR CHECKING ON newpet() and petdone()\n\n");
    int tests_passed1 = 0;
    printf("Test5: Error checking for no pets when using petdone()\n");
    petgroom_init(6);
    pet_t pet = dog;
    int result6 = petdone(pet);
    if (result6 != -1) {
        printf("Error: Success with petdone() on empty groomer's\n");
    }
    else {
        tests_passed1 += 1;
        printf("Test5 Success\n");
    }
    printf("Test5 Complete\n");
    printf("***********************************\n\n");
    // *****************
    printf("***********************************\n");
    printf("Test6: Error checking for uninitialized groomign facility for newpet() and petdone()\n");
    petgroom_done();
    int result7 = petdone(pet);
    if (result7 != -1) {
        printf("Error: Success with petdone() when uninitialized\n");
    }
    int result8 = newpet(pet);
    if (result8 != -1) {
        printf("Error: Success with newpet() when uninitialized\n");
    }

    if (result7 == -1 && result8 == -1) {
        tests_passed1 += 1;
        printf("Test6 Success\n");
    }
    printf("Test6 Complete\n");
    printf("***********************************\n\n");
    printf("TESTS FOR ERROR CHECKING ON newpet() and petdone() COMPLETE\n");
    printf("%d OUT OF 2 TESTS PASSED\n", tests_passed1);

    printf("**********************************************************************\n\n\n");


    // Tests for grooming functionality
    printf("**********************************************************************\n");
    printf("TESTS FOR GROOMING FUNCTIONALITY\n");
    printf("THESE TESTS ARE TO BE READ FOR ERRORS SINCE THERE ARE NO RETURN CHECKS\n\n");

    printf("***********************************\n");
    printf("Test7: Testing with 2 pets of same type\n");
    petgroom_init(6);

    pthread_t thread1, thread2;
    myret_t *rvals1, *rvals2;
    myarg_t args1 = {dog, 5, "1"};
    myarg_t args2 = {dog, 8, "2"};

    pthread_create(&thread1, NULL, pet_thread, &args1);
    pthread_create(&thread2, NULL, pet_thread, &args2);

    pthread_join(thread1, (void **)&rvals1);
    pthread_join(thread2, (void **)&rvals2);

    free(rvals1);
    free(rvals2);

    petgroom_done();

    printf("Test7 Complete\n");
    printf("***********************************\n\n");
    // *************************
    printf("***********************************\n");
    printf("Test8: Testing with a dog and a cat\n");
    petgroom_init(6);

    myret_t *rvals3, *rvals4;

    myarg_t args3 = {dog, 5, "1"};
    myarg_t args4 = {cat, 5, "2"};

    pthread_create(&thread1, NULL, pet_thread, &args3);
    pthread_create(&thread2, NULL, pet_thread, &args4);

    pthread_join(thread1, (void **)&rvals3);
    pthread_join(thread2, (void **)&rvals4);

    free(rvals3);
    free(rvals4);

    petgroom_done();

    printf("Test8 Complete\n");
    printf("***********************************\n\n");
    // *************************
    printf("***********************************\n");
    printf("Test9: Testing with a dog and a other\n");
    petgroom_init(6);

    myret_t *rvals5, *rvals6;

    myarg_t args5 = {other, 5, "1"};
    myarg_t args6 = {dog, 5, "2"};

    pthread_create(&thread1, NULL, pet_thread, &args5);
    pthread_create(&thread2, NULL, pet_thread, &args6);

    pthread_join(thread1, (void **)&rvals5);
    pthread_join(thread2, (void **)&rvals6);

    free(rvals5);
    free(rvals6);

    petgroom_done();

    printf("Test9 Complete\n");
    printf("***********************************\n\n");
    // *************************
    printf("***********************************\n");
    printf("Test10: Testing with a full grooming facility\n");
    petgroom_init(2);

    pthread_t thread3;

    myret_t *rvals7, *rvals8, *rvals9;

    myarg_t args7 = {other, 10, "1"};
    myarg_t args8 = {dog, 2, "2"};
    myarg_t args9 = {cat, 4, "3"};

    pthread_create(&thread1, NULL, pet_thread, &args7);
    pthread_create(&thread2, NULL, pet_thread, &args8);
    pthread_create(&thread3, NULL, pet_thread, &args9);

    pthread_join(thread1, (void **)&rvals7);
    pthread_join(thread2, (void **)&rvals8);
    pthread_join(thread3, (void **)&rvals9);

    free(rvals7);
    free(rvals8);
    free(rvals9);

    petgroom_done();

    printf("Test10 Complete\n");
    printf("***********************************\n\n");
    // *************************
    printf("***********************************\n");
    printf("Test11: Testing Rule 3 starvation prevention (Should pretty much process grooms in the order received FIFO)\n The dog that arrives after the cat blocks until cat completes\n");
    petgroom_init(6);

    pthread_t thread4, thread5, thread6;

    myret_t *rvals10, *rvals11, *rvals12, *rvals13, *rvals14, *rvals15;

    myarg_t args10 = {dog, 5, "1"};
    myarg_t args11 = {dog, 2, "2"};
    myarg_t args12 = {dog, 10, "3"};
    myarg_t args13 = {cat, 3, "4"};
    myarg_t args14 = {dog, 4, "5"};
    myarg_t args15 = {dog, 3, "6"};

    pthread_create(&thread1, NULL, pet_thread, &args10);
    sleep(1);
    pthread_create(&thread2, NULL, pet_thread, &args11);
    sleep(1);
    pthread_create(&thread3, NULL, pet_thread, &args12);
    sleep(1);
    pthread_create(&thread4, NULL, pet_thread, &args13);
    sleep(1);
    pthread_create(&thread5, NULL, pet_thread, &args14);
    sleep(1);
    pthread_create(&thread6, NULL, pet_thread, &args15);

    pthread_join(thread1, (void **)&rvals10);
    pthread_join(thread2, (void **)&rvals11);
    pthread_join(thread3, (void **)&rvals12);
    pthread_join(thread4, (void **)&rvals13);
    pthread_join(thread5, (void **)&rvals14);
    pthread_join(thread6, (void **)&rvals15);

    free(rvals10);
    free(rvals11);
    free(rvals12);
    free(rvals13);
    free(rvals14);
    free(rvals15);

    petgroom_done();

    printf("Test11 Complete\n");
    printf("***********************************\n\n");
    // *************************
    printf("***********************************\n");
    printf("Test12: Super Testing with other, cats, and dogs\n");
    petgroom_init(6);

    pthread_t thread7, thread8, thread9, thread10, thread11;

    myret_t *rvals16, *rvals17, *rvals18, *rvals19, *rvals20;

    myarg_t args16 = {other, 5, "1"};
    myarg_t args17 = {dog, 2, "2"};
    myarg_t args18 = {other, 10, "3"};
    myarg_t args19 = {cat, 3, "4"};
    myarg_t args20 = {dog, 4, "5"};
    myarg_t args21 = {other, 3, "6"};
    myarg_t args22 = {cat, 5, "7"};
    myarg_t args23 = {cat, 7, "8"};
    myarg_t args24 = {dog, 10, "9"};
    myarg_t args25 = {dog, 3, "10"};
    myarg_t args26 = {cat, 4, "11"};

    pthread_create(&thread1, NULL, pet_thread, &args16);
    pthread_create(&thread2, NULL, pet_thread, &args17);
    pthread_create(&thread3, NULL, pet_thread, &args18);
    pthread_create(&thread4, NULL, pet_thread, &args19);
    sleep(1);
    pthread_create(&thread5, NULL, pet_thread, &args20);
    pthread_create(&thread6, NULL, pet_thread, &args21);
    sleep(1);
    pthread_create(&thread7, NULL, pet_thread, &args22);
    pthread_create(&thread8, NULL, pet_thread, &args23);
    sleep(4);
    pthread_create(&thread9, NULL, pet_thread, &args24);
    pthread_create(&thread10, NULL, pet_thread, &args25);
    pthread_create(&thread11, NULL, pet_thread, &args26);

    pthread_join(thread1, (void **)&rvals10);
    pthread_join(thread2, (void **)&rvals11);
    pthread_join(thread3, (void **)&rvals12);
    pthread_join(thread4, (void **)&rvals13);
    pthread_join(thread5, (void **)&rvals14);
    pthread_join(thread6, (void **)&rvals15);
    pthread_join(thread7, (void **)&rvals16);
    pthread_join(thread8, (void **)&rvals17);
    pthread_join(thread9, (void **)&rvals18);
    pthread_join(thread10, (void **)&rvals19);
    pthread_join(thread11, (void **)&rvals20);

    free(rvals10);
    free(rvals11);
    free(rvals12);
    free(rvals13);
    free(rvals14);
    free(rvals15);
    free(rvals16);
    free(rvals17);
    free(rvals18);
    free(rvals19);
    free(rvals20);

    petgroom_done();

    printf("Test12 Complete\n");
    printf("***********************************\n\n");
    printf("TESTING COMPLETE\n");
    printf("**********************************************************************\n");
    return 0;
}
