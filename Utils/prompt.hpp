#include "../headers.hpp"
using namespace std;

string readme(string username) {
  string s = BLU;
  s += "[server]: Welcome to Safe Chat, " + username + " \n";
  s += "          Commands > \n";
  s += "          status:              Lists the status of all users.\n";
  s += "          connect [username]:  Connect To User [username].\n";
  s += "          goodbye:             End the current chatting session.\n";
  s += "          close:               Disconnect from the user from the server\n";
  s += "          clear:               Clears the chat from the window.\n";
  s += NRM;
  return s;
}