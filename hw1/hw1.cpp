#include <bits/stdc++.h> 
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <fcntl.h> 

using namespace std;

int more_than(char** arg,int length){
    int flag=0;
    
    for(int i=0;i<length;i++){
        //cout<<"yes"<<i<<endl;
        if(strcmp(arg[i],">")==0){
            flag=1;
            break;
        }
    }
    //cout<<flag<<endl;
    return flag;
}

int less_than(char** arg,int length){
    int flag=0;
    
    for(int i=0;i<length;i++){
        //cout<<"yes"<<i<<endl;
        if(strcmp(arg[i],"<")==0){
            flag=1;
            break;
        }
    }
    //cout<<flag<<endl;
    return flag;
}

int is_pipe(char** arg,int length){
    int flag=0;
    
    for(int i=0;i<length;i++){
        //cout<<"yes"<<i<<endl;
        if(strcmp(arg[i],"|")==0){
            flag=1;
            break;
        }
    }
    //cout<<flag<<endl;
    return flag;
}

int main(){
    while(true){
        cout<<">";
        string S, T;  
        getline(cin, S);  
        stringstream X(S); 
        char** arg = new char*[100];
        int length=0;
        
        while (getline(X, T, ' ')) { 
            char * writable = new char[T.size()+1];
            copy(T.begin(), T.end(), writable);
            writable[T.size()] = '\0';
            arg[length] = writable;
            length++;
        } 
        arg[length] = NULL;
        

        pid_t pid;
        pid = fork();
        if (pid < 0) { 
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else if (pid == 0) { 
            if(strcmp(arg[length-1],"&")){
                
                if(more_than(arg,length)){
                    //cout<<"111"<<endl;
                    int fd = open(arg[length-1], O_TRUNC|O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fd, STDOUT_FILENO);
                    close(fd); 
                    arg[length-1] = NULL;
                    arg[length-2] = NULL;
                    execvp(arg[0],arg); // ls NULL
                }else if(less_than(arg,length)){
                    int fd_in = open(arg[length-1], O_RDONLY, 0);
                    close(0);
                    dup2(fd_in,STDIN_FILENO);
                    close(fd_in);
                    arg[length-1] = NULL;
                    arg[length-2] = NULL;
                    execvp(arg[0],arg); // ls NULL
                }else if(is_pipe(arg,length)){
                    int index=0;
                    int flag=0;
                    char** command1 = new char*[100];
                    char** command2 = new char*[100];
                    for(int i=0;i<length;i++){
                        if(strcmp(arg[i],"|")==0){
                            flag=1;
                            index=0;
                            continue;
                        }
                        if(flag==0){
                            command1[index] = arg[i];
                        }else{
                            command2[index] = arg[i];
                        }
                        index++;

                    }
                    int fd[2];
                    pid_t childPid;
                    pipe(fd);
                    childPid = fork();
                    if (childPid == 0){
                        dup2(fd[1], 1);
                        close(fd[0]);
                        close(fd[1]);
                        execvp(command1[0], command1);
                    }
                    else{
                        dup2(fd[0], 0);
                        close(fd[0]);
                        close(fd[1]);
                        execvp(command2[0], command2);
                    }
                }else{
                    //cout<<"112"<<endl;
                    execvp(arg[0],arg);
                }
            }else{
                pid_t pid2;
                pid2 = fork();
                if(pid2 == 0){
                    arg[length-1] = NULL;
                    execvp(arg[0],arg);
                }else{
                    wait (NULL);
                }
            }
            
            
        }
        else { 
            if(strcmp(arg[length-1],"&")){
                //cout<<"1"<<endl;
                wait (NULL);
            }else{
                //cout<<"0"<<endl;
            }
            
        }
    }
    
}


