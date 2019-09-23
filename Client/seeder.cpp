/************************************************************************
**      RollNo:2018201033          Name : Darshan Kansagara			   **
************************************************************************/

#include "clientheader.h"
#include "socket.cpp"

#define CSIZE 512 * 1024

//***************************************************************************
// This function send chunks of data(=CHUNK SIZE) to leecher
//***************************************************************************
void *seederservice(void *socket_desc)
{
    int new_socket = *(int *)socket_desc;
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);
    writelog("Seeder gets Data from Clients : " + string(buffer));
    string actualfilepath = string(buffer);

    char *fpath = new char[actualfilepath.length() + 1];
    strcpy(fpath, actualfilepath.c_str());

    ifstream file1(fpath, ifstream::binary);

    if (!file1)
    {
        cout << "Can't Open file1  : " << string(fpath) << endl;
        //return "-1";
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

        send(new_socket, chunk_data, cur_cnk_size, 0);
    }

    //printf("Reply message sent from seeder\n");
    close(new_socket);
    file1.close();
    return socket_desc;
}

//*****************************************************************************
// It act as seeder when any client communicate with it to download the file
//*****************************************************************************
void *seederserverservice(void *socket_desc)
{
    string cli_socket = *(string *)socket_desc;
    socketclass cserversocket;
    pthread_t thread_id;
    cserversocket.setsocketdata(cli_socket);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed in seeder");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(cserversocket.ip);
    address.sin_port = htons(cserversocket.port);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed in seeder");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // accept connection from any leecher
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Error in accept connection in seeder");
            exit(EXIT_FAILURE);
        }
        writelog("******Connection accepted in seeder *******");

        //create new thread to provide data to that leecher and seeder again listen for other
        if (pthread_create(&thread_id, NULL, seederservice, (void *)&new_socket) < 0)
        {
            perror("\ncould not create thread in seeder\n");
        }
        if (new_socket < 0)
        {
            perror("accept failed in seeder");
        }
    }

    return socket_desc;
}