#include "SearchEngineServer.h"
#include <iostream>

#include "client.h"

// void testServer()
// {
//     SearchEngineServer server("127.0.0.1", 1234, 4, 10);
//     server.start();
// }

// g++ main.cc -lwfrest -lworkflow -lssl -lcrypto -lcrypt -llog4cpp
int main(int argc, char* argv[])
{
    //testServer();
    client cloudDiskServer(1);
    cloudDiskServer.loadModules();
    cloudDiskServer.start(1234);
    return 0;
}