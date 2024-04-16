#include "../headers.hpp"
using namespace std;
class Server {
  int sockfd;
  struct sockaddr_in saddr;

public:

  Server(uint16_t port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      err("Cannot Create Socket.");
    }
    memset(&saddr, 0, sizeof(sockaddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

// MAKE THE PORT AVAILABLE
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0) {
      perror("setsockopt(SO_REUSEADDR) failed");
    }
#ifdef SO_REUSEPORT
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuse, sizeof(reuse)) < 0) {
      perror("setsockopt(SO_REUSEPORT) failed");
    }
#endif

    if ( bind(sockfd, (sockaddr *)&saddr, sizeof saddr) == -1) {
      err("Cannot Bind The Socket.");
    }
  }

  void listenClients() {
    if (listen(sockfd, 10) == -1) {
      err("Cannot Listen.");
    }
    cout << "Waiting for Connections on port: " << PORT << endl;
  }

  int acceptClients() {
    int new_fd;
    socklen_t a = sizeof(saddr);
    if ((new_fd = accept(sockfd, (sockaddr *)&saddr, (socklen_t *)&a)) == -1) {
      err("Cannot Accept.");
    }
    return new_fd;
  }

  bool sendMessage(int fd, string s) {
    string sizeString = to_string(s.size());
    sizeString = string(MESSAGE_SIZE_LENGTH - sizeString.length(), '0') + sizeString;
    string paddedMessage = sizeString + s + string(MESSAGE_CONTENT_LENGTH - s.size(), 'X');
    char buf[MAX_SIZE];
    memcpy(buf, paddedMessage.c_str(), paddedMessage.size());
    int byte_total = 0, byte_now = 0, len = MAX_SIZE;
    while (byte_total < len) {
      if (((byte_now = send(fd, &buf[byte_total], len - byte_total, 0)) == -1)) {
        perror("Cannot Send Message");
        return false;
      }
      byte_total += byte_now;
    }
    return true;
  }

  string receiveMessage(int fd) {
    char buf[MAX_SIZE] = {0};
    int byte_total = 0, byte_now = 0, len = MAX_SIZE;
    while (byte_total < len) {
      if ( (byte_now = read(fd, &buf[byte_total], len - byte_total)) == -1 ) {
        perror("Cannot Receive Message");
        return "";
      }
      byte_total += byte_now;
    }
    string s(buf + MESSAGE_SIZE_LENGTH, MESSAGE_CONTENT_LENGTH);
    int sizeofMessage = stoi(string(buf, 20));
    s = s.substr(0, sizeofMessage);
    return s;
  }

  void disconnect(int fd) {
    close(fd);
  }

  void closeServer() {
    close(sockfd);
  }
};