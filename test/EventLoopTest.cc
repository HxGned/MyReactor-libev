#include "EventLoop.hh"

using namespace net;
using namespace std;

int main(int argc, char *argv[])
{
    EventLoop loop;
    // loop.callAsync();

    loop.queueInLoopThread([] () {cout << "hello world" << endl;});

    loop.loop();
    return 0;
}
