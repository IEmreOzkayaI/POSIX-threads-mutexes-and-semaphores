#include <unistd.h>          //Provides API for POSIX(or UNIX) OS for system calls
#include <stdio.h>           //Standard I/O Routines
#include <stdlib.h>          //For exit() and rand()
#include <pthread.h>         //Threading APIs
#include <semaphore.h>       //Semaphore APIs


#define REGISTER_NUMBER 5
#define CUSTOMER_NUMBER 25         
#define CUSTOMER_ARRIVAL_TIME_MIN 1
#define CUSTOMER_ARRIVAL_TIME_MAX 3
#define COFFEE_TIME_MIN 2
#define COFFEE_TIME_MAX 5


sem_t customers;                   //Semaphore
sem_t registers;                   //Semaphore
sem_t mutex;    		   //Semaphore as a mutex
int waitQueue[CUSTOMER_NUMBER];    //To exchange id between customer and register
int comeNew = 0;                  
int waitNext = 0;                  //Index candidate customer
static int count = 0;              //Counter of No. of customers
void registerThread(void *tmp);    //Thread Function
void customerThread(void *tmp);    //Thread Function
int customerCreationTime = 0;
int lastWaitForMonitor=0;
 
int main()
{   

    /*Thread declaration*/
    pthread_t coffe_register[REGISTER_NUMBER],customer[CUSTOMER_NUMBER]; 
    int i,status=0;
    
    /*Semaphore initialization*/
    sem_init(&customers,0,0);
    sem_init(&registers,0,0);
    sem_init(&mutex,0,1);  
    
    /*Register thread initialization*/
    printf("\t\t\t\t\t\t\t !! DEU CAFE OPENS !!\n");

    /*Creation of n Number Register Threads*/
    for(i=0;i<REGISTER_NUMBER;i++)             
    {   
       status=pthread_create(&coffe_register[i],NULL,(void *)registerThread,(void*)&i);
       sleep(1);
       if(status!=0)
          perror("No Register Present... Sorry!!\n");
    }
   
   
    /*Customer thread initialization*/
    for(i=0;i<CUSTOMER_NUMBER;i++)  //Creation of N Number Customer Threads                   
    {   

       status=pthread_create(&customer[i],NULL,(void *)customerThread,(void*)customerCreationTime);
       customerCreationTime = customerWait();    //Create customers in MAX-MIN interval
       sleep(customerCreationTime);         
       if(status!=0)
           perror("No Customers Yet!!!\n");
    }   
    
    for(i=0;i<CUSTOMER_NUMBER;i++)        //Waiting till all customers are dealt with
        pthread_join(customer[i],NULL);
        

    sleep(lastWaitForMonitor);  //sleep for monitoring last outputs
    printf("!! DEU CAFE CLOSES !!\n");
    
    exit(EXIT_SUCCESS);  //Exit abandoning infinite loop of register thread
}

void customerThread(void *tmp)  /*Customer Process*/
{  	 
   	int currentLocation;
        int time =(int)tmp;
	sem_wait(&mutex);  //Lock mutex to protect seat changes
	
   	printf("CUSTOMER-%d is created after %d seconds. \n",count,time);
	currentLocation = comeNew;
	waitQueue[mySeat] = count;
	comeNew = ++comeNew;  //Choose a vacant chair to sit

	sem_post(&mutex);                        //Release the seat change mutex
	sem_post(&registers);                //Fill one empty register

	sem_wait(&customers);              //Join queue of waiting customers

	sem_wait(&mutex);
	count++;                                 //Arrival of customer
	sem_post(&mutex);

    	pthread_exit(0);
}

void registerThread(void *tmp)        /*Register Process*/
{   
    int register_ = *(int *)(tmp);      
    int nextCustomer, customer , waitTime;


    while(1)           
    {   

        printf("\t\t\t\t\t\t\t\t\t\t\t		REGISTER-%d is empty .\n",register_);
        sem_wait(&registers);          //Join queue of waiting register
        sem_wait(&mutex); 
        nextCustomer = waitNext;
        customer = waitQueue[nextCustomer];         //Get selected customer's ID
	waitNext = ++waitNext;
        sem_post(&customers);          //Call selected customer
        sem_post(&mutex);
        
        printf("\t\t\t\t\t\t		CUSTOMER %d goes to register %d.\n",customer,register_);
        waitTime = registerWait();
        lastWaitForMonitor = waitTime;
	sleep(waitTime);  //sleep halts execution in specified SECOND
	
        printf("CUSTOMER %d finished buying from REGISTER %d after %d seconds \n",customer,register_,waitTime);

        
    }
	

       	
}

int customerWait()     /*Generates random number between MAX-MIN*/
{
     srand(time(NULL));
     int x = rand() % (CUSTOMER_ARRIVAL_TIME_MAX - CUSTOMER_ARRIVAL_TIME_MIN+1) + CUSTOMER_ARRIVAL_TIME_MIN; 
     return x;
}
int registerWait()     /*Generates random number between MAX-MIN*/
{     
     srand(time(NULL));
     int x = rand() % (COFFEE_TIME_MAX - COFFEE_TIME_MIN+1) + COFFEE_TIME_MIN; 
     return x;
}
