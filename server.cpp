#include "Utils/dh.hpp"
#include "Utils/rc4.hpp"
#include "Sockets/Server.hpp"
using namespace std;

mutex mtx;
queue<int> waitQueue;
map<string, int> usernames;
map<string, string> partner;

Server server(PORT);

// UTILITY FUNCTIONS
vector<string> parseString(string &s, char delimiter) {
    vector<string> res;
    string curr;
    for (auto x : s) {
        if (x == delimiter) {
            res.push_back(curr);
            curr = "";
        }
        else
            curr += x;
    }
    res.push_back(curr);
    return res;
}

void exitHandler(int sig) {
    cout << "Disconnecting Clients and Shutting Down Server\n";
    for (auto it : usernames) {
        server.sendMessage(it.second,"close");
        server.disconnect(it.second);
    }
    exit(0);
}

string sendPrimitives(){
    return to_string(G) + " " + to_string(P) + " ";
}

void sendStatus(string username) {
    vector<string> names;
    string statusMsg;
    statusMsg += GRN;
    statusMsg += "\t[server]:\n";
    for (auto it : usernames) {
        names.push_back(it.first);
    }
    for (int i = 0; i < names.size(); i++) {
        string status = partner.find(names[i]) == partner.end() ? string(GRN) + "FREE" + string(NRM) : string(RED) + "BUSY" + string(NRM);
        statusMsg += "\t" + string(BLU) + names[i] + string(NRM) + " " + status;
        if (i != names.size() - 1) {
            statusMsg += "\n";
        }
    }
    statusMsg += NRM;
    server.sendMessage(usernames[username],statusMsg);
    return;
}

bool connector(string username, vector<string> &parsedMsg) {
    string partnerName = parsedMsg[1];
    cout << "Session request from " + username + " to " + partnerName << endl;

    // MULTIPLE CHATS AT SAME TIME ISSUE
    if (partner.find(username) != partner.end()) {
        string errMessage = RED;
        errMessage += "\t[server]: You are already in a chat.";
        errMessage += NRM;
        server.sendMessage(usernames[username],errMessage);
        cout << username << "already present in a chat cannot connect to someone else"<<endl;
        return false;
    }

    // CHAT WITH ONESELF
    if (username == partnerName) {
        string errMessage = RED;
        errMessage += "\t[server]: You cannot connect with yourself";
        errMessage += NRM;
        server.sendMessage(usernames[username],errMessage);
        cout << username << " connection rejected as User cannot connect to self" << endl;
        return false;
    }

    // REQUESTED USERNAME DOES NOT EXIST
    if (usernames.find(partnerName) == usernames.end()) {
        string errMessage = RED;
        errMessage += "\t[server]: Username does not exist.";
        errMessage += NRM;
        server.sendMessage(usernames[username],errMessage);
        cout << username << " connection rejected as " << partnerName << " is not a valid username." << endl;
        return false;
    }

    // REQUESTED USER NAME IS BUSY
    if (partner.find(partnerName) != partner.end()) {
        string errMessage = RED;
        errMessage += "\t[server]: " + string(CYN) + partnerName + string(NRM) + " is BUSY.";
        errMessage += NRM;
        server.sendMessage(usernames[username],errMessage);
        cout << username << " connection rejected as " << partnerName << " is busy." << endl;
        return false;
    }

    // CONNECTION ESTABLISHMENT
    partner[username] = partnerName;
    partner[partnerName] = username;
    cout << "Connected: " << username << " and " << partnerName << endl;

    string msg = GRN;
    msg += "\t[server]: You are now connected to ";
    server.sendMessage(usernames[username],msg + partnerName + NRM);
    server.sendMessage(usernames[partnerName],msg + username + NRM);

    // HANDSHAKE
    server.sendMessage(usernames[username],"handshake");
    server.sendMessage(usernames[partnerName],"handshake");
    return true;
}

void closeSession(string username) {
    if(partner.find(username) == partner.end()) {
        return;
    }
    string partnerName = partner[username];
    string msg = GRN;
    msg += "\t[server]: " + username + " Closed the Chat Session" + NRM;
    server.sendMessage(usernames[partnerName],msg);
    if (partner.find(partnerName) != partner.end()) {
        partner.erase(partnerName);
    }
    if (partner.find(username) != partner.end()) {
        partner.erase(username);
    }
    msg = GRN;
    msg += "\t[server]: Session closed";
    msg += NRM;
    server.sendMessage(usernames[username],msg);
    sleep(0.1);
    server.sendMessage(usernames[username],"goodbye");
    server.sendMessage(usernames[partnerName],"goodbye");
    cout << "Disconnected " + username + " and " + partnerName << endl;
}

// ACTION CENTER
void *clientHandler(void *arg) {
    mtx.lock();
    int clientSocket = waitQueue.front();
    waitQueue.pop();
    mtx.unlock();

    server.sendMessage(clientSocket,sendPrimitives());

    // ACCEPT USERNAME
    string username = server.receiveMessage(clientSocket);
    while (usernames.count(username) > 0) {
        server.sendMessage(clientSocket,"REDO USERNAME.\n");
        username = server.receiveMessage(clientSocket);
    }
    usernames[username] = clientSocket;
    server.sendMessage(clientSocket,"Username accepted.\n");
    cout << GRN << username << " connected." << NRM << endl;

    string msg = "";
    while (true) {
        msg = server.receiveMessage(clientSocket);
        if (msg.size() == 0) {
            continue;
        }
        vector<string> parsedMsg = parseString(msg, ' ');
        if (parsedMsg[0] == "status") {
            sendStatus(username);
        }
        else if (parsedMsg[0] == "connect") {
            connector(username, parsedMsg);
        }
        else if (parsedMsg[0] == "key") {
            string A = parsedMsg[1];
            server.sendMessage(usernames[partner[username]],"keys");
            server.sendMessage(usernames[partner[username]], A);
        }
        else if (parsedMsg[0] == "close") {
            closeSession(username);
            break;
        }
        else if (parsedMsg[0] == "goodbye") {
            if(partner.find(username) == partner.end()) {
                string errMessage = RED;
                errMessage += "\t[server]: Not Connected to Any User";
                errMessage += NRM;
                server.sendMessage(usernames[username],errMessage);
                cout << RED << "Client Not Connected to Any User : Cannot Close Session" << NRM << endl;
            }
            else {
                cout << username << " said " << msg << endl;
                closeSession(username);
            }
        }
        else if (partner.find(username) != partner.end()) {
            string s = "\t[" + username + "]$" + msg;
            cout << s << endl;
            server.sendMessage(usernames[partner[username]],s);
        }
        else {
            string s = RED;
            s += "\t[server]: Invalid Command. Please refer to the readme for commands.";
            s += NRM;
            server.sendMessage(clientSocket,s);
        }
    }
    usernames.erase(username);
    server.disconnect(clientSocket);
    cout << username << " disconnected." << NRM << endl;
    return NULL;
}

int main() {
    srand(time(NULL));
    generatePrimitiveKeys();
    server.listenClients();
    signal(SIGINT, exitHandler);
    cout << GRN << "Server is up and running" << endl;
    cout << "G: " << to_string(G) << endl;
    cout << "P: " << to_string(P) << NRM << endl;
    while (true) {
        int clientSocket = server.acceptClients();
        waitQueue.push(clientSocket);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, clientHandler, NULL);
    }
    return 0;
}