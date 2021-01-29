#pragma once
#include "../Common/connect.h"

typedef struct PublisherData_st{
	char topic[MAX_TOPICLEN];
	char message[MAX_MSGLEN];
}PublisherData;
