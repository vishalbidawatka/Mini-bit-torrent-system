/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include "clientheader.h"

#define CSIZE 512 * 1024

char *filepath, *mtorrentpath;

//***************************************************************************
// It calculate hash(SHA1) of given data chunk
// shorthashflag = 1 means return 20 character of hash
// shorthashflag = 0 means return all 40 character of hash
//***************************************************************************
string calHashofchunk(char *schunk, int length1, int shorthashflag)
{

    unsigned char hash[SHA_DIGEST_LENGTH];
    char buf[SHA_DIGEST_LENGTH * 2];
    SHA1((unsigned char *)schunk, length1, hash);

    //printf("\n*****hash ********");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf((char *)&(buf[i * 2]), "%02x", hash[i]);

    //cout<<"hash : "<<buf<<endl;
    string ans;
    if (shorthashflag == 1)
    {
        for (int i = 0; i < 20; i++)
        {
            ans += buf[i];
        }
    }
    else
    {
        for (int i = 0; i < SHA_DIGEST_LENGTH * 2; i++)
        {
            ans += buf[i];
        }
    }
    return ans;
}

//***************************************************************************
// It returns combined hash of all chunk for any file (20+20+20...)
//***************************************************************************
string getFileHash(char *fpath)
{
    string fileHash;
    ifstream file1(fpath, ifstream::binary);

    /* basic sanity check */
    if (!file1)
    {
        cout << "FILE DOES NOT EXITST : " << string(fpath) << endl;
        return "-1";
    }

    struct stat fstatus;
    stat(fpath, &fstatus);

    // Logic for deviding file1 into chunks
    long int total_size = fstatus.st_size;
    long int chunk_size = CSIZE;

    int total_chunks = total_size / chunk_size;
    int last_chunk_size = total_size % chunk_size;

    if (last_chunk_size != 0) // if file1 is not exactly divisible by chunks size
    {
        ++total_chunks; // add last chunk to count
    }
    else //when file1 is completely divisible by chunk size
    {
        last_chunk_size = chunk_size;
    }

    // loop to getting each chunk
    for (int chunk = 0; chunk < total_chunks; ++chunk)
    {
        int cur_cnk_size;
        if (chunk == total_chunks - 1)
            cur_cnk_size = last_chunk_size;
        else
            cur_cnk_size = chunk_size;

        char *chunk_data = new char[cur_cnk_size];
        file1.read(chunk_data,    /* address of buffer start */
                   cur_cnk_size); /* this many bytes is to be read */

        string sh1out = calHashofchunk(chunk_data, cur_cnk_size, 1);
        fileHash = fileHash + sh1out;
    }

    return fileHash;
}

//***************************************************************************
// This function create mtorrent file by adding all information into it
//***************************************************************************
string createTorrentFile(char *fpath, char *mtpath, string tcksocket1, string tcksocket2)
{
    writelog("Mtorrent creater called for filepath : " + string(fpath));
    struct stat sb;
    if (stat(fpath, &sb) == -1)
    {
        cout << "FILE NOT FOUND" << endl;
        return "-1";
    }

    ofstream myfile;
    myfile.open(string(mtpath));
    myfile << tcksocket1 << endl;
    myfile << tcksocket2 << endl;
    myfile << string(fpath) << endl;
    myfile << sb.st_size << endl;
    string flhash = getFileHash(fpath);
    myfile << flhash << endl;
    myfile.close();
    writelog("Mtorrent Succesfully created");
    return flhash;
}
