#include "SearchEngineServer.h"
#include <iostream>

void testServer()
{
    SearchEngineServer server("127.0.0.1", 1234, 4, 10);
    server.start();
}

int main(int argc, char* argv[])
{
    testServer();
    return 0;
}

/*

g++ SearchEngineServer.cc main.cc ../net/*.cc HttpParser.cc -o Yewu.out && ./Yewu.out

*/