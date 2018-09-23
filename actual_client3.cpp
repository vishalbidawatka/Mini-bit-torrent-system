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
using namespace std;
vector<string> string_processing(string s);
int get_file_size(string file_name);
void generating_sha(string file_name);
void *task1(void *arguments);
void generating_sha(string file_name, string mtorrentfile, string t1info , string tip2info );
void get_ip_port(string org, string &ip, int &port);
struct information
{
    vector<string> comand;
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
    get_ip_port(client_info,clientip,clientport);
    get_ip_port(tracker_1_info,tracker_ip1,portt1);
    get_ip_port(tracker_2_info,tracker_ip2,portt2);
    int max_client = 32;
    pthread_t threadarr[32];    
    int noclient = 0;
    vector<string> command;
    int pid = 1;

 while (1)
    {
        string s;
        getline(cin, s);
        vector<string> command;
        info.comand = command;
        command = string_processing(s);
        if (command[0] == "" || command[0] == " ")
        {
            continue;
        }
        if (command[0] == "share")
        {
            string torrent_file_name = command[2];
            string file_name = command[1];
            generating_sha(file_name,torrent_file_name,tracker_1_info,tracker_2_info);
            bind_socket(string tracker_ip1 , int port , string s)

        }
        cin.clear();
    }


    return 0;
}
void *task1(void *arguments)
{   
    cout<<"hello";
    struct information *info = (struct information*)arguments;
    cout<<info->comand[0]<<endl;
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
void generating_sha(string file_name, string mtorrentfile, string t1info , string tip2info )
{
    FILE *f;
    f = fopen(file_name.c_str(), "rb");
    int siz = get_file_size(file_name);
    float sizeinkb = siz / (1000.0);
    int no_of_chunks = ceil((float)sizeinkb / 512.0);
    unsigned char buff[512000];
    unsigned char obuff[20];
    string sha = "";
    while (fread(buff, 1, sizeof buff, f))
    {
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