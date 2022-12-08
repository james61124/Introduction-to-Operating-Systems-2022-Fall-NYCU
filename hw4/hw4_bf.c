#include<stdlib.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/mman.h>
#include<sys/types.h>
#include <stdio.h>

struct block {
    size_t size;
    int free;
    struct block *prev;
    struct block *next;

};


static struct block *mem = NULL;
struct block *start;


struct block *newAllocMem(){
    struct block* ptr;
    //void* res = mmap(NULL,sizeof(struct allocmem),PROT_READ|PROT_WRITE);
    if((ptr = mmap(NULL, 20000, PROT_READ | PROT_WRITE ,MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0))==MAP_FAILED){
        return NULL;
    }
    else{
        ptr->size = 20000;
        ptr->free = 1;
        ptr->next = NULL;
        ptr->prev = NULL;
        return ptr;
    }
}

struct block *find_proper_chunk(size_t size){
    struct block *cur;
    cur = start;
    // char int_str[5];
    // int i = integer_to_string(int_str, cur->size);
    // write(1, int_str ,5);
    while(cur!=NULL){
        
        //write(1, "hehe\n" ,5);
        // char int_str[10];
        // int i = integer_to_string(int_str, cur->size);
        // write(1,int_str,5);
        // write(1," ",1);
        if( cur->size >= size && cur->free == 1){
            // char int_str[10];
            // int i = integer_to_string(int_str, cur->size);
            
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

int integer_to_string(char * string, int number){
   if(number == 0) { string[0] = '0'; return; };
   int divide = 0;
   int modResult;
   int  length = 0;
   int isNegative = 0;
   int  copyOfNumber;
   int offset = 0;
   copyOfNumber = number;
   if( number < 0 ) {
     isNegative = 1;
     number = 0 - number;
     length++;
   }
   while(copyOfNumber != 0) 
   { 
     length++;
     copyOfNumber /= 10;
   }

   for(divide = 0; divide < length; divide++) {
     modResult = number % 10;
     number    = number / 10;
     string[length - (divide + 1)] = modResult + '0';
   }
   if(isNegative) { 
   string[0] = '-';
   }
   string[length] = '\0';
}

struct block *new_address(struct block *proper_chunk, size_t size){
    //printf("haha");
    
    struct block *proper_chunk_original_next;
    
    //write(1, "haha" ,4);
    //write(1, "haha\n" ,5);
    proper_chunk_original_next = proper_chunk->next;
    
    
    struct block *proper_chunk_original_prev;
    proper_chunk_original_prev = proper_chunk->prev;
    
    struct block *new;
    size_t proper_chunk_original_size = proper_chunk->size;
    
    //new = proper_chunk + (proper_chunk->size - size) / 32;
    
    new = proper_chunk;
    if(size / 32 * 32 < size){
        new->size = (size / 32 + 1) * 32;
    }else{
        new->size = ( size / 32 ) * 32;
    }
    
    //new->size = (size / 32 + 1) * 32;

    proper_chunk = proper_chunk + new->size / 32;
    if(proper_chunk == proper_chunk_original_next){
        new->free = 0;
        new->next = proper_chunk_original_next;
        new->prev = proper_chunk_original_prev;
        proper_chunk_original_next->prev = new;
    }else{
        proper_chunk->size = proper_chunk_original_size - new->size;
        proper_chunk->prev = new;
        proper_chunk->next = proper_chunk_original_next;
        proper_chunk->free = 1;
        new->free = 0;
        new->next = proper_chunk;
        new->prev = proper_chunk_original_prev;

    }
 
    if(proper_chunk_original_prev!=NULL){

        // char int_str2[15];
        // sprintf(int_str2, "%p\n", proper_chunk_original_next);
        // write(1, int_str2 ,15);

        proper_chunk_original_prev->next = new;
    }
    return new;
    // if(proper_chunk_original_prev!=NULL){
    //     proper_chunk_original_prev->next = new;
    // }
    
}

void find_max_free_chunk(){
    struct block *cur;
    cur = start;
    size_t max = 0;
    while(cur!=NULL){
        // char int_str[10];
        // int i = integer_to_string(int_str, max);
        // write(1, int_str ,10);
        if(cur->size!=NULL && cur->free!=NULL){
            if( cur->size > max && cur->free == 1){
                // char int_str[10];
                // int i = integer_to_string(int_str, max);
                // write(1, int_str ,10);
                max = cur->size;
            }
        
        }
        cur = cur->next;
        
    }
    char int_str[10];
    int i = integer_to_string(int_str, max - 32);
    write(1, "Max Free Chunk Size = " ,22);
    write(1, int_str ,10);
    write(1, "\n", 1);
    return;
    //printf("%zu\n", max);
    //write(1,max,4);
}





void clean_struct(struct block* ptr){
    ptr->size = NULL;
    ptr->free = NULL;
    ptr->prev = NULL;
    ptr->next = NULL;
}



void *malloc(size_t size){
    if(size==0){
        
        find_max_free_chunk();
        
        int tmp = munmap(start , 20000);
        
        return NULL;
    }
    if(mem==NULL){
        // write(1,"first\n",6);
        mem = newAllocMem();
        
        start = mem;
        struct block *proper_chunk = find_proper_chunk(size+32);
        
        //printf("%zu\n", proper_chunk->size);
        struct block *new = new_address(proper_chunk, size+32);
        
        //find_max_free_chunk();
        
        //write(1,"max",4);
        //write(proper_chunk->size);
        return (void*)(new+1);
    }else{
        // write(1,"second\n",7);
        
        struct block *proper_chunk = find_proper_chunk(size+32);
        
        //printf("%zu\n", proper_chunk->size);
        
        struct block *new = new_address(proper_chunk, size+32);
        
        //find_max_free_chunk();
        
        
        return (void*)(new+1);
    }
    // write(1,"haha",4);
    // void* pass;
    // return pass;
    
}

void free(void* ptr){
    // write(1, "haha\n" ,5);
    struct block* head = ((struct block*)ptr) - 1;
    head->free = 1;
    int tmp = munmap(head + 1, head->size - 32);
    if(head->next!=NULL && head->prev!=NULL){
        struct block* pre = head->prev;
        struct block* back = head->next;
        if(pre->free == 1 && back->free == 1){
            pre->size = head->size + pre->size + back->size;
            pre->next = back->next;
            clean_struct(head);
            clean_struct(back);
        }
        
    }
    if(head->prev!=NULL){
        struct block* pre = head->prev;
        if(pre->free == 1){
            pre->size = head->size + pre->size;
            pre->next = head->next;
            clean_struct(head);
            return;
        }
        
    }
    if(head->next!=NULL){
        struct block* back = head->next;
        if(back->free == 1){
            head->size = head->size + back->size;
            head->next = back->next;
            clean_struct(back);
            return;
        }
        
    }
    


    // char int_str[10];
    // int i = integer_to_string(int_str, head->size);
    // write(1, int_str ,10);
    
    
}


