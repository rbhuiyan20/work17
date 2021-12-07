#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main() {

  // gets key of the contents file
	int k = ftok("contents.txt", 7);
  // semaphore id
	int sID = semget(k, 1, 0);
	if (sID == -1) {
		printf("Error getting semaphore: %s\n", strerror(errno));
		return 1;
	}

  // struct sembuf from notes
	struct sembuf sb;
  sb.sem_op = -1;
	sb.sem_num = 0;
	sb.sem_flg = SEM_UNDO;
	semop(sID, &sb, 1);

	int memID = shmget(k, sizeof(int), 0);
	int *memData = shmat(memID, 0, 0);

  // to get the last line added to content.txt
	int fileDesc = open("contents.txt", O_RDWR | O_APPEND, 0);
  // to change location of pointer of fileDesc
	lseek(fileDesc, *memData * -1, SEEK_END);
	char* file = malloc(*memData);
	read(fileDesc, file, *memData);
	printf("Last line added to contents.txt: %s\n", file);

  // to write into content.txt the  new user input
	
	printf("Add new line please!: ");
  char input[1000]; 
	fgets(input, 1000, stdin);
	*memData = strlen(input);
	write(fileDesc, input, strlen(input));
  
  // to set the operation to up
	sb.sem_op = 1;
  // performs operation
	semop(sID, &sb, 1);
}