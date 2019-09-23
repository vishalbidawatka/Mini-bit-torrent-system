# Mini BitTorrent

### Prerequisites
1. G++ compiler
   * ```sudo apt-get install g++```
2. openssl for SHA hashing
   * ```sudo apt-get install openssl```
### How to compile project
1. go to client directory
   * ```make```
2. go to tracker directory
   * ```make```
3. To clean solution 
   * ```make clean```
   
### How to Run project
  
#### To run the Tracker

```
./tracker <my_tracker_ip>:<my_tracker_port> <other_tracker_ip>:<other_tracker_port>  <seederlist_file>  < tracker_log_file>

eg : ./tracker 127.0.0.1:5000 127.0.0.1:6000 seederlist serverlog1
```

#### To run the Client

```
./client <CLIENT_IP>:<UPLOAD_PORT> <TRACKER_IP_1>:<TRACKER_PORT_1> <TRACKER_IP_2>:<TRACKER_PORT_2> <client_log_file>
```
* creating client1 on new terminal with socket : 127.0.0.1:8800 <br/>
eg : ```./client 127.0.0.1:8800 127.0.0.1:5000 127.0.0.1:6000 clientlog1```

* creating client2 on another terminal with socket : 127.0.0.1:7600 <br/>
eg : ```./client 127.0.0.1:7600 127.0.0.1:5000 127.0.0.1:6000 clientlog2```

#### Command/Functionality on Clinent side 
 
 1. **To Share the file over network :** <br/>
 ```
  share <local_file_path> <filename>.<file_extension>.mtorrent
  Eg : share /home/darshan/movie.mp4 movie.mp4.mtorrent.
  ```
 * It will generate .mtorrent file and send data to tracker that I(clinet/seeder) have this file. Now other client/leecher can download this file using .mtorrent file.
  
 2. **To Download the file :** <br/>
 ```
  get <path_to_.mtorrent_file> <destination_path>
  Eg : get movie.mp4.mtorrent newmovie.mp4
  ```
 * It will send request to tracker to know about available seeders and then randomly choose one seeder and make connection with it to get chunk of data. 
  
 3. **Show Downloading status** <br/>
 ```
 show_downloads
 ```
 * It will list out files which are downloading(D), successfully downloaded(S). 
 
4. **To Stop sharing of file over network :** <br/>
 ```
 remove <filename.mtorrent>
 Eg : remove movie.mp4.mtorrent
 ```
 * It will stop sharing of that file over network by removing .mtorrent file and metadata of that file from tracker.

5. **close client :** <br/>
 ```
 close
 ```
 * It will shutdown client and  remove all metadata of files which have been shared by this client from tracker. 
  
 #### Assumptions

* seederlist file must be present on tracker folder for tracker to run. If not then create empty seederlist file.
* Use ```\``` as delimeter for filename.
* absolute & relative path, both are supported.
* whenever client starts, it scans current working directory for .mtorrent file and update/inform tracker about it.
* Client(seeder/leecher) and tracker implemented by multithreading thus also support multithreading environment.
 
   