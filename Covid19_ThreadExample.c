#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_PATIENT 500
#define UniteSize 8
int loop = 1;
int patients[MAX_PATIENT];
int in = 0; //variable that holds the patients index in the array
int o = 0;

void *patient(void *num);
void *testUnit(void *num);

sem_t EnteringHospital;
sem_t WaitingRoom;
sem_t ProcessesofWaitingR;
sem_t patientlogin;

int Randoms(int lower, int upper) //the function used to bring patients at random time
{
    int num = (rand() % upper - lower + 1) + lower;
    return num;
}

int main(int argc, char *argv[])
{
    pthread_t Utid[UniteSize];
    pthread_t Ptid[MAX_PATIENT];
    int i, x;
    int numPatient;
    int Number[MAX_PATIENT];
    int NumberU[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    printf("Please enter patients number.");
    scanf("%d", &x);
    numPatient = x; //get the number of patients coming from the user.
    if (numPatient > 500)
    {
        printf("The maximum number of patients should be 500.");
        system("PAUSE");
        return 0;
    }

    sem_init(&EnteringHospital, 0, 1); //This semaphore prevents patients from accessing the function used to enter the hospital randomly
                                       // at the same time and allows them to enter the hospital at different times.
                                       //If they access the usleep function that takes random values at the same time, they will all log in at the same time.This would be undesirable

    sem_init(&patientlogin, 0, 1); //It is a semaphore that is created to ensure that patients are taken according to the order they come
                                   //and not to interfere with each other and to avoid confusion while entering the waiting room.

    sem_init(&WaitingRoom, 0, 0); //It is the semaphore that enables the unit to be activated while patients are entering the waiting room.

    sem_init(&ProcessesofWaitingR, 0, 1); //It is the part where what is happening in the room is determined after the patients enter one of the units. Announcements are made for how many more patients are expected or the test process is completed while the patients is inside. If the test
                                          // has been carried out, new patients are admitted to the room.The semaphore used here prevents interference of announcements.

    for (i = 0; i < MAX_PATIENT; i++)
    {
        Number[i] = i;
    }

    for (i = 0; i < UniteSize; i++) //creating Units treads
    {
        pthread_create(&Utid[i], NULL, testUnit, (void *)&NumberU[i]);
    }

    for (i = 0; i < numPatient; i++) //creating patients treads
    {
        pthread_create(&Ptid[i], NULL, patient, (void *)&Number[i]);
    }

    for (i = 0; i < numPatient; i++)
    {
        pthread_join(Ptid[i], NULL);
    }
    loop = 0;

    system("PAUSE");
    return 0;
}

void *patient(void *num)
{
    int id = *(int *)num;
    sem_wait(&EnteringHospital);
    int time = Randoms(50, 150);
    usleep(time); //allows patients to enter the hospital at random times
    printf("The patient %d came to the hospital.\n", id);
    sem_post(&EnteringHospital);
    sem_wait(&patientlogin);
    patients[in] = id; //It is the global array where we keep patient IDs in order of arrival in order to access patient IDs from testUnit function.
    in++;
    sem_post(&WaitingRoom);
    usleep(500);
    sem_post(&patientlogin);
}

void *testUnit(void *num)
{
    int id = *((int *)num);
    int Patientsarray[3]; //the local array for each unit that holds the patient in it
    int countt = 0;       //It is the variable that holds the index of patients in the Patientsarray.
                          //When the number of patients in the array is 3, countt is set to 0.
    while (loop)
    {
        sem_wait(&WaitingRoom);
        sem_wait(&ProcessesofWaitingR);
        printf("%d enter the Covid19 test unit %d  waiting room.\n%d filling the form\n", patients[o], id, patients[o]);
        Patientsarray[countt] = patients[o];
        countt++;
        o++;

        if (countt == 1) //message to press console when there is a patient in the waiting room
        {
            printf("[X][][]unit %d staff say :*Two patient is waited *\n", id);
            printf("\n");
        }
        if (countt == 2) //message to press console when there are two patient in the waiting room
        {
            printf("[X][X][]unit %d staff say :Thelast people,let's start!Please, pay attention to your social distance and hygiene; use a mask.\n", id);
            printf("\n");
        }
        if (countt == 3) //messages to be pushed to the console when there are three patients in the room and resetting the Patientsarray index.
        {
            printf("[X][X][X]unit %d : %d,%d,%d patient testing completed.\n", id, Patientsarray[0], Patientsarray[1], Patientsarray[2]);
            printf("Patients %d,%d,%d are leaving the hospital.Unit %d staff say : *The room is ventilated.*\n", Patientsarray[0], Patientsarray[1], Patientsarray[2], id);
            printf("\n");
            countt = 0;
        }
        sem_post(&ProcessesofWaitingR);
    }
}
