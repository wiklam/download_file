//Wiktor Pilarczyk 308533
#include "header.h"


static void error_handle(const char *str, char *err){
    fprintf(stderr, str, err);
    exit(EXIT_FAILURE); 
}


int Write(int fd, const char *buf, size_t count){
    int ret = write(fd, buf, count);
    if(ret < 0)
        error_handle("Write error %s\n", strerror(errno));
    return ret;
}


int Close(int filefd){
    int ret = close(filefd);
    if(ret < 0)
        error_handle("Close error %s\n", strerror(errno));
    return ret;
}


int Open(const char *path, int flags, int mode){
    int ret = open(path, flags, mode);
    if(ret < 0)
        error_handle("Open error %s\n", strerror(errno));
    return ret;
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
    if(a == 0){
        if(number[0] == '0' && strlen(number) == 1)
            return a;
        error_handle("Invalid port number\n", NULL);
    }
    return a;
}


void get_entrance(int argc, char **argv, char **ip, int *port, char **file, int *size){
    if(argc != 5)
        error_handle("Invalid number of arguments - ./transport [server ip] [port] [filename] [filesize]\n", NULL);
    
    *ip = argv[1];
    struct sockaddr_in recipient;
    if(!inet_pton(AF_INET, *ip, &recipient.sin_addr))
        error_handle("Invalid ip address\n", NULL);
    *port = get_number(argv[2]);
    if(*port <= 0 || *port > 65535)
        error_handle("Invalid port number\n", NULL);
    *file = argv[3];
    if(strcmp(*file, "transport") == 0)   // do not want to overwrite the executable file
        error_handle("Filename can not be transport\n", NULL);
    *size = get_number(argv[4]);
    if(*size <= 0 || *size > 10000000)
        error_handle("Invalid size number\n", NULL);
}


ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen){
    ssize_t rv = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
    if (rv < 0)
        error_handle("Sendto error %s\n", strerror(errno));
    return rv;
}


ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    ssize_t rv = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (rv < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        error_handle("Recvfrom error %s\n", strerror(errno));
    }
    return rv;
}