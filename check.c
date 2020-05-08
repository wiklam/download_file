//Wiktor Pilarczyk 308533
#include "header.h"


static void error_handle(const char *str, char *err){
    fprintf(stderr, str, err);
    exit(EXIT_FAILURE); 
}


int Socket(int domain, int type, int protocol){
    int rv = socket(domain, type, protocol);
    if (rv < 0)
        error_handle("Socket error %s\n", strerror(errno));
    return rv;
}


int min(int a, int b){
    if(a < b)
        return a;
    return b;
}


static int get_number(char *number){
    int a = atoi(number);
    if(a == 0 || a < 0){
        if(number[0] == '0' && strlen(number) == 1)
            return a;
        error_handle("Invalid port number\n", NULL);
    }
    return a;
}


void get_entrance(int argc, char **argv, char *ip, int *port, char *file, int *size){
    if(argc != 5)
        error_handle("Invalid number of arguments - ./transport [server ip] [port] [filename] [filesize]\n", NULL);
    
    ip = argv[1];
    struct sockaddr_in recipient;
    if(!inet_pton(AF_INET, ip, &recipient.sin_addr))
        error_handle("Invalid ip address\n", NULL);
    *port = get_number(argv[2]);
    file = argv[3];
    if(strcmp(file, "transport") == 0)   // do not want to overwrite the executable file
        error_handle("Filename can not be transport\n", NULL);
    *size = get_number(argv[4]);
}
