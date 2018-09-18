#include <unistd.h> 
#include<iostream>
#include<stdlib.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include<netdb.h>
#include <string.h> 
#include<arpa/inet.h>
#include<pthread.h>
#define PORT 8081
using namespace std;
int main(int argc, char const *argv[]) 
{    


    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 
    
    char *hello = "Hello from server";  
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    inet_pton(AF_INET, "10.1.37.17", &address.sin_addr);
    address.sin_port = htons( PORT );
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    while(1)
    { 
    char buffer[1024] = {0}; 
    valread = read( new_socket , buffer, 1024); 
    printf("%s\n",buffer );
    if(string(buffer) == "bye")
        {
            break;
        }
    } 
    







    
    int sock2 = 0, valread2; 
    struct sockaddr_in serv_addr;
    char *hello2 = "Hello from vishal bidawatka"; 
    char buffer2[1024] = {0};  
    sock2 = socket(AF_INET, SOCK_STREAM, 0);
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(8080); 
       
    inet_pton(AF_INET, "10.1.37.221", &serv_addr.sin_addr);
   
    connect(sock2, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
 
    send(sock2 , hello2 , strlen(hello2) , 0 ); 
    printf("Hello message sent\n"); 
    valread2 = read( sock2 , buffer2, 1024); 
    printf("%s\n",buffer2 ); 
    

    






    //sending

    return 0;
}