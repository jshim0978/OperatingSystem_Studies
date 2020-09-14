#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

int randomNumber();

int main(int argc, char** argv)
{
    int fd,fd2;
    int i,j;
    int randomnumber;
    int randomnumbers[3];
    int userinputnumber;
    int userinputnumbers[3];
    int* pmmap;
    int* pmmap2;
    int semaphorevalue;
    int zero = 0;
    int temp;
    int checkwithrananduser[3]={-1,};
    int strikecnt;
    int ballcnt;

    sem_t* startgamewithtwoclient; 
    sem_t* roomoccupied;
    sem_t* flag1;
    sem_t* flag2;
    sem_t* otherclientinputtime;
    sem_t* forcorrectvalueprint;
    sem_t* finishgame;


    fd = open("mymmap1.mm", O_RDWR|O_CREAT, S_IRWXU);
    if(fd < 0){
        perror("open");
        exit(1);
    }
    write(fd, &zero, sizeof(int));
    pmmap = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if((unsigned)pmmap == (unsigned)-1){
        perror("mmap");
        exit(1);
    }
    close(fd);


    fd2 = open("mymmap2.mm", O_RDWR|O_CREAT, S_IRWXU);
    if(fd2 < 0){
        perror("open");
        exit(1);
    }
    write(fd2, &zero, sizeof(int));
    pmmap2 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if((unsigned)pmmap2 == (unsigned)-1){
        perror("mmap");
        exit(1);
    }
    close(fd2);



    sem_unlink("startgamewithtwoclient");
    if((startgamewithtwoclient = sem_open("startgamewithtwoclient", O_CREAT, 0644, 0)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }

    sem_unlink("roomoccupied");
    if((roomoccupied = sem_open("roomoccupied", O_CREAT, 0644, 2)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }

    sem_unlink("flag1");
    if((flag1 = sem_open("flag1", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }

    sem_unlink("flag2");
    if((flag2 = sem_open("flag2", O_CREAT, 0644, 0)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }
    
    sem_unlink("finishgame");
    if((finishgame = sem_open("finishgame", O_CREAT, 0644, 0)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }

    sem_unlink("otherclientinputtime");
    if((otherclientinputtime = sem_open("otherclientinputtime", O_CREAT, 0644, 0)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }

    sem_unlink("forcorrectvalueprint");
    if((forcorrectvalueprint = sem_open("forcorrectvalueprint", O_CREAT, 0644, 0)) == SEM_FAILED) {
    perror("Sem Failed");
    exit(1);
    }


    randomnumber = randomNumber();
    temp = randomnumber;
    i=2;
    if(randomnumber<100){
        randomnumbers[0]=0;
    }
    while(randomnumber > 0) {
    randomnumbers[i--] = randomnumber % 10;
    randomnumber /= 10;
    }
    randomnumber = temp;
    printf("Random Number : ");
    for(i=0; i<3; i++) {
    printf("%d",randomnumbers[i]);
    }
    puts("");

    sem_wait(startgamewithtwoclient); //사용자가 둘이 접속했는지 확인
    printf("Client hi\n");

    sem_wait(startgamewithtwoclient);

    printf("Client hi\n");

    printf("=========================\n");
    printf("        Game Start       \n");
    printf("=========================\n");



    while(1) {

    sem_wait(otherclientinputtime); //다른 사용자의 입력을 기다림

//////////////////////////////////////////////////////
    userinputnumber = pmmap[0];
    temp = userinputnumber;

    for(i=2;i>=0;i--){
        userinputnumbers[i] = userinputnumber % 10;
        userinputnumber /=10;
    }
    userinputnumber = temp;
    strikecnt = 0;
    ballcnt = 0;
    //각 자리값 비교해서 strikecnt와 ballcnt 확인
    for(i=0; i<3; i++) {
        if(randomnumbers[i] == userinputnumbers[i]) {
        strikecnt++;
        checkwithrananduser[i]++;
        }
    }

    if(strikecnt == 3) { 
        pmmap2[0] = 3;
        pmmap2[1] = 0;
    } else {
        for(i=0; i<3; i++) {
            for(j=0; j<3; j++) {
                if(i!=j&&userinputnumbers[i] == randomnumbers[j]) {
                    ballcnt++;
                    break;
                }
            }
        }
        pmmap2[0] = strikecnt;
        pmmap2[1] = ballcnt;
    }
    pmmap2[2] = pmmap[1];
    pmmap2[3] = pmmap[0]; 
    if(strikecnt == 3) {
        pmmap2[4] = pmmap[1]; 
    }
    printf("[%d] %d : %d strike, %d ball\n",pmmap2[2],pmmap2[3],strikecnt,ballcnt);
//////////////////////////////////////////////////////

    sem_post(forcorrectvalueprint);
    if(strikecnt == 3) {
        sem_wait(finishgame);
        break;
    }
    }

    printf("\n%d win!!\n",pmmap2[4]);

    sem_wait(roomoccupied);
    sem_wait(roomoccupied);

    sem_close(startgamewithtwoclient);
    exit(0);
}

 int randomNumber(){
    int arr[3];
    int i,j,cnt=1;
    int random=0;
    srand(time(NULL));
    while(cnt){
        cnt=0; 
        for(i=0; i<3; i++){
            arr[i] = (rand() % 10);  
             //printf("%d\t",arr[i]);
        }
        for(i=0; i<3; i++){
            for(j=0; j<3; j++){
                if(i!=j&&arr[i]==arr[j])
                    cnt=1;
                }
        }
    }
    random = arr[0]*100+arr[1]*10+arr[2];
    //printf("%d\n",random);
    return random;
}