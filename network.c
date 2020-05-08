//Wiktor Pilarczyk 308533
#include "header.h"
#include "check.h"
#include "header.h"
#include "file.h"
#include "check.h"

#define WIN_SIZE 10
#define MAX_PACKET 1000

static int window = 0;
static bool received[WIN_SIZE];


static void get_message(){

}


static void prepare_message(){

}


static void send_request(){
    prepare_message();
}


static void receive_messages(){
    get_message();
}


int main(int argc, char *argv[]){
    char *ip = NULL, *myfile = NULL;
    int port, size;
    get_entrance(argc, argv, ip, &port, myfile, &size);
    int g = 0, packet_num = ((MAX_PACKET - 1) + size)/MAX_PACKET;
    myfile = NULL;
    while(true){
        if(received[window + g])
            continue;

        int start = (window + g)*(MAX_PACKET);
        int packet_size = min(MAX_PACKET, size - (window + g)*(MAX_PACKET));
        receive_messages(ip, port, start, packet_size);

        if(!received[window + g])
            send_request();

        if(save_to_file())
            break;

        g++;
        g %= WIN_SIZE;
        if(g + window == packet_num)
            g = 0;
    }
}