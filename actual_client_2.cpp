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
void bind_to_soclet(string trackip , string com_from_client , int trackport, string file_name , string myip , int myport, string shastr, float size , string shrget,string fileoutput="default.txt");
vector<string> string_processing2(string s);
vector<string> string_processing3(string s);
void *clientasserver(void *arguments);
void connect_to_client_for_downloading_file(string clientinfowithchunk, string);
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
    pthread_create(&threadarr[0],NULL, clientasserver, &client_info);

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
            string filename;
            int filesize;
            int i = 0;
            while (getline(infile, line))
            {
                if(i == 3)
                {
                   shaforsearch = line; 
                }
                if(i == 4)
                {
                   filename = line;
                }
                if(i == 2)
                {
                   filesize = atoi(line.c_str());
                }
                i++;
            }
        
        cout<<filename<<" "<<filesize<<" "<<shaforsearch<<" "<<endl;
        bind_to_soclet(tracker_ip1,s,portt1, filename, clientip , clientport, shaforsearch , filesize ,"get",file_name_output);

        }
        
        cin.clear();
    }

    return 0;
}
void bind_to_soclet(string trackip , string com_from_client , int trackport, string file_name , string myip , int myport, string shastr, float size , string shorget , string fileoutput)
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
        close(sock);


        vector<string> allips = string_processing3(buffer);
        int no_of_seeders = allips.size();
        int noofchunks = ceil(size/512);
        if (noofchunks = 0)
        {
            noofchunks = 1;
        }
        // for(int i = 0 ; i<no_of_seeders ; i++)
        // {
        //     connect_to_client_for_downloading_file(allips[i]+"|1");
        // }
        connect_to_client_for_downloading_file(allips[0]+"|"+file_name+"|1", fileoutput);
    }
    //send(sock,hello.c_str(),strlen(hello.c_str()),0);
     //char buffer[1024] = {'\0'}; 
    //valread =  read( sock , buffer, 1024);
    //cout<<buffer<<endl;
   // printf("Hello message sent\n"); 
    
    return ;


}
void connect_to_client_for_downloading_file(string clientinfowithchunk, string filenameoutput)
{
    vector<string> clientip = string_processing3(clientinfowithchunk);
    string clientipport =  clientip[0];
    vector<string> ipport = string_processing2(clientipport);
    string clientipuniq = ipport[0];
    int clientportuniq = atoi(ipport[1].c_str());


    //connectingtoclientandgettingdata
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    //string s  = "Hello from client\n\0"; 
    //string hello = "Hello from client"; 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("socket failed\n");
    }
    serv_addr.sin_family = AF_INET; 
    inet_pton(AF_INET, clientipuniq.c_str() , &serv_addr.sin_addr); 
    serv_addr.sin_port = htons(clientportuniq);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return ;
    } 
    string full = clientinfowithchunk;
    int datalen = strlen(full.c_str()); // # of bytes in data
    int tmp = htonl(datalen);
    cout<<tmp;
    int n = write(sock, (char*)&tmp, sizeof(tmp));
    cout<<n<<endl;
    n = send(sock ,full.c_str() , strlen(full.c_str()) , 0 ); 
    cout<<n<<endl;

    FILE * f;
    f = fopen ( filenameoutput.c_str() , "ab");
    char bufferun[512];
    int nuq = read(sock, bufferun, 512);
    //printf("%*.*s\n", n, n, buffer);
    int byteswritten  = fwrite(bufferun,1,512,f);
    fclose(f);
    close(sock);


}
// void *task1(void *arguments)
// {
//     string s = *(string *)arguments;
//     cout << s << endl;
// }
void *clientasserver(void *arguments)
{
    string str = *reinterpret_cast<std::string*>(arguments);
    cout<<"IN SERVER"<<endl;
    cout<<str<<endl;
    vector<string> iport = string_processing2(str);
    string ip = iport[0];
    int port = atoi(iport[1].c_str());
    cout<<ip<<endl;
    cout<<port<<endl;
    int max_client = 64;
    pthread_t threadarrclient[max_client];
    

    int conenctionfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
    address.sin_port = htons(port);

    if (listenfd < 0)
    {
        cerr << "Cannot open socket" << endl;
        return 0;
    }
    if (bind(listenfd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    listen(listenfd,64);
    cout<<"client socket opened succesfully"<<endl;
    
    int noclient = 0; 

    while (1)
    {
        if (noclient < 32)
        {
            conenctionfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            pthread_create(&threadarrclient[noclient], NULL, task1, (void *)&conenctionfd);

            noclient++;
        }
        else
        {
            break;
        }
    }
    
}
void *task1(void *conenctionfd)
{
    int cfd = *(int *)conenctionfd;

    cout<<"First client request recieved"<<endl;
    int buflen;
    int n = read(cfd, (char *)&buflen, sizeof(buflen));
    buflen = ntohl(buflen);
    //cout<<buflen<<endl;
    char buffer[buflen + 1] = {'\0'};
    n = read(cfd, buffer, buflen);
    printf("%*.*s\n", n, n, buffer);


    vector<string> commandrecievedfromanotherclient = string_processing3(buffer);
    string fname_to_give = commandrecievedfromanotherclient[1];
    for(int i = 2 ; i<commandrecievedfromanotherclient.size();i++)
    {
        int chunk_no = atoi(commandrecievedfromanotherclient[i].c_str());
        string document;
        FILE * f;
        f = fopen ( fname_to_give.c_str() , "rb");
        unsigned char buffer2[512];
        while(!feof(f))
        {   
        int bytes = fread(buffer2,1,512,f);
        int bytes2 = send(cfd ,buffer2, 512 , 0 ); 
        cout<<bytes2<<endl;
        for(int i = 0; i < bytes; i++){
        document += buffer2[i];
        cout << buffer2[i];
        }
        }
        fclose ( f );


    }
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
vector<string> string_processing3(string s)
{   //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find('|');
    //cout<<a<<endl;
    while (a != string::npos)
    {   //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find('|', b);
        
    }
   //cout<<s.substr(b, a - b)<<endl;
    splited_string.push_back(s.substr(b, a - b));
    return splited_string;
}
vector<string> string_processing2(string s)
{   //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find(':');
    //cout<<a<<endl;
    while (a != string::npos)
    {   //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find(':', b);
        
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
    fwrite("\n", 1,1,f2);
    fwrite(file_name.c_str(),file_name.size(),1,f2);
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