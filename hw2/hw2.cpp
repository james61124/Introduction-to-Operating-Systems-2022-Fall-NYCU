#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <bits/stdc++.h>
#include <thread>
#include <sys/time.h>
#include <time.h>
#define SIZE 19200000

using namespace std;


struct matrix{
    int length;
    // uint32_t *a;
    // uint32_t *b;
    // uint32_t *c;
    uint32_t a[800][800];
    uint32_t b[800][800];
    uint32_t c[800][800];
};

void calculate(int start, int end, int dimension, struct matrix* shmaddr){
    for(int i=start;i<end;i++){
        for(int j=0;j<dimension;j++){
            for(int k=0;k<dimension;k++){
                //c[i][j] += a[i][k] * b[k][j];
                //int index_a = i * dimension + k;
                //int index_b = k * dimension + j;
                //shmaddr -> c[index_c] += shmaddr -> a[index_a] * shmaddr -> b[index_b];
                shmaddr -> c[i][j] += shmaddr -> a[i][k] * shmaddr -> b[k][j];
                //if(m==1 && i==0 &&j==0) cout<<shmaddr -> a[i][k] * shmaddr -> b[k][j];
            }
            printf("%d %d %d\n",i,j,shmaddr -> c[i][j]);
        }
    }
}

int main(){

    //time_t start, end;
    struct timeval start, end;

    /* shared memory */
    int shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|0600 ) ;
    void *shm = shmat( shmid, NULL, 0 ) ;
    struct matrix* shmaddr = (struct matrix*)shm;

    //int shmid;
    //struct matrix* shmaddr;
    int dimension;
    //shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|0600 ) ;
    printf("input the matrix dimension:");
    //printf("111");
    scanf("%d",&dimension);
    //printf("111");
    if ( shmid < 0 ){
        printf("error");
    }
    //void *shm = shmat( shmid, NULL, 0 ) ;
    //shmaddr = (struct matrix*)shm;
    for(int i=0;i<800;i++){
        for(int j=0;j<800;j++){
            shmaddr -> a[i][j] = 0;
            shmaddr -> b[i][j] = 0;
            shmaddr -> c[i][j] = 0;
        }
    }
    //shmaddr -> a = malloc( dimension * dimension * sizeof(uint32_t) );
    uint32_t a = 0;
    //printf("111");
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            int index = i * dimension + j;
            shmaddr -> a[i][j] = a;
            //shmaddr -> a[index] = a;
            //printf("a %d %d %d\n",i ,j , shmaddr -> a[index]);
            a++;
        }
    }
    uint32_t b=0;
    //shmaddr -> b = malloc( dimension * dimension * sizeof(uint32_t) );
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            int index = i * dimension + j;
            shmaddr -> b[i][j] = b;
            //shmaddr -> b[index] = b;
            //printf("b %d %d %d\n",i ,j , shmaddr -> b[index]);
            b++;
        }
    }
    //shmaddr -> c = malloc( dimension * dimension * sizeof(uint32_t) );
    // for (int i = 0; i < dimension; i++) {
    //     for (int j = 0; j < dimension; j++) {
    //         int index = i * dimension + j;
    //         shmaddr -> c[i][j] = 0;
    //         //shmaddr -> c[index] = 0;
    //         //printf("%d ", shmaddr -> c[index]);
    //     }
    // }
    // shmaddr = shmat( shmid, NULL, 0 ) ;
    for(int m=1;m<17+1;m++){
        for (int i = 0; i < dimension; i++) {
            for (int j = 0; j < dimension; j++) {
                int index = i * dimension + j;
                shmaddr -> c[i][j] = 0;
                //shmaddr -> c[index] = 0;
                //printf("%d ", shmaddr -> c[index]);
            }
        }   

        gettimeofday(&start, 0);


        pid_t pid[20];
        int* index = new int[m+1];
        index[0]=0;
        for(int l=1;l<m+1;l++){
            index[l]=dimension*l/m;
        }     
        //thread mThread[16]; 
        //pid[0] = fork();
        //if(pid[0]==0){
            for(int n=0;n<m;n++){  

                //mThread[n] = thread( calculate, index[n], index[n+1], dimension, shmaddr );

                pid[n] = fork();
                if(pid[n] == 0){
                    for(int i=index[n];i<index[n+1];i++){
                        for(int j=0;j<dimension;j++){
                            //c[i][j] = 0;
                            //int index_c = i * dimension + j;
                            //shmaddr -> c[index_c] = 0;
                            for(int k=0;k<dimension;k++){
                                //c[i][j] += a[i][k] * b[k][j];
                                //int index_a = i * dimension + k;
                                //int index_b = k * dimension + j;
                                //shmaddr -> c[index_c] += shmaddr -> a[index_a] * shmaddr -> b[index_b];
                                shmaddr -> c[i][j] += shmaddr -> a[i][k] * shmaddr -> b[k][j];
                                //if(m==1 && i==0 &&j==0) cout<<shmaddr -> a[i][k] * shmaddr -> b[k][j];
                            }
                            //printf("%d %d %d\n",i,j,shmaddr -> c[index_c]);
                        }
                    }
                    exit(-1);
                }
                //exit(-1);
                //wait(NULL);
            }
            //wait(NULL);
        //}
        
        //wait(NULL);
        // for(int n=0;n<m;n++){
        //     mThread[n].join();
        // }
        //wait(NULL);
        for(int p=0;p<17;p++){
            if(pid[p]>0){
                wait(NULL);
            }
        }
        gettimeofday(&end, 0);


        //double time_taken = double(end - start);
        uint32_t sum = 0;
        for(int i=0;i<dimension;i++){
            for(int j=0;j<dimension;j++){
                //int index = i * dimension + j;
                //sum = sum + shmaddr -> c[index];
                sum = sum + shmaddr -> c[i][j];
                //printf("%d %d %u\n",i,j,shmaddr -> c[i][j]);
                //printf("%d %d %u\n",i,j,shmaddr -> c[index]);
                
            }
        }
        //Multiplying matrices using 1 process
        //Elapsed time: 5.814723 sec, Checksum: 561324032
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;

        cout << "Multiplying matrices using "<<m<<" process"<<endl;
        cout<<"Elapsed time: "<<  sec+(usec/1000000.0) << " sec, Checksum: " << sum <<endl;

        //printf("%d\n", m);
        //printf("%u\n", sum);
        
    }
    
    // pid_t pid;
    // pid = fork();
    // if(pid < 0){
    //     fprintf(stderr, "Fork Failed");
    //     exit(-1);
    // }else if(pid == 0){
    //     //shmaddr = shmat( shmid, NULL, 0 ) ;
    //     for(int i=dimension/2;i<dimension;i++){
    //             for(int j=0;j<dimension;j++){
    //                 //c[i][j] = 0;
    //                 //int index_c = i * dimension + j;
    //                 //shmaddr -> c[index_c] = 0;
    //                 for(int k=0;k<dimension;k++){
    //                     //c[i][j] += a[i][k] * b[k][j];
    //                     //int index_a = i * dimension + k;
    //                     //int index_b = k * dimension + j;
    //                     //shmaddr -> c[index_c] += shmaddr -> a[index_a] * shmaddr -> b[index_b];
    //                     shmaddr -> c[i][j] += shmaddr -> a[i][k] * shmaddr -> b[k][j];
    //                 }
    //                 //printf("%d %d %d\n",i,j,shmaddr -> c[index_c]);
    //             }
    //         }
    //     //memset(shmaddr, ch, size - 1);
    //     //shmdt( shmaddr ) ;
    //     //wait (NULL);
    //     //printf("*****************\n");
    //     exit(-1);
        
        
    // }else{
    //     int a=1;
    //     int b=4;
    //     //shmaddr = shmat( shmid, NULL, 0 ) ;
    //     //shmdt( shmaddr ) ;
    //     //shmaddr = shmat( shmid, NULL, 0 ) ;
    //     for(int i=0;i<dimension/2;i++){
    //         for(int j=0;j<dimension;j++){
    //             //c[i][j] = 0;
    //             //int index_c = i * dimension + j;
    //             //shmaddr -> c[index_c] = 0;
    //             for(int k=0;k<dimension;k++){
    //                 //c[i][j] += a[i][k] * b[k][j];
    //                 //int index_a = i * dimension + k;
    //                 //int index_b = k * dimension + j;
    //                 //shmaddr -> c[index_c] += shmaddr -> a[index_a] * shmaddr -> b[index_b];
    //                 shmaddr -> c[i][j] += shmaddr -> a[i][k] * shmaddr -> b[k][j];
    //             }
    //             //printf("%d %d %u\n",i,j,shmaddr -> c[index_c]);
    //         }
    //     }
    //     //shmdt( shmaddr ) ;
    //     wait (NULL);

    // }
    //shmaddr = shmat( shmid, NULL, 0 ) ;
    
    //printf("%d %d\n",shmaddr->c,shmaddr->d);

}