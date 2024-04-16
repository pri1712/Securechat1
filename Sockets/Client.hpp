#include "../headers.hpp"
#include "../Utils/util.hpp"
using namespace std;

class Client {
  int sockfd;
  struct sockaddr_in saddr;

public:
  Client(uint16_t port, string addr) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      err("Unable to create socket.");
    }

    memset(&saddr, 0, sizeof(sockaddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    if ( inet_pton(AF_INET, addr.c_str(), &saddr.sin_addr) == -1) {
      err("Wrong address format.");
    }
  }

  int connectServer() {
    if (connect(sockfd, (sockaddr *)&saddr, sizeof saddr) == -1) {
      err("Unable to Connect.");
    }
    return sockfd;
  }

  bool sendMessage(string s) {
    string sizeString = to_string(s.size());
    sizeString = string(MESSAGE_SIZE_LENGTH - sizeString.length(), '0') + sizeString;
    string paddedMessage = sizeString + s + string(MESSAGE_CONTENT_LENGTH - s.size(), 'X');

    char buf[MAX_SIZE];
    memcpy(buf, paddedMessage.c_str(), paddedMessage.size());
    int byte_total = 0, byte_now = 0, len = MAX_SIZE;
    while (byte_total < len) {
      if ((byte_now = send(sockfd, &buf[byte_total], len - byte_total, 0)) == -1) {
        perror("Unable to Send Message");
        return false;
      }
      byte_total += byte_now;
    }
    return true;
  }
  
  string receiveMessage() {
    char buf[MAX_SIZE] = {0};
    int byte_total = 0, byte_now = 0, len = MAX_SIZE;
    while (byte_total < len) {
      if ((byte_now = read(sockfd, &buf[byte_total], len - byte_total)) == -1) {
        perror("Unable to Receive Message");
        return "";
      }
      byte_total += byte_now;
    }
    string s(buf + MESSAGE_SIZE_LENGTH, MESSAGE_CONTENT_LENGTH);
    int sizeofMessage = stoi(string(buf, 20));
    s = s.substr(0, sizeofMessage);
    return s;
  }

  void disconnect() {
    close(sockfd);
  }
};