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
#include <fstream>
#include <algorithm>
using namespace std;
vector<string> string_processing(string s);
int get_file_size(string file_name);
//void generating_sha(string file_name);
void *task1(void *arguments);
string generating_sha(string file_name, string mtorrentfile, string t1info, string tip2info, int &size);
void get_ip_port(string org, string &ip, int &port);
void bind_to_soclet(string trackip, string com_from_client, int trackport, string file_name, string myip, int myport, string shastr, int size, string shrget, string fileoutput = "default.txt");
vector<string> string_processing2(string s);
vector<string> string_processing3(string s);
void *clientasserver(void *arguments);
void *connect_to_client_for_downloading_file(void *arguments);
void send_client_the_file(string filename, int size, int cfd, int chunk_num);
void download_file_from_client(string filenameoutput, int sock);
void merge_all_file(string sha, string outputfilename,int no_of_seeders);
struct thread_arg
{
    string clientinfo;
};
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
    get_ip_port(client_info, clientip, clientport);
    get_ip_port(tracker_1_info, tracker_ip1, portt1);
    get_ip_port(tracker_2_info, tracker_ip2, portt2);
    int max_client = 32;
    pthread_t threadarr[32];
    int noclient = 0;
    vector<string> command;
    pthread_create(&threadarr[0], NULL, clientasserver, &client_info);

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
            int size;
            string sha = generating_sha(file_name, torrent_file_name, tracker_1_info, tracker_2_info, size);

            bind_to_soclet(tracker_ip1, s, portt1, file_name, clientip, clientport, sha, size, "share");
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
                if (i == 3)
                {
                    shaforsearch = line;
                }
                if (i == 4)
                {
                    filename = line;
                }
                if (i == 2)
                {
                    filesize = atoi(line.c_str());
                }
                i++;
            }

            cout << filename << " " << filesize << " " << shaforsearch << " " << endl;
            bind_to_soclet(tracker_ip1, s, portt1, filename, clientip, clientport, shaforsearch, filesize, "get", file_name_output);
        }

        cin.clear();
    }

    return 0;
}
void bind_to_soclet(string trackip, string com_from_client, int trackport, string file_name, string myip, int myport, string shastr, int size, string shorget, string fileoutput)
{

    unsigned char buff[shastr.size() + 1];
    strcpy((char *)buff, shastr.c_str());
    unsigned char obuff[20];
    string sha = "";
    SHA1(buff, shastr.size() + 1, obuff);
    string sName(reinterpret_cast<char *>(obuff));
    sha = sha + sName;

    //server

    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    //string s  = "Hello from client\n\0";
    string hello = "Hello from client";
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("socket failed\n");
    }
    cout << trackip << endl;
    cout << trackport << endl;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, trackip.c_str(), &serv_addr.sin_addr);
    serv_addr.sin_port = htons(trackport);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
    // int datalen = strlen(com_from_client.c_str()); // # of bytes in data
    // int tmp = htonl(datalen);
    // cout<<tmp;
    // int n = write(sock, (char*)&tmp, sizeof(tmp));
    // cout<<n<<endl;
    // n = send(sock , com_from_client.c_str() , strlen(com_from_client.c_str()) , 0 );
    //cout<<n<<endl;
    if (shorget == "share")
    {
        string full = "";
        full = shorget + ";" + file_name + ";" + myip + ";" + to_string(myport) + ";" + shastr + ";" + to_string(size);
        cout << full << endl;
        int datalen = strlen(full.c_str()); // # of bytes in data
        int tmp = htonl(datalen);
        cout << tmp;
        int n = write(sock, (char *)&tmp, sizeof(tmp));
        cout << n << endl;
        n = send(sock, full.c_str(), strlen(full.c_str()), 0);
        cout << n << endl;
    }
    if (shorget == "get")
    {
        string full = "";
        full = shorget + ";" + shastr;
        cout << full << endl;
        int datalen = strlen(full.c_str()); // # of bytes in data
        int tmp = htonl(datalen);
        cout << tmp;
        int n = write(sock, (char *)&tmp, sizeof(tmp));
        cout << n << endl;
        n = send(sock, full.c_str(), strlen(full.c_str()), 0);
        cout << n << endl;
        pthread_t threadarr3[32];

        int buflen;
        int n1 = read(sock, (char *)&buflen, sizeof(buflen));
        buflen = ntohl(buflen);
        cout << buflen << endl;
        char buffer[buflen + 1] = {'\0'};
        n1 = read(sock, buffer, buflen);
        printf("%*.*s\n", n1, n1, buffer);
        close(sock);

        vector<string> allips = string_processing3(buffer);
        int no_of_seeders = allips.size();
        int size_of_chunk = ceil((float)size / (float)no_of_seeders);
        // if (noofchunks = 0)
        // {
        //     noofchunks = 1;
        // }
        cout << "number of seeders" << no_of_seeders << endl;
        cout << "size of each chunk" << size_of_chunk << endl;
        string each_client_info;
        struct thread_arg args[no_of_seeders];
        for (int i = 0; i < no_of_seeders; i++)
        {
            args[i].clientinfo = allips[i] + "|" + file_name + "|" + to_string(size_of_chunk) + "|" + to_string(i + 1) + "|" + shastr.substr(0,20) + "_" + to_string(i + 1);
            //each_client_info = allips[i]+"|"+file_name+"|"+to_string(size_of_chunk)+"|"+to_string(i+1)+"|"+"vishal"+"_"+to_string(i+1);
            cout << "each client info" << args[i].clientinfo << endl;
            pthread_create(&threadarr3[i], NULL, &connect_to_client_for_downloading_file, &args[i]);
            sleep(0.5);
        }
        // connect_to_client_for_downloading_file(allips[0]+"|"+file_name+"|"+to_string(size_of_chunk)+"|1", fileoutput);

        for (int j = 0; j < no_of_seeders; j++)
        {
            pthread_join(threadarr3[j], NULL);
        }
       merge_all_file(shastr.substr(0,20), fileoutput ,no_of_seeders);
    }
}
void merge_all_file(string sha, string outputfilename,int no_of_seeders)
{   
    string command;
    for(int i = 0;i<no_of_seeders ; i++)
    {
    command = "cat "+sha+"_"+to_string(i+1)+">> "+outputfilename;

    system(command.c_str());
    }
    command = "rm "+sha+"_"+"*";
    system(command.c_str());

}
//send(sock,hello.c_str(),strlen(hello.c_str()),0);
//char buffer[1024] = {'\0'};
//valread =  read( sock , buffer, 1024);
//cout<<buffer<<endl;
// printf("Hello message sent\n");

void *connect_to_client_for_downloading_file(void *arguments)
{
    cout << "in threaf of connecting client" << endl;
    struct thread_arg *clientinfostruct = (thread_arg *)arguments;
    string clientinfowithchunk = clientinfostruct->clientinfo;
    cout << "in threaf of connecting client" << clientinfowithchunk << endl;
    vector<string> clientip = string_processing3(clientinfowithchunk);
    string clientipport = clientip[0];
    cout << clientipport << endl;
    vector<string> ipport = string_processing2(clientipport);
    string clientipuniq = ipport[0];
    int clientportuniq = atoi(ipport[1].c_str());
    string filenameoutput = clientip[4];
    cout << filenameoutput << endl;

    //connectingtoclientandgettingdata
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    //string s  = "Hello from client\n\0";
    //string hello = "Hello from client";
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("socket failed\n");
    }
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, clientipuniq.c_str(), &serv_addr.sin_addr);
    serv_addr.sin_port = htons(clientportuniq);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
    }
    string full = clientinfowithchunk;
    int datalen = strlen(full.c_str()); // # of bytes in data
    int tmp = htonl(datalen);
    cout << tmp;
    int n = write(sock, (char *)&tmp, sizeof(tmp));
    cout << n << endl;
    n = send(sock, full.c_str(), strlen(full.c_str()), 0);
    cout << n << endl;

    // FILE * f;
    // f = fopen ( filenameoutput.c_str() , "ab");
    // char bufferun[512];
    // int nuq = read(sock, bufferun, 512);
    // //printf("%*.*s\n", n, n, buffer);
    // int byteswritten  = fwrite(bufferun,1,512,f);
    // fclose(f);
    // close(sock);

    download_file_from_client(filenameoutput, sock);
}
void download_file_from_client(string filenameoutput, int sock)
{
    FILE *f;
    int buflen = 1;
    int n1;
    int n2;
    cout<<"in file download "<<filenameoutput<<endl;
    f = fopen(filenameoutput.c_str(), "w");
 
        cout<<"in downloading"<<endl;
        n1 = read(sock, (char *)&buflen, sizeof(buflen));
        buflen = ntohl(buflen);
        cout<<buflen<<"buflen"<<endl;

        char buffer[buflen];

        n2 = read(sock, buffer, sizeof(buffer));
        int byteswritten = fwrite(buffer, 1, buflen, f);
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
    string str = *reinterpret_cast<std::string *>(arguments);
    cout << "IN SERVER" << endl;
    cout << str << endl;
    vector<string> iport = string_processing2(str);
    string ip = iport[0];
    int port = atoi(iport[1].c_str());
    cout << ip << endl;
    cout << port << endl;
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
    listen(listenfd, 64);
    cout << "client socket opened succesfully" << endl;

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

    cout << "First client request recieved" << endl;
    int buflen;
    int n = read(cfd, (char *)&buflen, sizeof(buflen));
    buflen = ntohl(buflen);
    //cout<<buflen<<endl;
    char buffer[buflen + 1] = {'\0'};
    n = read(cfd, buffer, buflen);
    printf("%*.*s\n", n, n, buffer);

    vector<string> commandrecievedfromanotherclient = string_processing3(buffer);
    string fname_to_give = commandrecievedfromanotherclient[1];
    int chunk_size = atoi(commandrecievedfromanotherclient[2].c_str());
    int chunk_num = atoi(commandrecievedfromanotherclient[3].c_str());
    int filesize_from_client = get_file_size(fname_to_give);
    cout << chunk_size << " " << chunk_num << " "
         << "chunksize and chunk num" << endl;
    send_client_the_file(fname_to_give, chunk_size, cfd, chunk_num);
    // for(int i = 2 ; i<commandrecievedfromanotherclient.size();i++)
    // {
    //     int chunk_no = atoi(commandrecievedfromanotherclient[i].c_str());
    //     string document;
    //     FILE * f;
    //     f = fopen ( fname_to_give.c_str() , "rb");
    //     unsigned char buffer2[512];
    //     while(!feof(f))
    //     {
    //     int bytes = fread(buffer2,1,512,f);
    //     int bytes2 = send(cfd ,buffer2, 512 , 0 );
    //     cout<<bytes2<<endl;
    //     for(int i = 0; i < bytes; i++){
    //     document += buffer2[i];
    //     cout << buffer2[i];
    //     }
    //     }
    //     fclose ( f );

    // }
}
void send_client_the_file(string filename, int size, int cfd, int chunknum)
{
    int datalen;
    int tmp;
    int n;
    FILE *f;
    f = fopen(filename.c_str(), "rb");
    // if(size < 512000)
    // {
    if(f == NULL)
    {
        cout<<"cant open file"<<endl;
    }
    else
    {
        cout<<"file opened"<<endl;
    }

    char buffer[size] = {'\0'};
    fseek(f, (chunknum - 1) * size, 0);
    int bytes = fread(buffer, 1, size, f);
    
    if(bytes<size)
    {
        datalen = bytes;
    }
    else
    {
        datalen = size;
    }
    
    tmp = htonl(datalen);
    cout << tmp<<endl;
    n = write(cfd, (char *)&tmp, sizeof(tmp));

    cout<<"bytes read from file "<<bytes<<endl;
    int bytes2 = send(cfd, buffer, datalen, 0);
    cout<<"bytes sent" << bytes2;
    // datalen = 0;
    // tmp = htonl(datalen);
    // cout << tmp<<endl;
    // n = write(cfd, (char *)&tmp, sizeof(tmp));

    //}

    // else
    // {
    //     int nochunks = size/512000;
    //     while(nochunks--)
    //     {
    //         datalen = 512000; // # of bytes in data
    //         tmp = htonl(datalen);
    //         cout<<tmp;
    //         n = write(cfd, (char*)&tmp, sizeof(tmp));
    //         char buffer[datalen];
    //         int bytes = fread(buffer,1,datalen,f);
    //         int bytes2 = send(cfd,buffer,datalen,0);
    //         datalen = 512000;
    //         tmp = htonl(datalen);
    //         cout<<tmp;
    //         n = write(cfd, (char*)&tmp, sizeof(tmp));

    //     }
    // int remaining  = size - 512000*nochunks;
    // if(remaining = 0)
    // {
    //     datalen = 0;
    //     tmp = htonl(datalen);
    //     cout<<tmp;
    //     n = write(cfd, (char*)&tmp, sizeof(tmp));

    // }
    // else
    // {
    //     datalen = remaining; // # of bytes in data
    //     tmp = htonl(datalen);
    //     cout<<tmp;
    //     n = write(cfd, (char*)&tmp, sizeof(tmp));
    //     char buffer[datalen];
    //     int bytes = fread(buffer,1,datalen,f);
    //     int bytes2 = send(cfd,buffer,datalen,0);
    //     datalen = 0;
    //     tmp = htonl(datalen);
    //     cout<<tmp;
    //     n = write(cfd, (char*)&tmp, sizeof(tmp));

    // }
    //}
}
vector<string> string_processing(string s)
{ //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find(' ');
    //cout<<a<<endl;
    while (a != string::npos)
    { //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find(' ', b);
    }
    //cout<<s.substr(b, a - b)<<endl;
    splited_string.push_back(s.substr(b, a - b));
    return splited_string;
}
vector<string> string_processing3(string s)
{ //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find('|');
    //cout<<a<<endl;
    while (a != string::npos)
    { //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find('|', b);
    }
    //cout<<s.substr(b, a - b)<<endl;
    splited_string.push_back(s.substr(b, a - b));
    return splited_string;
}
vector<string> string_processing2(string s)
{ //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find(':');
    //cout<<a<<endl;
    while (a != string::npos)
    { //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find(':', b);
    }
    //cout<<s.substr(b, a - b)<<endl;
    splited_string.push_back(s.substr(b, a - b));
    return splited_string;
}
string generating_sha(string file_name, string mtorrentfile, string t1info, string tip2info, int &sizeinkb)
{
    FILE *f;
    f = fopen(file_name.c_str(), "rb");
    int siz = get_file_size(file_name);
    sizeinkb = siz;
    int no_of_chunks = ceil((float)sizeinkb / 512.0);
    unsigned char buff[512001] = {'\0'};
    unsigned char obuff[20];
    string sha = "";
    while (int n = fread(buff, 1, 512000, f))
    {
        buff[n] = '\0';
        SHA1(buff, 512000, obuff);
        char out[41]; //null terminator
        for (int i = 0; i < 20; i++) {
        snprintf(out+i*2, 4, "%02x ", obuff[i]);
        }
        ///string sName(reinterpret_cast<char *>(obuff));
        sha = sha + string(out).substr(0,20);
    }
    fclose(f);
    FILE *f2;
    f2 = fopen(mtorrentfile.c_str(), "w");
    fwrite(t1info.c_str(), t1info.size(), 1, f2);
    fwrite("\n", 1, 1, f2);
    fwrite(tip2info.c_str(), tip2info.size(), 1, f2);
    fwrite("\n", 1, 1, f2);
    fwrite(to_string(sizeinkb).c_str(), to_string(sizeinkb).size(), 1, f2);
    fwrite("\n", 1, 1, f2);
    replace(sha.begin(), sha.end(), '\n', 'x');
    fwrite(sha.c_str(), sha.size(), 1, f2);
    fwrite("\n", 1, 1, f2);
    fwrite(file_name.c_str(), file_name.size(), 1, f2);
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