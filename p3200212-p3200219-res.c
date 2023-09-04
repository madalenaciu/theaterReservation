#include <stdio.h>
#include "p3200212-p3200219-res.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define BILLION 1000000000L;

typedef struct init_arguments{
	int size;                //size of table id = num of customers
	int id[];                //table with customers's id
}INIT_ARGUMENTS;

enum {NS_PER_SECOND = 1000000000};
double time1 = 0;
double time2 = 0;
double time3 = 0;
int sec = 0;
long nsec = 0;
int seed;  
int Ncust;
int tel = Ntel; 
int cash = Ncash;
int seats[NzoneA+NzoneB][Nseat];    //theater plan 
int c1 = 0;                         //counter for sucess
int c2 = 0;                         //counter for rejection no seats
int c3 = 0;                         //counter for rejection no money
double account = 0;                 //banking account
pthread_mutex_t mutexc1;            //mutex for variable c1
pthread_mutex_t mutexc2;            //mutex for variable c2
pthread_mutex_t mutexc3;            //mutex for variable c3
pthread_mutex_t mutexbudget;        //mutex for variable account 
pthread_mutex_t mutexseat;          //mutex for table seats
pthread_mutex_t mutexcount;         //mutex for variable tel (Ntel)
pthread_mutex_t mutexcount2;        //mutex for variable cash (Ncash)
pthread_mutex_t mutexexit;          
pthread_cond_t countThresholdCondition1;    
pthread_cond_t countThresholdCondition2;
int rA,rB = 0;


int main(int argc,char *argv[]){
	int rc;
	if(argc!= 3){
		printf("ERROR WRONG NUMBER OF ARGUMENTS");
		
	}else{
	
		Ncust = atoi(argv[1]);
		seed = atoi(argv[2]);
		INIT_ARGUMENTS *arr = (INIT_ARGUMENTS  *)malloc(sizeof(INIT_ARGUMENTS *) + Ncust * sizeof(int));
		pthread_t thread[Ncust];
		
		// initialization of mutexcount
		rc=pthread_mutex_init(&mutexcount,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexcount) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexcount2
		rc=pthread_mutex_init(&mutexcount2,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexcount2) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexc1
		rc=pthread_mutex_init(&mutexc1,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexc1) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexc2
		rc=pthread_mutex_init(&mutexc2,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexc2) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexc3
		rc=pthread_mutex_init(&mutexc3,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexc3) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexbudget
		rc=pthread_mutex_init(&mutexbudget,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexbudget) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexseat
		rc=pthread_mutex_init(&mutexseat,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexseat) is %d\n", rc);
			exit(-1);
		}
		
		// initialization of mutexexit
		rc=pthread_mutex_init(&mutexexit,NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_mutex_init(mutexexit) is %d\n", rc);
			exit(-1);
		}
		
		//initialization of countThresholdCondition1
		rc=pthread_cond_init(&countThresholdCondition1,NULL);
		if (rc != 0) {
    		printf("ERROR: return code from pthread_cond_init(countThresholdCondition1) is %d\n", rc);
       		exit(-1);
	    }
		
		//initialization of countThresholdCondition2
		rc=pthread_cond_init(&countThresholdCondition2,NULL);
		if (rc != 0) {
    		printf("ERROR: return code from pthread_cond_init(countThresholdCondition1) is %d\n", rc);
       		exit(-1);
	    }
		
		
		//initialization of theather array with 0
		for(int i =0;i<30;i++){
			for(int j = 0;j<10;j++){
				seats[i][j] = 0;
			}
		}
		
		//thread creating
		for(int i = 0;i<Ncust;i++){
			arr->id[i] = i+1;
			rc = pthread_create(&thread[i],NULL,booking,&arr->id[i]);
			
			if (rc != 0) {
    		printf("ERROR: return code from pthread_create() is %d\n", rc);
       		exit(-1);
	        }
			sleep(rand_r(&seed)%(treshigh-treslow)+treslow);	
		}
		
		
		for(int i = 0;i<Ncust;i++){
			rc=pthread_join(thread[i],NULL);
			if (rc != 0) {
		        printf("ERROR: return code from pthread_join() is %d\n", rc);
		        exit(-1);		
	        }
        }
		
		//print theater plan after process completion
		int l=0;
		for(int i =0;i<30;i++){
			for(int j = 0;j<10;j++){
				l ++;
				if(i<10){
					printf("ZoneA Row: %d Seat %d Customer: %d \n",i,l,seats[i][j]);
				}
				else{
					printf("ZoneB Row: %d Seat %d Customer: %d \n",i,l,seats[i][j]);
				}
				
			}
		}
		
		
		printf("Reservetion revenue: %lf \n",account);       //print banking account 
		
		//print time
		printf("%lf\n",(time1+time2)/Ncust); //meso xrono anamonhs pelatwn
		printf("%lf\n",(time3)/Ncust);		//meso xrono ejyphrethshs pelatwn
		//printf("\nOR \n%d seconds and %ld nanoseconds wait!\n",sec,nsec);
		//print precentages 
		printf("Sucess of booking %lf \n",(double)c1/(double)Ncust*100);
		printf("Rejection of no seats %lf \n",(double)c2/(double)Ncust*100);
		printf("rejection of not enough money for booking %lf \n",(double)c3/(double)Ncust*100);
		printf("error seats A %d ",rA);
		
		free(arr);
		//destroy mutexcount
		rc=pthread_mutex_destroy(&mutexcount);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexcount) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexcount2
		rc=pthread_mutex_destroy(&mutexcount2);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexcount2) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexc1
		rc=pthread_mutex_destroy(&mutexc1);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexc1) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexc2
		rc=pthread_mutex_destroy(&mutexc2);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexc2) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexc3
		rc=pthread_mutex_destroy(&mutexc3);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexc3) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexexit
		rc=pthread_mutex_destroy(&mutexexit);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexexit) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexbudget
		rc=pthread_mutex_destroy(&mutexbudget);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexbudget) is %d\n", rc);
      		exit(-1);
	    }
		
		//destroy mutexseat
		rc=pthread_mutex_destroy(&mutexseat);
		if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy(mutexseat) is %d\n", rc);
      		exit(-1);
	    }
        
		//destroy condition countThresholdCondition1
		rc=pthread_cond_destroy(&countThresholdCondition1);
		if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy(countThresholdCondition1) is %d\n", rc);
		exit(-1);		
	    }
		
		
		//destroy condition countThresholdCondition2
		rc=pthread_cond_destroy(&countThresholdCondition2);
		if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy(countThresholdCondition2) is %d\n", rc);
		exit(-1);		
	    }
	
	}

	
	//free(arr->id);
	return 0;
}

int myPow(int x,int n){
	int i;
	int number = 1;
	for(i = 0;i<n;i++){
		number *= x;
	}
	return(number);

}

struct timespec start, finish,start2,finish2,start3,finish3;

void * booking(void * args){
	int rc;
	int idd = *(int *)args;                    //id of customer
	rc = pthread_mutex_lock(&mutexcount);
	clock_gettime(CLOCK_REALTIME,&start3);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock(mutexcount) is %d\n", rc);
		pthread_exit(&rc);
	}
	
	clock_gettime(CLOCK_REALTIME,&start);
	printf("Customer number: %d \n",idd);
	//tha jekinhsei apo edw
	
	while(tel == 0){
		printf("WAIT for available operator..\n");
		rc = pthread_cond_wait(&countThresholdCondition1,&mutexcount);
		if (rc != 0) {	
		   printf("ERROR: return code from pthread_cond_wait(countThresholdCondition1) is %d\n", rc);
		   pthread_exit(&rc);
	    }
	}
	clock_gettime(CLOCK_REALTIME,&finish);
	printf("Serving..\n");
	tel--;
	double t =  (finish.tv_sec-start.tv_sec) + (double)(finish.tv_nsec-start.tv_nsec)/(double)BILLION;
	time1+= t;

	rc = pthread_mutex_unlock(&mutexcount);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock(mutexcount) is %d\n", rc);
		pthread_exit(&rc);
	}
	
	int p = rand_r(&seed)%(10-1+1)+1;         //find the zone...A or B?
	char *res;
	int x = 0;
	if(p <= PzoneA*10){
		res = "A";
	}
	else{
		x = 1;
		res = "B";
	}
	
	int numOfSeats = rand_r(&seed)%(Nseathigh-Nseatlow+1)+Nseatlow;    
	sleep(rand_r(&seed)%(tseathigh-tseatlow+1)+tseatlow);
	rc = pthread_mutex_lock(&mutexseat);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock(mutexseat) is %d\n", rc);
		pthread_exit(&rc);
	}
	int count =0;          //number of adjacent seats
	int cost;              //Cost for each customer
	int i = x*10;
	int k;
	int j;
	int e = myPow(3,x);
	while(i<e*10){
		j =0;
		while(j<10){
			if(seats[i][j] == 0){
				count++;
			}else{
				count = 0;
			}
			if(count == numOfSeats){
				//printf("NUM OF SEATS %d AND CUSTOMER %d\n",numOfSeats,idd);
				k = j - numOfSeats+1;
				while(k<=j){
					seats[i][k] = idd;
					k++;
				}
				if(x==0){
					cost = CzoneA*numOfSeats;
				}else{
					cost = CzoneB*numOfSeats;
				}
			    break;
			}
		    j++;
		}
		if(cost!=0){
			break;
		}
		i++;
		count = 0;
	}

	
	rc = pthread_mutex_unlock(&mutexseat);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock(mutexseat) is %d\n", rc);
		pthread_exit(&rc);
	}
	rc = pthread_mutex_lock(&mutexcount);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock(mutexcount) is %d\n", rc);
		pthread_exit(&rc);
	}
	tel++;
	rc = pthread_cond_signal(&countThresholdCondition1);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_signal(countThresholdCondition1) is %d\n", rc);
		pthread_exit(&rc);
	}
	rc = pthread_mutex_unlock(&mutexcount);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock(mutexseat) is %d\n", rc);
		pthread_exit(&rc);
	}
	
	if(cost==0){
		rc = pthread_mutex_lock(&mutexc2);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexc2) is %d\n", rc);
		    pthread_exit(&rc);
	    }
		c2++;
		printf("Error not enough seats %d. Try another day...Bye bye.\n",idd);
		if(res == "A"){
			rA ++;
		}
		rc = pthread_mutex_unlock(&mutexc2);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_unlock(mutexc2) is %d\n", rc);
		    pthread_exit(&rc);
	    }
	    clock_gettime(CLOCK_REALTIME,&finish3);
	    double t3 =  (finish3.tv_sec-start3.tv_sec) + (double)(finish3.tv_nsec-start3.tv_nsec)/(double)BILLION;
	    time3 += t3;
		pthread_exit(NULL);
	}
	
	rc = pthread_mutex_lock(&mutexcount2);
	clock_gettime(CLOCK_REALTIME,&start2);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock(mutexcount2) is %d\n", rc);
	    pthread_exit(&rc);
	}
	while(cash == 0){
		printf("WAIT for available cashier...\n");
		rc = pthread_cond_wait(&countThresholdCondition2,&mutexcount2);
		if (rc != 0) {	
		   printf("ERROR: return code from pthread_cond_wait(countThresholdCondition2) is %d\n", rc);
		   pthread_exit(&rc);
	    }
	}
	printf("Serving..\n");
	cash--;
	
	clock_gettime(CLOCK_REALTIME,&finish2);
	t =  (finish2.tv_sec-start2.tv_sec) + (double)(finish2.tv_nsec-start2.tv_nsec)/(double)BILLION;
	time2 += t;
	rc = pthread_mutex_unlock(&mutexcount2);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock(mutexcount2) is %d\n", rc);
		pthread_exit(&rc);
	}
	
	sleep(rand_r(&seed)%(tcashhigh-tcashlow+1)+tcashlow);
	p = rand_r(&seed)%(10-1+1)+1;         //find if it will be a successful payment
	k =j-numOfSeats+1;
	//int s = i *10 +1;

	if(p < Pcardsucces*10){
		rc = pthread_mutex_lock(&mutexc1);
	    if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexc1) is %d\n", rc);
		    pthread_exit(&rc);
	    }
		c1 ++;
		int s2[5];
		for(int u =0;u<5;u++){
			s2[u] = -1;
		}
		int count2 = 0;
		int count3 = 0;
		for(int u = 0;u<30;u++){
			for(int f = 0;f<10;f++){
				count3++;
				if(seats[u][f] == idd){
					s2[count2] = count3;
					count2++;
				}
			}
		}
		printf("Acceptable customer %d seats at zone %s Row: %d  Seats: %d  %d  %d  %d  %d \n",idd,res,i,s2[0],s2[1],s2[2],s2[3],s2[4]);
		
		rc = pthread_mutex_unlock(&mutexc1);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_unlock(mutexc1) is %d\n", rc);
		    pthread_exit(&rc);
	    }
		rc = pthread_mutex_lock(&mutexbudget);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexbudget) is %d\n", rc);
		    pthread_exit(&rc);
	    }
		account+= cost;
		rc = pthread_mutex_unlock(&mutexbudget);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexbudget) is %d\n", rc);
		    pthread_exit(&rc);
	    }
	}
	else{
		rc = pthread_mutex_lock(&mutexc3);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexc3) is %d\n", rc);
		    pthread_exit(&rc);
	    }
		c3++;
		printf("Not successful payment %d, we return the seats.\n",idd);
		rc = pthread_mutex_unlock(&mutexc3);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexc3) is %d\n", rc);
		    pthread_exit(&rc);
	    }
		rc = pthread_mutex_lock(&mutexseat);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_lock(mutexcseat) is %d\n", rc);
		    pthread_exit(&rc);
	    }
	    for(int y =0;y<30;y++){
	    	for(int r = 0;r<10;r++){
	    		if(seats[y][r] == idd){
	    			seats[y][r] = 0;
	    		}
	    	}
	    }
		rc = pthread_mutex_unlock(&mutexseat);
		if (rc != 0) {	
		    printf("ERROR: return code from pthread_mutex_unlock(mutexseat) is %d\n", rc);
		    pthread_exit(&rc);
	    }
	}
	rc = pthread_mutex_lock(&mutexcount2);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock(mutexcount2) is %d\n", rc);
		pthread_exit(&rc);
	}
	cash++;
	rc = pthread_cond_signal(&countThresholdCondition2);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_signal(countThresholdCondition2) is %d\n", rc);
		pthread_exit(&rc);
	}
	rc = pthread_mutex_unlock(&mutexcount2);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock(mutexcount2) is %d\n", rc);
		pthread_exit(&rc);
	}
	clock_gettime(CLOCK_REALTIME,&finish3);
	double t3 =  (finish3.tv_sec-start3.tv_sec) + (double)(finish3.tv_nsec-start3.tv_nsec)/(double)BILLION;
	time3 += t3;
	pthread_exit(NULL);
	
}
