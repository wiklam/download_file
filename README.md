# Client download implementation
## Overview
Program allows us to download a file from a server using UDP. The client is sending a request in the format "GET start size", where start means offset of the file which we want do download, size - the size of the package which will be sent. Server responds with header "DATA start size\n" and the rest are size bytes of the file starting from offset start.

## Policy
The client is using the sliding window protocol to get the file, the window size can be set in the network.c and the packet size too, but it is limited to 1000 by the server. Using select with a timeout to sleep waiting for server response.

## How to run
To run our program we have to start our server "./transport-server port_number"  and the client "./transport server_ip server_port_number save_to_file file_size" file size is limited by 1 000 000.

## Server
Probability to get response - 1/2
Probability to get duplicated message - 1/5
Delay of the response - 0.5-1/5s
Server hold just 1000 datagrams to send in queue.
