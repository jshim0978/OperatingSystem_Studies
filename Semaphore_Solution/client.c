#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int fd,fd2;
    int i;
    int* pmmap;
    int* pmmap2;
    int semaphorevalue;
    int userinputnumber;



    sem_t* startgamewithtwoclient = sem_open("startgamewithtwoclient", 0);
    sem_t* roomoccupied = sem_open("roomoccupied", 0);
    sem_t* flag1 = sem_open("flag1", 0);
    sem_t* flag2 = sem_open("flag2", 0);
    sem_t* finishgame = sem_open("finishgame", 0);
    sem_t* otherclientinputtime = sem_open("otherclientinputtime", 0);
    sem_t* forcorrectvalueprint = sem_open("forcorrectvalueprint", 0);


    if((fd = open("mymmap1.mm", O_RDWR, 0666)) < 0) {
	perror("File open error");
	exit(1);
    }

    if((pmmap = (int *)mmap(0, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, fd ,0)) == NULL) {
	perror("mmap error");
	exit(1);
    }

    if((fd2 = open("mymmap2.mm", O_RDWR, 0666)) < 0) {
	perror("File open error");
	exit(1);
    }

    if((pmmap2 = (int *)mmap(0, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, fd2 ,0)) == NULL) {
	perror("mmap error");
	exit(1);
    }

    sem_getvalue(roomoccupied, &semaphorevalue);
    //printf("roomoccupied = %d",semaphorevalue);
    if(!semaphorevalue) {
	printf("You can't enter this room! \n");
	exit(1);
    }

    sem_wait(roomoccupied);

 

    printf("======================\n");
    printf("     Hello %d\n",getpid());
    printf("     Game Start!!\n");
    printf("======================\n");

    sem_post(startgamewithtwoclient);
    //printf("test");


    while(1) {

		sem_getvalue(flag1, &semaphorevalue);
		//printf("semaphorevalue = %d\n",semaphorevalue);
		//sem_getvalue(flag2, &semaphorevalue);
		//printf("semaphorevalue = %d\n",semaphorevalue);
		if(!semaphorevalue) {
		    sem_wait(flag2);

		    if(pmmap2[0] == 3) {
		    //strike가 3개 나왔을 경우 본인인지 아닌지 판단
			printf("[%d] %d : %d strike, %d ball\n",pmmap2[2], pmmap2[3], pmmap2[0], pmmap2[1]);
			if(pmmap2[2] == getpid()) {
			    printf("You Win !!\n");
			} else {
			    printf("You Lose !!\n");
			}
			sem_post(finishgame);
			break;
		    } else {
		    //상대 결과 출력
			printf("[%d] %d : %d strike, %d ball\n",pmmap2[2], pmmap2[3], pmmap2[0], pmmap2[1]);
		    }

		    sem_post(flag1);
		    sem_post(flag1);
		} else {
		    sem_wait(flag1);
			//printf("semaphorevalue = %d\n",semaphorevalue);

		    printf("Input Number(000 - 999) : ");
		    scanf("%d",&userinputnumber);
		    pmmap[0] = userinputnumber;
		    pmmap[1] = getpid();


		    sem_post(otherclientinputtime);
		    sem_wait(forcorrectvalueprint);

		    sem_post(flag2);

		    if(pmmap2[0] == 3) {
			printf("[%d] %d : %d strike, %d ball\n",pmmap2[2], pmmap2[3], pmmap2[0], pmmap2[1]);
			if(pmmap2[2] == getpid()) {
			    printf("You Win !!\n");
			} else {
			    printf("You Lose !!\n");
			}
				sem_post(finishgame);
				break;
		    } else {
				printf("[%d] %d : %d strike, %d ball\n",pmmap2[2], pmmap2[3], pmmap2[0], pmmap2[1]);
		    }

		    sem_wait(flag1);
		}
    }

    close(fd);
    close(fd2);

    sem_post(roomoccupied);
}