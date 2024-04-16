#ifndef HEADERS
#define HEADERS

// GENERAL
#include <bits/stdc++.h>
#include <cstring>

// THREADS
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <mutex>

// SOCKET PROGRAMMING
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// DATATYPE
#define ll long long

// TERMINAL IO
#define GOUP "\033[A\r"
#define CLR "\e[2J\e[H"

// COLORS
#define CYN "\x1B[36m"
#define MAG "\x1B[35m"
#define BLU "\x1B[34m"
#define GRN "\x1B[32m"
#define RED "\x1B[31m"
#define WHT "\x1B[37m"
#define NRM "\x1B[0m"

// ENVIRONMENT VARIABLES
#define PORT 8080
#define MAX_SIZE 1024
#define IP_ADDRESS "local"
#define MESSAGE_SIZE_LENGTH 20
#define MESSAGE_CONTENT_LENGTH 1004

#endif