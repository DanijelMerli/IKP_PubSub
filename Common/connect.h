#pragma once
#include <stdio.h>
#include <WinSock2.h>

#define TYPE_PUBLISHER 0
#define TYPE_SUBSCRIBER 1

// connect to PubSub Engine
// type - type of connection (publisher/subscriber)
// returns true if successful 
// returns false if an error occured
bool connect(int type);

// Initialize windows sockets library
bool initializeWindowsSockets(); 
