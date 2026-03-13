#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>  // for DNS lookup 
#include <sys/socket.h>
#include <netinet/in.h> //allows sockaddr and other thingamabobs

static char URL[100];
struct in_addr* URL_lookup(){  // stupid and gay declaration 
    printf("Enter the url of the site you want to download:");
    
    //char URL[100];
    fgets(URL, sizeof(URL), stdin);
    URL[strcspn(URL, "\n")] = 0;
    
    struct hostent *URL_INFO =  gethostbyname(URL); // netdb.h struct 2 store info
    struct in_addr *addr = (struct in_addr *)URL_INFO -> h_addr_list[0];
    //this is a predefined struct for the adress: "Network Byte Order"
    return addr;
}




int sock_handle(){
    
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct in_addr *addr = URL_lookup();
    
    struct sockaddr_in sock_addr;//need for most socket functions
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(80); //httons form int(80) into NBE 80 got http
    sock_addr.sin_addr = *addr; // got from dns lookup
    
    
    connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)); //!!!!!need to find out how to allow sockaddr_in not just sockaddr
    if (connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    char HTTP_request[780];
    sprintf(HTTP_request,   //http GET request to get homepage of site, html file  
            "GET / HTTP/1.1\r\n"
            "Host: %s\r\n,"
            "Connection: close\r\n"
            "\r\n"
          ,URL);
          
    send(sock, HTTP_request, strlen(HTTP_request), 0);
    
    //recv(sock, fptr, sizeof(fprt), MSG_WAITALL);
    //possibly offload buffer into another methood to store it in a file
    //if multithread need multiple files/loop all sock commands ig
    FILE *fptr = fopen("didly.txt", "wb");
    
    char buffer[1024];
    int bytes_recived;
    while((bytes_recived = recv(sock, buffer, sizeof(buffer), 0)) >0){
        fwrite(buffer, 1, bytes_recived, fptr);
    }

    

    
    fclose(fptr);
    close(sock);
    
}


int main(){
    
    sock_handle();
    return 0;
}