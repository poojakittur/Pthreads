#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

int MAXCUSTOMER = 100;
static int NUM_FREE_CLERKS = 0;
static int NUM_CUST_WAIT = 0;
static int NUM_CUST_ARRIVED = 0;

static pthread_mutex_t post_office;
static pthread_cond_t free_clerk = PTHREAD_COND_INITIALIZER;

void * customerOperation(void *);

struct Customer{
	int customerId;
	int delay;
	int serviceTime;
}newCustomer[100];

int main(int argc, char** argv){

	int totalNumberOfClerks;
	int customerID, delay, serviceTime;
	int i = 1;
	int numCust = 0;
	int cust;
	pthread_t tid;
	pthread_t custtid[MAXCUSTOMER];
	
	totalNumberOfClerks = atoi(argv[1]);
	NUM_FREE_CLERKS = totalNumberOfClerks;

	printf("--The Post office has %d clerks--\n", totalNumberOfClerks);

	//create a lock
	pthread_mutex_init(&post_office, NULL);

	while(scanf("%d %d %d", &customerID, &delay, &serviceTime) > 0){

		//printf("Customer id is : %d\n", customerID);
		//printf("Delay is : %d\n", delay);
		//printf("Service Time is : %d\n", serviceTime);
		
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

	for(i=1; i<=numCust; i++){
		pthread_join(custtid[i], NULL);
		//printf(" cust %d joined\n",i);
	}

	printf("%d customer(s) went serviced.\n", NUM_CUST_ARRIVED);
	printf("%d customer(s) did not have to wait\n", (NUM_CUST_ARRIVED - NUM_CUST_WAIT));
	printf("%d customer(s) had to wait\n", NUM_CUST_WAIT);
	
	return 0;

}

void *customerOperation(void *arg){
	int serviceTime;
	//customerId = (int) arg;
	struct Customer *arrivedCustomer = malloc(sizeof(struct Customer));
	arrivedCustomer = (struct Customer *) arg;
	serviceTime = arrivedCustomer -> serviceTime;

	//Request a lock
	pthread_mutex_lock(&post_office);
	printf("Customer # %d arrives at the post office\n", arrivedCustomer -> customerId); 

	NUM_CUST_ARRIVED++;
	//printf("Number of cust incremented\n");

	while(NUM_FREE_CLERKS == 0){
		
		NUM_CUST_WAIT++;
		
		//printf("Waiting on a conditional variable\n");
		//wait on condition variable
		pthread_cond_wait(&free_clerk, &post_office);
		//printf("Wait over \n");
	}	
	
	//printf("Decrementing num of clerks \n");
	NUM_FREE_CLERKS--;
	printf("Customer # %d starts getting helped\n", arrivedCustomer -> customerId);
	pthread_mutex_unlock(&post_office);
	
	//sleep for service time
	sleep(serviceTime);	
		
	pthread_mutex_lock(&post_office);
	printf("Customer # %d leaves the post office\n", arrivedCustomer -> customerId);
	NUM_FREE_CLERKS++;
	pthread_cond_signal(&free_clerk);
	pthread_mutex_unlock(&post_office);

	//pthread_exit((void*) 0);
}
	



