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
using namespace std;
void *task1(void *);
// class tracker
// {
//     int max_client = 32;
//     string seeder_file_list_file;
//     string log_file_name;
//     string tracker1_ip;
//     int tracker1_port;
//     tracker(string ip,int port, string log_file, string seeder_list)
//     {
//         tracker1_port = port;
//         tracker1_ip = ip;
//         log_file_name = log_file;
//         seeder_file_list_file = seeder_list;
//     }
//     void setting_tracker()
//     {
//     int conenctionfd;
//     pthread_t threadarr[max_client];
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
//     int listenfd = socket(AF_INET, SOCK_STREAM , 0);
//     address.sin_family = AF_INET;
//     inet_pton(AF_INET, tracker1_ip.c_str() , &address.sin_addr);
//     address.sin_port = htons( tracker1_port );
//     }
// },
class information_of_files
{
  public:
    //vector<string> hashes;
    unordered_map<string, string> filenames;
    unordered_map<string, string> global_seeder_list;
    unordered_map<string, int> sizes;
};

information_of_files global_file_manager;

vector<string> string_processing(string s)
{ //cout<<s<<endl;
    size_t a, b = 0;
    vector<string> splited_string;
    a = s.find(';');
    //cout<<a<<endl;
    while (a != string::npos)
    { //cout<<s.substr(b, a - b)<<endl;
        splited_string.push_back(s.substr(b, a - b));
        b = a + 1;
        a = s.find(';', b);
    }
    //cout<<s.substr(b, a - b)<<endl;
    splited_string.push_back(s.substr(b, a - b));
    return splited_string;
}

int main(int argc, char **argv)
{
    int max_client = 32;
    int PORT;
    string tracker_1_info;
    string tracker_2_info;
    string seeder_file_list_file;
    string log_file_name;
    string tracker1_ip;
    int tracker1_port;
    tracker_1_info = string(argv[1]);
    tracker_2_info = string(argv[2]);
    seeder_file_list_file = string(argv[3]);
    log_file_name = string(argv[4]);

    int i = tracker_1_info.find(':', 0);
    tracker1_ip = tracker_1_info.substr(0, i);
    tracker1_port = atoi(tracker_1_info.substr(i + 1, tracker_1_info.size()).c_str());

    //socketopen

    int conenctionfd;
    pthread_t threadarr[max_client];
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, tracker1_ip.c_str(), &address.sin_addr);
    address.sin_port = htons(tracker1_port);

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
    listen(listenfd, max_client);
    int noclient = 0;
    while (1)
    {
        if (noclient < 32)
        {
            conenctionfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            pthread_create(&threadarr[noclient], NULL, task1, (void *)&conenctionfd);

            noclient++;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < noclient; i++)
    {
        pthread_join(threadarr[i], NULL);
    }

    return 0;
}
void *task1(void *conenctionfd)
{
    int cfd = *(int *)conenctionfd;
    //char buffer[1024] = {0};
    int buflen;
    int n = read(cfd, (char *)&buflen, sizeof(buflen));
    buflen = ntohl(buflen);
    //cout<<buflen<<endl;
    char buffer[buflen + 1] = {'\0'};
    n = read(cfd, buffer, buflen);
    printf("%*.*s\n", n, n, buffer);
    vector<string> recieved_from_command = string_processing(buffer);

    if (recieved_from_command[0] == "share")
    {
        string has = recieved_from_command[4];
        cout << has << endl;
        global_file_manager.filenames[has] = recieved_from_command[1];
        cout << global_file_manager.filenames[has] << endl;
        if (global_file_manager.global_seeder_list.find(has) == global_file_manager.global_seeder_list.end())
        {
            cout << recieved_from_command[3] << endl;
            global_file_manager.global_seeder_list[has] = global_file_manager.global_seeder_list[has] + recieved_from_command[2] + ":" + recieved_from_command[3];
        }
        else
            global_file_manager.global_seeder_list[has] = global_file_manager.global_seeder_list[has] + "|" + recieved_from_command[2] + ":" + recieved_from_command[3];

        cout << global_file_manager.global_seeder_list[has] << endl;
        global_file_manager.sizes[has] = atoi(recieved_from_command[5].c_str());
        //cout<<global_file_manager.sizes[has]<<endl;
    }
    if (recieved_from_command[0] == "get")
    {
        cout << recieved_from_command[1] << endl;
        string full = global_file_manager.global_seeder_list[recieved_from_command[1]];
        cout<<full.c_str()<<endl;

        int datalen = strlen(full.c_str()); // # of bytes in data
        int tmp = htonl(datalen);
        cout << tmp;
        int n = write(cfd, (char *)&tmp, sizeof(tmp));
        cout << n << endl;
        n = send(cfd, full.c_str(), strlen(full.c_str()), 0);
        cout << n << endl;
        // s= s+global_file_manager.global_seeder_list[recieved_from_command[1]].
    }

    // for ( int i = n ;i<1024 ; i++)
    // buffer[i] = '\0';
    // global_file_manager.filenames.push_back(string(buffer));
    // for(int i = 0 ; i<1024 ; i++)
    // {
    //     cout<<buffer[i]<<endl;
    // }
    // cout<<"HELLO"<<endl;
    // cout<<buffer<<endl;
    // cout<<endl;
    // char buffer2[1024] = {0};
    // n  = read(cfd,buffer2,1024);
    // for ( int i = n ;i<1024 ; i++)
    // buffer2[i] = '\0';

    // global_file_manager.filenames.push_back(string(buffer2));
    // cout<<string(buffer2)<<endl;
    //cout<<string(buffer);

    //printf("%s",buffer);
}

//send(cfd,buffer,bufSize,0);
