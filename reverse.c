#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

int fd, buf_size;
long file_size;
typedef struct{
	char* left_buf;
	char* right_buf;
	off_t left_pos;
	int len;
} thread_info;
pthread_mutex_t file_lock;


void reverse_str(char* str){
	int len = strlen(str);
	int half = len >> 1;
	for(int i = 0; i < half;i++){
		char c = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = c;
	}
}
void* reverse(void* args){
	thread_info info = *((thread_info*)args);
	
	pthread_mutex_lock(&file_lock);
	lseek(fd, info.left_pos,SEEK_SET);
	read(fd, info.left_buf, info.len);	
	lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);
	read(fd, info.right_buf, info.len);
	pthread_mutex_unlock(&file_lock);
	
	info.left_buf[info.len] = '\0';
	info.right_buf[info.len] = '\0';
	
	reverse_str(info.left_buf);
	reverse_str(info.right_buf);
	
	pthread_mutex_lock(&file_lock);
	lseek(fd, info.left_pos,SEEK_SET);
	write(fd, info.right_buf, info.len);
	lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);
	write(fd, info.left_buf, info.len);
	pthread_mutex_unlock(&file_lock);
}

void main(int argc, char *argv[]){
	
	
	int THREADS = 2;
	char* filename = "input.txt";
	int totalMemSize = 0;
	int FlagActivate = 0;
	int j = 0;
	char* ch;
	
	

	for(j = 1; j < argc; j++){
		if(argv[j][0] == '-'){
				strcpy(ch, argv[j]);
				if(!strcmp(ch+1, "nthreads")){
					THREADS = strtol(argv[j+1], (char **)NULL, 10);
					if((THREADS < 1) || (THREADS > 90)){
						fprintf(stderr, "amounts of threads has to be between 1-90.\n");
						argc = 0;
						exit(1);
						
					}
					continue;
				}
				else if(!strcmp(ch+1, "input")){
					filename = argv[j+1];
					continue;
				}
				else if(!strcmp(ch+1, "mem")){
					totalMemSize = strtol(argv[j+1], (char **)NULL, 10);
					FlagActivate = 1;
					continue;
				}
		}
	}
	
	
	pthread_mutex_init(&file_lock, NULL);
	fd = open(filename, O_RDWR);
	char c;
	int n_read;
	thread_info args[THREADS];
	file_size = lseek(fd, 0, SEEK_END);
	
	printf("FILE SIZE = %ld\n", file_size);
	if((THREADS > (file_size/2)) || (FlagActivate && (THREADS > (totalMemSize/2)))){
	
		fprintf(stderr, "Number of threads cant be greater than half the input file size, or the chosen memory size.\n");
		argc = 0;
		exit(1);
		
	}
	long remainder;
	if(!FlagActivate || totalMemSize > file_size){ 
		printf("buffer size determined by file size\n");
		buf_size = (int)(ceil(file_size/(2.0*THREADS)) + 1);
		remainder = file_size >> 1;
	}
	else{
		buf_size = (totalMemSize/THREADS)/2; 
		remainder = totalMemSize >> 1;
	}
	char* left_buf[THREADS];
	char* right_buf[THREADS];
	pthread_t thread[THREADS];
	for(int i = 0; i < THREADS;i++){
		
		
		left_buf[i] = (char*) malloc(buf_size);
		right_buf[i] = (char*) malloc(buf_size);
		
	}

	for(int i = 0; i < THREADS;i++){
		args[i].left_buf = left_buf[i];
		args[i].right_buf = right_buf[i];
		args[i].left_pos = i*(buf_size-1);
		args[i].len = (i != THREADS-1)?(buf_size-1):remainder;
		remainder -= (buf_size-1);
		printf("spawning  thread #%d w/ len = %d, left_pos = %d\n", i, args[i].len, args[i].left_pos); 
		pthread_create(thread + i, NULL, reverse, (void *) (args+i));
	}
	for(int i = 0; i < THREADS;i++)
		pthread_join(thread[i], NULL);

}