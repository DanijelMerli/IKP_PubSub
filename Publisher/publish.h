#pragma once
#include "../Common/connect.h"

// send topic and msg to PubSub Engine
// returns number of bytes sent
// returns -1 if an error occured
int publish(char* topic, char* msg);


typedef struct PublisherData_st{
	char topic[MAX_TOPICLEN];
	char message[MAX_MSGLEN];
}PublisherData;
