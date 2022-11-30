#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<pthread.h>
#include<time.h>
#include<math.h>
#include <sys/types.h> //maybe put this

#define THREADS 2
int fd, buf_size;
long file_size;
typedef struct {
	char* left_buf;
	char* right_buf;
	off_t left_pos;
	int len;
} thread_info;



void reverse_str(char* str) {
	int len = strlen(str);
	int half = len >> 1;
	for (int i = 0; i < half; i++) {
		char c = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = c;
	}
}
void* reverse(void* args) {
	thread_info info = *((thread_info*)args);
	lseek(fd, info.left_pos, SEEK_SET);
	read(fd, info.left_buf, info.len);
	info.left_buf[info.len] = '\0';
	lseek(fd, (off_t)(-(info.left_pos + info.len)), SEEK_END);
	read(fd, info.right_buf, info.len);
	info.right_buf[info.len] = '\0';
	reverse_str(info.left_buf);
	reverse_str(info.right_buf);
	lseek(fd, info.left_pos, SEEK_SET);
	write(fd, info.right_buf, info.len);
	lseek(fd, (off_t)(-(info.left_pos + info.len)), SEEK_END);
	write(fd, info.left_buf, info.len);
}

void main(void) {
	char* filename = "input.txt";
	fd = open(filename, O_RDWR);
	char c;
	int n_read;
	thread_info args[THREADS];
	file_size = lseek(fd, 0, SEEK_END);//find the file size
	if (file_size % 2) {
		fprintf(stderr, "Error: file size is odd\n");
		exit(1);
	}
	buf_size = (int)(ceil(file_size / (2.0 * THREADS)) + 1);
	char* left_buf[THREADS];
	char* right_buf[THREADS];
	pthread_t thread[THREADS];
	for (int i = 0; i < THREADS; i++) {
		left_buf[i] = (char*)malloc(buf_size);
		right_buf[i] = (char*)malloc(buf_size);
	}
	long remainder = file_size >> 1;
	for (int i = 0; i < THREADS; i++) {
		args[i].left_buf = left_buf[i];
		args[i].right_buf = right_buf[i];
		args[i].left_pos = i * (buf_size - 1);
		args[i].len = (i != THREADS - 1) ? (buf_size - 1) : remainder;
		remainder -= (buf_size - 1);
		pthread_create(thread + i, NULL, reverse, (void*)(args + i));
	}
	for (int i = 0; i < THREADS; i++)
		pthread_join(thread[i], NULL);

}

int main()
{

}