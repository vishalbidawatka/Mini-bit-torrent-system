/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include "clientheader.h"

//***************************************************************************
// This called mtorrent creator to create mtorrent file make appropiate
// complex data that need to send to tracker for sharing of file
// (command + shorthash(40) + clientsocket + filepath )
//***************************************************************************
string executeshareclient(vector<string> tokens, string clntsckstr, string trcksck1str, string trcksck2str)
{
    string cmd = tokens[0];
    string fpath = tokens[1];
    string mtpath = tokens[2];
    //cout<<"Command  : "<<cmd<<endl;
    //cout<<"File path : "<<fpath<<endl;
    //cout<<"Mtorrent path : "<<mtpath<<endl;
    char *fp, *tp;
    fp = new char[fpath.length() + 1];
    strcpy(fp, fpath.c_str());

    tp = new char[mtpath.length() + 1];
    strcpy(tp, mtpath.c_str());
    string filehash = createTorrentFile(fp, tp, trcksck1str, trcksck2str);
    if (filehash == "-1")
    {
        writelog("\nError Encounter for creating hash of file in sharing: " + fpath);
        return "-1";
    }
    writelog("share cmd gets Long Hash : " + filehash); //need to do hash of hash
    char *longhash = new char[filehash.length() + 1];
    strcpy(longhash, filehash.c_str());
    string shorthash = calHashofchunk(longhash, filehash.length(), 0);

    string ans = cmd + "#" + shorthash + "#" + clntsckstr + "#" + fpath;
    writelog("(SHARE cmd)Complex Data need to send to tracker :" + ans);
    return ans;
}