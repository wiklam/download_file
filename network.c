//Wiktor Pilarczyk 308533
#include "header.h"
#include "check.h"
#include "header.h"
#include "file.h"
#include "check.h"

#define WIN_SIZE 10
#define MAX_PACKET 1000
#define DEBUG
#ifdef DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

static int window = 0;
static bool received[WIN_SIZE];
static char data[WIN_SIZE][10001];


static int prepare_message(char *message, int start, int size){
    return sprintf(message, "GET %d %d\n", start, size);
}


static void send_request(int sockfd, struct sockaddr_in server, int start, int size){
    char message[20];
    int len = prepare_message(message, start, size);
    sendto(sockfd, message,  len, MSG_DONTWAIT, 
                    (struct sockaddr*) &server, sizeof(server));
}


static void receive_messages(int sockfd, struct in_addr sin_addr, unsigned short port){
    char message[1100];
    struct sockaddr_in  sender;	
    socklen_t sender_len = sizeof(sender);
    int len;
    while((len = recvfrom(sockfd, message, 1099, MSG_DONTWAIT, 
                        (struct sockaddr*)&sender, &sender_len))){
        if(sin_addr.s_addr != sender.sin_addr.s_addr || port != sender.sin_port)    // correct sender
            continue;

        int start, size;
        sscanf(message, "DATA %d %d\n", &start, &size);
        if(start/MAX_PACKET < window)   // old packet
            continue;
        
        int index = (start/MAX_PACKET)%WIN_SIZE;    // already received;
        if(received[index])
            continue;
        received[index] = 1;
        
        int cnt = 0;
        while(message[cnt] != '\n')
            cnt++;
        cnt++;
        for(int g = 0; g < size; g++)
            data[index][g] = message[g+cnt];
    }
}


int main(int argc, char *argv[]){
    char *ip = NULL, *myfile = NULL;
    int port, size;
    get_entrance(argc, argv, &ip, &port, &myfile, &size);
    debug("ip: %s port: %d \nfilename: %s, size: %d\n", ip, port, myfile, size);
    
    int g = 0, packet_num = ((MAX_PACKET - 1) + size)/MAX_PACKET;
    
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_address.sin_addr);

    while(true){
        receive_messages(sockfd, server_address.sin_addr, server_address.sin_port);

        if(!received[window + g]){
            int start = (window + g)*(MAX_PACKET);
            int packet_size = min(MAX_PACKET, size - (window + g)*(MAX_PACKET));
            send_request(sockfd, server_address, start, packet_size);
        }
        while(received[window]){
            received[window] = 0;

        }

        g++;
        g %= WIN_SIZE;
        if(g + window == packet_num)
            g = 0;
    }
}