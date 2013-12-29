#include<stdio.h>

int front = -1,rear = -1,queue_size = 301;
char* arr[301];


int isQEmpty(){
    if(front == -1){
        return 1;
    }
    else{
        return 0;
    }
}

int isQFull(){
    if (front == (rear + 1) % queue_size)
        return 1;
    else
        return 0;
}

int doQInsert(char * element_to_insert){
    if(isQFull()){
        fprintf(stderr,"Queue is full\n");
        exit(20);
        return 0;
    }
    if(front == -1){
        front = 0;
        rear = 0;
    }
    arr[rear] = element_to_insert;
    rear = (rear + 1) % queue_size;
    return 1;
}

char * doQDelete(){
    char *ret;
    if(isQEmpty()){
        fprintf(stderr,"Queue is empty\n");
        return NULL;
    }
    ret = arr[front];
    front = (front + 1) % queue_size;
    if(front == rear){
        front = rear = -1;
    }
    return ret;
}

/* Returns no. of elements in the queue */
int getCurrentQSize(){
    if(isQEmpty()){
        fprintf(stderr,"current queue size --> %d\n",0);
        return 0;
    }
    int i = front,count=0;
    while(1){
        if(i == rear){
            fprintf(stderr,"current queue size --> %d\n",count);
            break;
        }
        count++;
        i = (i + 1) % queue_size;
    }
    return count;
}

char * getNthElement(int n){
    if(isQEmpty()){
        fprintf(stderr,"getNthelement() : current queue size --> %d\n",0);
        exit(67); //remove
        return NULL;
    }
    int i = front,count=0;
    char *element = NULL;
    while(1){
        if(i == rear){
            fprintf(stderr,"getNthelement(): OutofBound - current queue size --> %d\n",count);
            exit(66); //remove
            break;
        }
        if(count == n){
            element = arr[i];
            break;
        }
        count++;
        i = (i + 1) % queue_size;
    }
    return element;
}

void printQ(){
}

/*
int main(){

    doQInsert(12);
    doQInsert(13);
    doQInsert(15);
    doQInsert(18);
    doQInsert(25);
    printQ();
    getCurrentQSize();
    getFirstQElement();

    doQDelete();
    doQDelete();
    printQ();
    getCurrentQSize();
    getFirstQElement();

    doQInsert(27);
    doQInsert(28);
    printQ();
    getCurrentQSize();
    getFirstQElement();

    doQDelete();
    printQ();
    getCurrentQSize();
    getFirstQElement();
}
*/
