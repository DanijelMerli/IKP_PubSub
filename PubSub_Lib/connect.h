#pragma once

#define TYPE_PUBLISHER 0
#define TYPE_SUBSCRIBER 1

// connect to PubSub Engine
// type - type of connection (publisher/subscriber)
// returns 0 if successful 
// returns -1 if an error occured
int connect(int type);