#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

//struct in order data from notes page
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(int argc, char* argv[]) 
{

  // give user input on how to play
    if (argc != 2) 
    {
        printf("How to use---\n to start, %s create then do ./write \n to end, %s remove\n", argv[0], argv[0]);
        return 1;
    }

  // if creating, make shared memory, semaphore, and open file with truncate

	if (strcmp(argv[1], "create") == 0) 
  {
		int fileData = open("contents.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
		// returns key 
    int k = ftok("contents.txt", 7);
		close(fileData);

    // to create the semaphore, prints out error message if needed
		int semid = semget(k, 1, IPC_CREAT | IPC_EXCL | 0644);
		if (semid == -1) 
    {
			printf("Couldn't make semaphore %s\n", strerror(errno));
			return 1;
		}

    // to initialize semaphore data to value 1
		union semun us;
		us.val = 1;
		int err = semctl(semid, 0, SETVAL, us);
		if (err == -1) 
    {
			printf("Could not set the semaphore value : %s\n", strerror(errno));
			return 1;
		}
		printf("Semaphore made!\n");

    // to make the shared memory
		int memID = shmget(k, sizeof(int), IPC_CREAT | 0644);
		int* memData = shmat(memID, 0, 0);
		*memData = 0;
		shmdt(memData);
		printf("Shared memory made!\n");

    // if removing, remove the shared memory and semaphore, dont forget to display contents fo the story
	} else if (strcmp(argv[1], "remove") == 0) 
  {

    // to remove semaphore
		int k = ftok("contents.txt", 7);
		int semid = semget(k, 1, 0);
		if (semid == -1) 
    {
			printf("Could not remove semaphore 0_0: %s\n", strerror(errno));
			return 1;
		}
		int err = semctl(semid, IPC_RMID, 0);
		if (err == -1) 
    {
			printf("Could not remove semaphore 0_0: %s\n", strerror(errno));
			return 1;
		}
		printf("Semaphore removed\n");

    // to remove shared memory
		int memID = shmget(k, sizeof(int), 0);
		shmctl(memID, IPC_RMID, 0);
		printf("Shared memory removed\n");

    // to print out the contents of the story
		printf("\nFull contents of the Story:\n");
    struct stat s;
    // opens data file
		int fileData = open("contents.txt", O_RDONLY, 0);
		stat("contents.txt", &s);

		char* file = malloc(s.st_size);
		read(fileData, file, s.st_size);
    // prints out the contents of the story
		printf("%s", file);
	} else {
		return 1;
	}
}