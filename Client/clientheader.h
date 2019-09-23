/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include<bits/stdc++.h>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <dirent.h>

using namespace std;

extern const char *logpath;
// extern int globalcurchunksize;

//mtorrent generator's functions
string calHashofchunk(char *schunk,int length1,int shorthashflag);
string getFileHash(char *fpath);
string createTorrentFile(char *fpath, char *mtpath,string tcksocket1,string tcksocket2);

//Client Commands
string executeshareclient(vector <string> tokens,string clntsckstr,string trcksck1str,string trcksck2str);
string executegetclient(vector <string> tokens);
string executeremoveclient(vector <string> tokens,string clntsckstr);

void *seederserverservice(void *socket_desc);

void writelog(string str);
