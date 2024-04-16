#include "Utils/dh.hpp"
#include "Utils/rc4.hpp"
#include "Utils/prompt.hpp"
#include "Sockets/Client.hpp"
using namespace std;

string username;
ll privateKey;
ll secretKey;
bool secure;

// GLOBAL CLIENT OBJECT
Client client(PORT, IP_ADDRESS);

// UTILITY FUNCTIONS
string removeUsername(string msg) {
  string name;
  int i = 0;
  while (i < msg.length() && msg[i] != '$') {
    name.push_back(msg[i]);
    i++;
  }
  i++;
  if (i >= msg.length()) {
    // SERVER MESSAGE
    string temp = msg;
    encrypt(temp, to_string(secretKey));
    return temp;
  }
  string newMsg = msg.substr(i);
  if(newMsg != "goodbye" && newMsg != "close") {
    cout << MAG << name << NRM << ": ";
  }
  return newMsg;
}

void getPublicKeys(string s) {
  string strG = "", strP = "";
  bool genP = false;
  for(int i = 0; i < s.size(); i++) {
    if(s[i] == ' ') {
      genP = true;
    }
    if(genP) {
      strG += s[i];
    }
    else {
      strP += s[i];
    }
  }
  setPrimitiveKeys(stoll(strG), stoll(strP));
}

void exitHandler(int sig) {
  client.sendMessage("close");
  client.disconnect();
  exit(0);
}

// MESSAGE HANDLERS
void *receiver(void *arg) {
  while (true) {

    string message = client.receiveMessage();
    if (secure) {
      message = removeUsername(message);
      if(message != "goodbye" && message != "close" && message != "handshake" && message != "keys") {
        encrypt(message, to_string(secretKey));
      }
    }

    if (message == "close" ) {
      cout << "Server closed the connection." << endl;
      exit(0);
    } 
    
    else if (message == "goodbye") {
      secure = false;
      secretKey = 0;
      continue;
    } 
    
    else if (message == "handshake") {
      privateKey = getPrivateKey();
      client.sendMessage("key " + to_string(createPublicKey(privateKey)));
      continue;
    } 
    
    else if (message == "keys") {
      string B = client.receiveMessage();
      secretKey = createSecretKey(stoll(B), privateKey);
      secure = true;
      continue;
    }

    cout << message << endl;
  }
}

void *sender(void *arg) {
  while (true) {
    string message;
    getline(cin, message);
    string user = "[" + username + "]: ";
    if (message.size()) {
      cout << GOUP << left << CYN << user << NRM << message << endl;
    }
    if (message == "clear") {
      cout << CLR;
      continue;
    }
    if (secure && message != "status" && message != "goodbye" && message != "close") {
      encrypt(message, to_string(secretKey));
    }
    client.sendMessage(message);
    // sleep(0.1);
    if (message == "close") {
      exit(0);
    }
  }
}

int main() {
  srand(time(NULL));
  signal(SIGINT, exitHandler);
  
  // CONNECT TO SERVER
  client.connectServer();
  cout <<GRN<<"Connected to server."<<NRM<<endl;
  getPublicKeys(client.receiveMessage());

  // RECEIVE A VALID USERNAME: ONE THAT DOES NOT EXIST
  int redo = 0;
  do {
    if(redo == 1) {
      cout<<"Username Already Exists!"<<endl;
    }
    cout << "Enter username: ";
    cin >> username;
    client.sendMessage(username);
    if(redo == 0) {
      redo++;
    }
  } while(client.receiveMessage() == "REDO USERNAME.\n");
  
  // PRINT THE WELCOME MESSAGE AND SHOW INSTRUCTIONS
  cout<<GRN<<"Username Accepted"<<NRM<<endl;
  cout<<readme(username)<<endl;

  // USE SEPARATE THREADS TO RECEIVE AND SEND MESSAGES 
  pthread_t receive_t, send_t;
  pthread_create(&receive_t, NULL, receiver, NULL);
  pthread_create(&send_t, NULL, sender, NULL);
  pthread_join(receive_t, NULL);
  pthread_join(send_t, NULL);
  client.disconnect();
  return 0;
}