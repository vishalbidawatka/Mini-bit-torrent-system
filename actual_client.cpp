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
#include <vector>
#include<unordered_map>
using namespace std;
void *task1( void *arguments);
class client 
{   
    public:
    string clientip;
    int clientport;
    string tracker_ip1;
    string tracker_ip2;
    int portt1;
    int portt2;
    string log_file_name;
    int max_client = 32;
    pthread_t threadarr[32];
    string command;
    client(string ip, int port, string tip1 , string tip2, int p1, int p2 ,string log2)
    {
        clientip = ip;
        clientport = port;
        tracker_ip1 = tip1;
        tracker_ip2 = tip2;
        p1 = portt1;
        p2 = portt2;
        log_file_name = log2;
        
    }
    void serve_forever()
    {   
        int noclient = 0;
        while(1)
        {
            string temp = "";
            cin>>temp;
            if(temp != "")
            {
            pthread_create(&threadarr[noclient], NULL, task1, (void *)); 
            }
            noclient++;
        }
    }




};
void *task1( void *arguments)
{
    client *clientdata = (client *)arguments;
    cout<<clientdata->command;
    cout<<clientdata->log_file_name;
}
int main()
{

    client trial("1.2.3.4",1234,"1.2.3.4","1.2.3.4",1234,1234,"logs.txt");
    trial.serve_forever();

}