/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include "clientheader.h"

//***************************************************************************
// This function build appropiate complex data that need to send to tracker
// for remove command ==> (command + shorthash(40) + clientsocket)
//***************************************************************************
string executeremoveclient(vector<string> tokens, string clntsckstr)
{
    string cmd = tokens[0];
    string mtpath = tokens[1];
    char *tp = new char[mtpath.length() + 1];
    strcpy(tp, mtpath.c_str());
    ifstream fileptr(mtpath, ifstream::binary);
    if (!fileptr)
    {
        cout << "Mtorrent not exist (REMOVE): " << string(mtpath) << endl;
        return "-1";
    }
    int count = 4;
    string linecontent;
    while ((count > 0) && getline(fileptr, linecontent))
    {
        count--;
    }
    getline(fileptr, linecontent);
    string mtorrenthash = linecontent;
    //cout<<"mtorrent contains hash is : "<<mtorrenthash<<endl;
    fileptr.close();

    char *longhash = new char[mtorrenthash.length() + 1];
    strcpy(longhash, mtorrenthash.c_str());
    string shorthash = calHashofchunk(longhash, mtorrenthash.length(), 0);

    string ans = cmd + "#" + shorthash + "#" + clntsckstr;
    writelog("(REMOVE)Complex Data need to send to tracker :" + ans);
    return ans;
}