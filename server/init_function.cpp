#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector.h>

using namespace std;
int main (void){
    int serverId;
    sockaddr_in server_addr{
        .sin_family=AF_INET;
        .sin_port=htons(3333);
        .sin_addr=htonl(INADDR_ANY)};
    };
    servertId=socket(PF_INET, SOCK_STREAM,0);

    vector <string> messages; 
    if(bind(serverId,(sockaddr*)&server_addr,sizeof(server_addr))==-1){
        perror("Bind failed");
        return 1;
    }
    listen(serverId,1) ;
    int clientId;
    clientId = accept(serverId,nullptr,nullptr);
    string receive="";
    while(receive != "close"){
        messages.push_back(receive);
        read(clientId,receive);
    }

    shutdown(clientId,SHUT_RDWR)
    close(clientId);
    close(serverId);


    return 0;
}