#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex;
unsigned char files[16][40];

struct sharedBuffers{
	int indexOfFiles;
	char output[40];

};
struct sharedBuffers sB[7];

void bubbleSort(int * integers, int n){
	int temp;
	for(int i = 0;i<n-1;i++){
		for(int j =0;j<n-i-1;j++){
			if(integers[j]>integers[j+1]){
				temp = integers[j];
				integers[j] = integers[j+1];
				integers[j+1] = temp;
			}
		}
	}
}

void * routine(void * arg){

	int index = *(int*)arg;
	
	
	FILE * fptr1;
	FILE * fptr2;
	FILE * fptro;

	fptr1 = fopen(files[index*2],"r");
	fptr2 = fopen(files[index*2+1],"r");

	int integers[999];
	int i=0;

	if(fptr1 == NULL){
		printf("File not found: %s",files[index]);
	}
	else{
		while(!feof(fptr1)){
			fscanf(fptr1, "%d",&integers[i]);
			i++;
		}
	}
	if(fptr2 == NULL){
		printf("File not found: %s",files[index+1]);
	}
	else{
		while(!feof(fptr2)){
			fscanf(fptr2,"%d",&integers[i]);
			i++;
		}
	}
	bubbleSort(integers, i-1);
	fptro = fopen(sB[index].output,"w");

	if(fptro==NULL){
		printf("file cannot created");
	}
	else{
		pthread_mutex_lock(&mutex);
		for(int j=1;j<i-1;j++){
			
			fprintf(fptro, "%d\n", integers[j]);
			
		}
		pthread_mutex_unlock(&mutex);
	}

	fclose(fptr1);
	fclose(fptr2);
	fclose(fptro);
		
	free(arg);
}

int main(int argc, char *argv[]){
	//timer
	clock_t start, stop;
	double duration;
	start = clock();

	pthread_t threads[7];
	int counter;
	int FileCount;
	char outputName[40];
	char bufferName[L_tmpnam];
	
	if(argc == 1){
		printf("error, no files entered\n");
	}
	counter = 0;
	for(int i=1;i<argc ;i++){
		if(strcmp(argv[i],"-n") == 0){
			FileCount= atoi(argv[i+1]);
			i++;

		}
		else if(strcmp(argv[i],"-o") == 0){
			strncpy(outputName, argv[i+1], 40);
			i++;
		}
		else{
			strncpy(files[counter], argv[i],40);
			counter++; 
		}

	}
	int inputFileCount = counter;
	//buffer name assignment
	for(int i=0;i<FileCount-1;i++){
		
		if(i==FileCount-2){
			strncpy(sB[i].output, outputName, 40);
			strncpy(files[counter], outputName,40);
			counter++;
		}
		else{
			strncpy(sB[i].output, tmpnam(bufferName), 40);

			strncpy(files[counter], sB[i].output,40);
			counter++;
		}
	}

	if(inputFileCount != FileCount){
		printf("error, not enough files\n");
		exit(0);
		
	}
	//threads initializer
	pthread_mutex_init(&mutex,NULL);
	for(int i=0;i<FileCount-1;i++){
		int* a = malloc(sizeof(int));
		*a = i;
		if(pthread_create(&threads[i], NULL, &routine, a) != 0){
			printf("Thread create failed\n");
		}
		sleep(.001);
	}
	for(int i=0;i<FileCount-1;i++){
		if(pthread_join(threads[i], NULL) !=0){
			printf("Thread join failed\n");
		}
	}
	pthread_mutex_destroy(&mutex);

	stop = clock();
	duration = (double)(stop-start);
	printf("Duration: %.2lf\n", duration);

	return 0;
}
