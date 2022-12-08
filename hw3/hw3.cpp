#include <iostream>
#include <fstream>
#include <cstdlib>
#include <mutex>
#include <pthread.h>
#include <vector>
#include <queue>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define MAXLINE 1000005

struct timeval start, end1;
int num_thread;
int output_time;
int finished_job[15];
int pushed_job[15];
int child_job[15] = {8,8,9,9,10,10,11,11,12,12,13,13,14,14,15};
queue<int>job_list;
sem_t job_number;
sem_t _mutex;
int buffer[MAXLINE];
int length;
int thread_index;




void bubble_sort(int begin, int end){
    //cout<<"bubblestart"<<endl;
    for(int i = end; i > 0; i--){
        for(int j = begin; j <= i-1; j++){
            if( buffer[j] > buffer[j+1]){
                int tmp = buffer[j];
                buffer[j] = buffer[j+1];
                buffer[j+1] = tmp;
            }
        }
    }
    //cout<<"bubblefinish"<<endl;
}

void merge(int const left, int const mid,int const right){
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;
 
    // Create temp arrays
    auto *leftArray = new int[subArrayOne],
         *rightArray = new int[subArrayTwo];
 
    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = buffer[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = buffer[mid + 1 + j];
 
    auto indexOfSubArrayOne
        = 0, // Initial index of first sub-array
        indexOfSubArrayTwo
        = 0; // Initial index of second sub-array
    int indexOfMergedArray
        = left; // Initial index of merged array
 
    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne
           && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne]
            <= rightArray[indexOfSubArrayTwo]) {
            buffer[indexOfMergedArray]
                = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else {
            buffer[indexOfMergedArray]
                = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        buffer[indexOfMergedArray]
            = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        buffer[indexOfMergedArray]
            = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}
 
// begin is for left index and end is
// right index of the sub-array
// of arr to be sorted */
void merge_sort(int begin, int end)
{
    if (begin >= end)
        return; // Returns recursively
 
    auto mid = begin + (end - begin) / 2;
    merge_sort(begin, mid);
    merge_sort(mid + 1, end);
    merge(begin, mid, end);
}

void job_content(int job){
    if(job >= 0 && job <= 7){
        bubble_sort(length/8*job, length/8*(job+1)-1);
    }else if(job >= 8 && job <= 11){
        int tmp_job = job - 8;
        merge_sort(length/4*tmp_job,length/4*(tmp_job+1)-1);
    }else if(job >= 12 && job <= 13){
        int tmp_job = job - 12;
        merge_sort(length/2*tmp_job,length/2*(tmp_job+1)-1);
    }else if(job == 14){
        int tmp_job = job - 14;
        merge_sort(length*tmp_job,length*(tmp_job+1)-1);
    }
}

void* sort(void *threadid){
    //sem_wait(&binSem);
    //sem_post(&binSem);
    //cout<<"haha"<<endl;
    while(true){
        //cout<<"job"<<endl;
        sem_wait(&job_number);
        sem_wait(&_mutex);
        int *a;
        int job_index = job_list.front();
        //cout<<&threadid<<" "<<job_list.front()<<endl;
        //int number = sem_getvalue(&_mutex,a);
        //cout<<"haha"<<endl;
        //cout<<"number:"<<number<<endl;
        job_list.pop();
        sem_post(&_mutex);
        //number = sem_getvalue(&_mutex,a);
        //cout<<"number:"<<number<<endl;
        //do the job
        sem_wait(&_mutex);
        //cout<<&threadid<<"do the job"<<job_index<<endl;
        sem_post(&_mutex);
        job_content(job_index);
        sem_wait(&_mutex);
        //cout<<&threadid<<"finish the job"<<job_index<<endl;
        finished_job[job_index] = 1;
        sem_post(&_mutex);
        //cout<<"finished"<<endl;
        sem_wait(&_mutex);
        if(job_index%2==0 && job_index!=14){
            if(finished_job[job_index+1]==1 && pushed_job[child_job[job_index]] == 0){
                job_list.push(child_job[job_index]);
                pushed_job[child_job[job_index]]=1;
                //cout<<&threadid<<" push: "<<child_job[job_index]<<endl;
                sem_post(&job_number);
                //cout<<"haha1"<<endl;
            }
        }else if(job_index%2==1 && job_index!=15){
            if(finished_job[job_index-1]==1 && pushed_job[child_job[job_index]] == 0){
                job_list.push(child_job[job_index]);
                pushed_job[child_job[job_index]]=1;
                //cout<<&threadid<<" push: "<<child_job[job_index]<<endl;
                sem_post(&job_number);
                //cout<<"haha2"<<endl;
            }
        }else{
            job_list.push(15);
            sem_post(&job_number);
            break;

        }
        sem_post(&_mutex);
    }
    output_time--;
    if(output_time == 0){
        job_list.pop();
        
        gettimeofday(&end1, 0);
        //cout<<"time end"<<end1.tv_sec<<" "<<start.tv_sec<<endl;
        int sec = end1.tv_sec - start.tv_sec;
        int usec = end1.tv_usec - start.tv_usec;
        cout<<"worker thread #"<<thread_index<<", elapsed "<<  (sec*1000)+(usec/1000000.0) <<" ms"<<endl;
    }
    sem_post(&_mutex);
    return 0;
}

int main(){
    // int buffer_len = 100000;
    // ofstream myfile ("input.txt");
    // if (myfile.is_open()){
    //     for(int i=0;i<buffer_len;i++){
    //         myfile << rand();
    //         //myfile << 1000-i;
    //         myfile << " ";
    //     }
    // }
    // myfile.close();
    


    thread_index = 1;
    for(int i=1;i<=8;i++){
        //cout<<"iiiii"<<i<<endl;
        length = 0;
        
        int thread = 0;
        int res;
        // Semaphore 初始化
        res = sem_init(&job_number, 0, 8);
        res = sem_init(&_mutex, 0, 1);
        for(int i=0;i<MAXLINE;i++){
            buffer[i]=0;
        }
        
        //buffer input
        ifstream fin("input.txt"); 
        while(!fin.eof()) { 
            fin >> buffer[length];
            length++;
        } 
        length--;
        //cout<<length<<endl;

        for(int i=0;i<15;i++){
            finished_job[i] = 0;
            pushed_job[i] = 0;
        }
        

        //job_list initialize
        for(int i=0;i<8;i++){
            job_list.push(i);
            pushed_job[i]=1;
        }
        num_thread = i;
        output_time = num_thread;
        
        pthread_t threads[num_thread];
        //cout<<"time start"<<endl;
        gettimeofday(&start, 0);
        for(int j=0; j<num_thread; j++){
            pthread_create(&threads[j], NULL, sort, NULL);
        }
        for(int j=0; j<num_thread; j++){
            pthread_join(threads[j], NULL);
        }
        
        //cout<<"output"<<endl;
        string path = "output" + to_string(thread_index) + ".txt";
        ofstream myfile (path);
        if (myfile.is_open()){
            for(int i=0;i<length;i++){
                myfile << buffer[i];
                myfile << " ";
            }
        }
        myfile.close();

        thread_index++;
        
    }
    
    
    
    pthread_exit(NULL);
    //pthread_exit(NULL);
    
    return 0;

}