#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
    /* AF_UNIX/AF_LOCAL：用在本機程序與程序間的傳輸，讓兩個程序共享一個檔案系統(file system)
     * AF_INET , AF_INET6 ：讓兩台主機透過網路進行資料傳輸，AF_INET使用的是IPv4協定，而AF_INET6則是IPv6協定。
     *
     * SOCK_STREAM：提供一個序列化的連接導向位元流，可以做位元流傳輸。對應的protocol為TCP。
     * SOCK_DGRAM：提供的是一個一個的資料包(datagram)，對應的protocol為UDP
     *
     * 設定socket的協定標準，一般來說都會設為0，讓kernel選擇type對應的默認協議。
     * 成功產生socket時，會返回該socket的檔案描述符(socket file descriptor)，我們可以透過它來操作socket。若socket創建失敗則會回傳-1(INVALID_SOCKET)。
     */

    //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("Fail to create a socket.");
    }

    // socket的連線
    // 客戶端要連向伺服端，需要先知道並儲存伺服端的IP及port，netinet/in.h已經為我們定義好了一個struct sockaddr_in來儲存這些資訊
    struct sockaddr_in info;
    bzero(&info, sizeof(info)); // 初始化，將struct涵蓋的bits設為0
    info.sin_family = PF_INET;  //sockaddr_in為Ipv4結構

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address, inet_addr()便負責將字串型式的IP轉換為整數型式的IP。
    info.sin_port = htons(8700); // htons()就是Host TO Network Short integer的縮寫，它將本機端的字節序(endian)轉換成了網路端的字節序。

    /* int connect(int sd, struct sockaddr *server, int addr_len);
     * sd是socket的描述符
     *
     * 負責提供關於這個socket的所有信息
     * 它的意義簡單明瞭，就是server的大小
     */

    int err = connect(sockfd, (struct sockaddr*)&info, sizeof(info));

    if (err == -1) {
        printf("Connection error");
    }

    //Send a message to server
    char message[] = {"Hi there"};
    char receiveMessage[100] = {};
    send(sockfd, message, sizeof(message), 0);
    recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);

    printf("%s", receiveMessage);
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
