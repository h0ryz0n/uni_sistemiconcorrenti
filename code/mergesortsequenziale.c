/* Merge sort in C */
///////////// MODIFICATO CON DIMENSIONE A SCELTA E CALCOLO TEMPO DI ESECUZIONE IN MICROSECONDI
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <papi.h>

// Function to Merge Arrays L and R into A. 
// lefCount = number of elements in L
// rightCount = number of elements in R. 

// PAPI
long_long startT,stopT;  //tempi di esecuzione
long_long countCacheMiss;  //contatore cache miss
int EventSet = PAPI_NULL;
int retval;   // valore ritorno papi

void Merge(int *A,int *L,int leftCount,int *R,int rightCount) {
	int i,j,k;

	// i - to mark the index of left aubarray (L)
	// j - to mark the index of right sub-raay (R)
	// k - to mark the index of merged subarray (A)
	i = 0; j = 0; k =0;

	while(i<leftCount && j< rightCount) {
		if(L[i]  < R[j]) A[k++] = L[i++];
		else A[k++] = R[j++];
	}
	while(i < leftCount) A[k++] = L[i++];
	while(j < rightCount) A[k++] = R[j++];
}

// Recursive function to sort an array of integers. 
void MergeSort(int *A,int n) {
	int mid,i, *L, *R;
	if(n < 2) return; // base condition. If the array has less than two element, do nothing. 

	mid = n/2;  // find the mid index. 

	// create left and right subarrays
	// mid elements (from index 0 till mid-1) should be part of left sub-array 
	// and (n-mid) elements (from mid to n-1) will be part of right sub-array
	L = (int*)malloc(mid*sizeof(int)); 
	R = (int*)malloc((n- mid)*sizeof(int)); 
	
	for(i = 0;i<mid;i++) L[i] = A[i]; // creating left subarray
	for(i = mid;i<n;i++) R[i-mid] = A[i]; // creating right subarray

	MergeSort(L,mid);  // sorting the left subarray
	MergeSort(R,n-mid);  // sorting the right subarray
	Merge(A,L,mid,R,n-mid);  // Merging L and R into A as sorted list.
        free(L);
        free(R);
}

int main(int argc, char *argv[]) {
	/* Code to test the MergeSort function. */
	//NIENTE PARAMETRI
	if (argc==1)
	{
		printf("PARAM: numeroelementi array");
		return -1;
	}
	//GENERIAMO UN ARRAY A CASO
	int ELEM = atoi(argv[1]);
	int A[ELEM];

	for (int i=0;i<ELEM;i++)
	{
		A[i]=rand();
		//printf("%d" , A[i]);
	}
	printf("\n");
		
	//int A[] = {6,2,3,1,9,10,15,13,12,17}; // creating an array of integers. 
	int i,numberOfElements;

	// finding number of elements in array as size of complete array in bytes divided by size of integer in bytes. 
	// This won't work if array is passed to the function because array
	// is always passed by reference through a pointer. So sizeOf function will give size of pointer and not the array.
	// Watch this video to understand this concept - http://www.youtube.com/watch?v=CpjVucvAc3g  
	numberOfElements = sizeof(A)/sizeof(A[0]); 

	// Calling merge sort to sort the array. 
	/*
	//START CLOCK
	struct timeval tval_before, tval_after, tval_result;
	gettimeofday(&tval_before, NULL);
	*/
	
	// PAPI: INIZIALIZZAZIONE
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		printf("Errore init PAPI\n");
		exit(1);
	}
	// PAPI: CREAZIONE EVENTSET
	if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
		printf("Errore creazione EventSet PAPI\n");
		exit(1);
	}
	retval = PAPI_add_event(EventSet,PAPI_L2_TCM);
	if (retval!=PAPI_OK) printf(PAPI_strerror(retval));
	retval = PAPI_start(EventSet) != PAPI_OK;
	if (retval!=PAPI_OK) printf(PAPI_strerror(retval));
	// PAPI: preleva il tempo
	startT=PAPI_get_real_usec();

	//MERGE SORT
	MergeSort(A,numberOfElements);
	
	/*
	//STOP CLOCK
	gettimeofday(&tval_after, NULL);
	//CALCOLO TEMPO ESECUZIONE
	timersub(&tval_after, &tval_before, &tval_result);
	printf("Time elapsed: %ld us\n", (long int)tval_result.tv_usec);
	*/
	
	// PAPI: stop del timer e del contatore cache miss
	stopT=PAPI_get_real_usec();
	retval = PAPI_stop(EventSet, &countCacheMiss);
	if (retval!=PAPI_OK) printf(PAPI_strerror(retval));
	// VISUALIZZA REPORT
	printf("tcm:%d \ttime:%lld us\n",countCacheMiss,(stopT-startT));

	//printing all elements in the array once its sorted.
	//for(i = 0;i < numberOfElements;i++) printf("%d ",A[i]);
	printf("\n");
	return 0;
}
