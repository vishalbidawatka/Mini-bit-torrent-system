#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <unordered_map>
#include <openssl/sha.h>
#include <cmath>
#include<fstream>
using namespace std;
vector<string> string_processing(string s);
int get_file_size(string file_name);
void generating_sha(string file_name);
void *task1(void *arguments);
string generating_sha(string file_name, string mtorrentfile, string t1info , string tip2info ,float &size);
void get_ip_port(string org, string &ip, int &port);
void bind_to_soclet(string trackip , string com_from_client , int trackport, string file_name , string myip , int myport, string shastr, float size , string shrget);
int main(int argc, char **argv)
{
    string clientip;
    int clientport;
    string tracker_ip1;
    string tracker_ip2;
    int portt1;
    int portt2;
    string log_file_name;
    string client_info = string(argv[1]);
    string tracker_1_info = string(argv[2]);
    string tracker_2_info = string(argv[3]);
    get_ip_port(client_info,clientip,clientport);
    get_ip_port(tracker_1_info,tracker_ip1,portt1);
    get_ip_port(tracker_2_info,tracker_ip2,portt2);
    int max_client = 32;
    pthread_t threadarr[32];    
    int noclient = 0;
    vector<string> command;
    while (1)
    {
        string s;
        getline(cin, s);
        vector<string> command;
        command = string_processing(s);
        if (command[0] == "" || command[0] == " ")
        {
            continue;
        }
        if (command[0] == "share")
        {
            string torrent_file_name = command[2];
            string file_name = command[1];
            float size;
            string sha = generating_sha(file_name,torrent_file_name,tracker_1_info,tracker_2_info,size);

            bind_to_soclet(tracker_ip1,s,portt1, file_name, clientip , clientport, sha , size ,"share");

        }
        if (command[0] == "get")
        {
            string torrent_file_name = command[1];
            string file_name_output = command[2];
            string line;
            ifstream infile(torrent_file_name);
            string shaforsearch;
            int i = 0;
            while (getline(infile, line))
            {
                if(i == 3)
                {
                   shaforsearch = line; 
                }
                i++;
            }
        

        bind_to_soclet(tracker_ip1,s,portt1, "file_name", clientip , clientport, shaforsearch , 512 ,"get");

        }
        
        cin.clear();
    }

    return 0;
}
void bind_to_soclet(string trackip , string com_from_client , int trackport, string file_name , string myip , int myport, string shastr, float size , string shorget )
{   

    unsigned char buff[shastr.size()+1] ;
    strcpy( (char*) buff, shastr.c_str() );
    unsigned char obuff[20];
    string sha = "";
    SHA1(buff, shastr.size()+1 , obuff);
    string sName(reinterpret_cast<char *>(obuff));
    sha = sha + sName;


    //server

    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    //string s  = "Hello from client\n\0"; 
    string hello = "Hello from client"; 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("socket failed\n");
    }
    cout<<trackip<<endl;
    cout<<trackport<<endl;
    serv_addr.sin_family = AF_INET; 
    inet_pton(AF_INET, trackip.c_str() , &serv_addr.sin_addr); 
    serv_addr.sin_port = htons(trackport);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return ;
    } 
    // int datalen = strlen(com_from_client.c_str()); // # of bytes in data
    // int tmp = htonl(datalen);
    // cout<<tmp;
    // int n = write(sock, (char*)&tmp, sizeof(tmp));
    // cout<<n<<endl;
    // n = send(sock , com_from_client.c_str() , strlen(com_from_client.c_str()) , 0 ); 
    //cout<<n<<endl;
    if(shorget == "share")
    {
        string full = "";
        full = shorget+";"+file_name+";"+myip+";"+to_string(myport)+";"+shastr+";"+to_string(size);
        cout<<full<<endl;
        int datalen = strlen(full.c_str()); // # of bytes in data
        int tmp = htonl(datalen);
        cout<<tmp;
        int n = write(sock, (char*)&tmp, sizeof(tmp));
        cout<<n<<endl;
        n = send(sock ,full.c_str() , strlen(full.c_str()) , 0 ); 
        cout<<n<<endl;
    }
    if(shorget == "get")
    {   
        string full = "";
        full = shorget+";"+shastr;
        cout<<full<<endl;
        int datalen = strlen(full.c_str()); // # of bytes in data
        int tmp = htonl(datalen);
        cout<<tmp;
        int n = write(sock, (char*)&tmp, sizeof(tmp));
        cout<<n<<endl;
        n = send(sock ,full.c_str() , strlen(full.c_str()) , 0 ); 
        cout<<n<<endl;


        int buflen;
        int n1 = read(sock, (char *)&buflen, sizeof(buflen));
        buflen = ntohl(buflen);
        cout<<buflen<<endl;
        char buffer[buflen + 1] = {'\0'};
        n1 = read(sock, buffer, buflen);
        printf("%*.*s\n", n1, n1, buffer);
        
    }
    //send(sock,hello.c_str(),strlen(hello.c_str()),0);
     //char buffer[1024] = {'\0'}; 
    //valread =  read( sock , buffer, 1024);
    //cout<<buffer<<endl;
   // printf("Hello message sent\n"); 
    close(sock);
    return ;


}

void *task1(void *arguments)
{
    string s = *(string *)arguments;
    cout << s << endl;
}
vector<string> string_processing(string s)
{   //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find(' ');
    //cout<<a<<endl;
    while (a != string::npos)
    {   //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find(' ', b);
        
    }
   //cout<<s.substr(b, a - b)<<endl;
    splited_string.push_back(s.substr(b, a - b));
    return splited_string;
}
string generating_sha(string file_name, string mtorrentfile, string t1info , string tip2info , float &sizeinkb )
{
    FILE *f;
    f = fopen(file_name.c_str(), "rb");
    int siz = get_file_size(file_name);
    sizeinkb = siz / (1000.0);
    int no_of_chunks = ceil((float)sizeinkb / 512.0);
    unsigned char buff[512001] = {'\0'};
    unsigned char obuff[20];
    string sha = "";
    while (int n = fread(buff, 1, 512000 , f))
    {   
        buff[n] = '\0';
        SHA1(buff, 512000, obuff);
        string sName(reinterpret_cast<char *>(obuff));
        sha = sha + sName;
    }
    fclose(f);
    FILE *f2;
    f2 = fopen(mtorrentfile.c_str(), "w");
    fwrite(t1info.c_str(), t1info.size(), 1, f2);
    fwrite("\n", 1,1,f2);
    fwrite(tip2info.c_str(), tip2info.size(), 1, f2);
    fwrite("\n", 1,1,f2);
    fwrite(to_string(sizeinkb).c_str(),to_string(sizeinkb).size(),1,f2);
    fwrite("\n", 1,1,f2);
    fwrite(sha.c_str(),sha.size(),1,f2);
    fclose(f2);
    return sha;
}
int get_file_size(string file_name)
{
    FILE *fp = fopen(file_name.c_str(), "rb");
    fseek(fp, 0L, SEEK_END);
    size_t sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return int(sz);
}
void get_ip_port(string org, string &ip, int &port)
{

    int i = org.find(':', 0);
    ip = org.substr(0, i);
    port = atoi(org.substr(i + 1, org.size()).c_str());
}