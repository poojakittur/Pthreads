//Name : Pooja Kittur
//Course name : "COSC 6360--Operating Systems"
//Assignment : 3

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

int MAXCUSTOMER = 100;
static int NUM_FREE_CLERKS = 0;
static int NUM_CUST_WAIT = 0;
static int NUM_CUST_ARRIVED = 0;

//Declaring the mutex
static pthread_mutex_t post_office;

//Creating condition variable
static pthread_cond_t free_clerk = PTHREAD_COND_INITIALIZER;

void * customerOperation(void *);

struct Customer{
	int customerId;
	int delay;
	int serviceTime;
} *newCustomer;

int main(int argc, char** argv){

	int totalNumberOfClerks;
	int customerID, delay, serviceTime;
	int i = 1;
	int numCust = 0;
	int cust;
	pthread_t tid;
	pthread_t custtid[MAXCUSTOMER];
	
	if(argc < 2){
		printf("Please provide the number of clerks\n");
		exit(0);
	}
	
	//Allocate memory dynamically
	newCustomer = malloc(MAXCUSTOMER *sizeof(newCustomer));

	//Fetching total number of clerks from command line and converting it to int
	totalNumberOfClerks = atoi(argv[1]);
	NUM_FREE_CLERKS = totalNumberOfClerks;

	printf("--The Post office has %d clerk(s)--\n", totalNumberOfClerks);

	//create a lock(mutex)
	pthread_mutex_init(&post_office, NULL);

	//Read the input file line by line
	while(scanf("%d %d %d", &customerID, &delay, &serviceTime) > 0){
		
		//Reallocate the memory if the maximum customers reaches the initial capacity 100
		if(numCust >= MAXCUSTOMER)
		{
			MAXCUSTOMER = MAXCUSTOMER + MAXCUSTOMER;
			newCustomer = realloc(newCustomer, MAXCUSTOMER *sizeof(newCustomer));
		}

		if(delay > 0){
			sleep(delay);
		}

		newCustomer[numCust].customerId = customerID;
		newCustomer[numCust].delay = delay;
		newCustomer[numCust].serviceTime = serviceTime;

		//create a thread
		pthread_create(&custtid[i++], NULL, customerOperation, (void *) &newCustomer[numCust]);
		numCust++;
	}

	//Wait till all the threads terminate
	for(i=1; i<=numCust; i++){
		pthread_join(custtid[i], NULL);
	}

	printf("%d customer(s) went serviced.\n", NUM_CUST_ARRIVED);
	printf("%d customer(s) did not have to wait\n", (NUM_CUST_ARRIVED - NUM_CUST_WAIT));
	printf("%d customer(s) had to wait\n", NUM_CUST_WAIT);
	
	return 0;

}

void *customerOperation(void *arg){
	int serviceTime;
	
	struct Customer *arrivedCustomer = malloc(sizeof(struct Customer));
	arrivedCustomer = (struct Customer *) arg;
	serviceTime = arrivedCustomer -> serviceTime;

	//Request a lock
	pthread_mutex_lock(&post_office);
	printf("Customer # %d arrives at the post office\n", arrivedCustomer -> customerId); 

	NUM_CUST_ARRIVED++;

	//If all the clerks are busy
	while(NUM_FREE_CLERKS == 0){
		
		NUM_CUST_WAIT++;
		
		//wait on condition variable
		pthread_cond_wait(&free_clerk, &post_office);
	}	
	
	//Clerk is serving the customer
	NUM_FREE_CLERKS--;
	printf("Customer # %d starts getting helped\n", arrivedCustomer -> customerId);

	//Unlock the mutex
	pthread_mutex_unlock(&post_office);
	
	//sleep for service time
	sleep(serviceTime);	
		
	pthread_mutex_lock(&post_office);
	printf("Customer # %d leaves the post office\n", arrivedCustomer -> customerId);
	//Clerk is freed
	NUM_FREE_CLERKS++;

	//Signal the thread which is waiting for a clerk using condition variable
	pthread_cond_signal(&free_clerk);
	pthread_mutex_unlock(&post_office);

}
	



