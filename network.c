//Wiktor Pilarczyk 308533
#include "header.h"
#include "check.h"

#define WIN_SIZE 1500
#define MAX_PACKET 1000
#define WAIT_TIME 100000 //in ms
//#define DEBUG
#ifdef DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

static int window = 0;
static int received[WIN_SIZE];
static char data[WIN_SIZE][MAX_PACKET+2];


static int prepare_message(char *message, int start, int size){
    return sprintf(message, "GET %d %d\n", start, size);
}


static void send_request(int sockfd, struct sockaddr_in server, int start, int size){
    char message[20];
    int len = prepare_message(message, start, size);
    Sendto(sockfd, message,  len, MSG_DONTWAIT, 
                    (struct sockaddr*) &server, sizeof(server));
}


static void receive_messages(int sockfd, struct in_addr sin_addr, in_port_t port){
    char message[MAX_PACKET + 20];
    struct sockaddr_in  sender;	
    socklen_t sender_len = sizeof(sender);
    long len;
    while((len = Recvfrom(sockfd, message, MAX_PACKET + 20, MSG_DONTWAIT, 
                        (struct sockaddr*)&sender, &sender_len))){
        if(len < 0)
            break;
        if(sin_addr.s_addr != sender.sin_addr.s_addr || port != sender.sin_port){    // correct sender
            debug("Dostałem smieci nie od serwera\n");
            continue;
        }

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
        memcpy(data[index], &message[cnt], size);
    }
}


static inline void try_move_window(int filefd, int size, int packet_num){
    while(received[window%WIN_SIZE]){
        received[window%WIN_SIZE] = 0;
        Write(filefd, data[window%WIN_SIZE], min(MAX_PACKET, size - window*MAX_PACKET));
        window++;
        if(window%10 == 0)
            printf("%.2f%% done\n",((float)window/packet_num)*100);
    }
}


static inline int set_connection(char *ip, int port, struct sockaddr_in *server_address){
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    
    bzero(server_address, sizeof(*server_address));
    (*server_address).sin_family = AF_INET;
    (*server_address).sin_port = htons(port);
    inet_pton(AF_INET, ip, &(*server_address).sin_addr);

    return sockfd;
}


static inline void make_send(int sockfd, struct sockaddr_in server_address, int size, int index){
    int start = (window + index)*(MAX_PACKET);
    int packet_size = min(MAX_PACKET, size - (window + index)*(MAX_PACKET));
    send_request(sockfd, server_address, start, packet_size);
}



int main(int argc, char *argv[]){
    char *ip = NULL, *myfile = NULL;
    int port, size;
    get_entrance(argc, argv, &ip, &port, &myfile, &size);
    debug("ip: %s port: %d \nfilename: %s, size: %d\n", ip, port, myfile, size);
    
    int packet_num = ((MAX_PACKET - 1) + size)/MAX_PACKET;
    
    int filefd = Open(myfile, O_CREAT|O_TRUNC|O_WRONLY, 0644);

    struct sockaddr_in server_address;
    int sockfd = set_connection(ip, port, &server_address);
    
    for(int g = 0; g < min(WIN_SIZE, packet_num); g++)
        make_send(sockfd, server_address, size, g);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = WAIT_TIME;

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    int ret;

    while((ret = select(sockfd + 1, &descriptors, NULL, NULL, &timeout)) >= 0){
        receive_messages(sockfd, server_address.sin_addr, server_address.sin_port);

        if(ret == 0){
            for(int g = 0; g < min(WIN_SIZE, packet_num - window); g++)
                if(!received[(window+g)%WIN_SIZE])
                    make_send(sockfd, server_address, size, g);
            timeout.tv_usec = WAIT_TIME;
        }

        try_move_window(filefd, size, packet_num);
        
        if(window >= packet_num)
            break;

        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
    }
    Close(filefd);
}