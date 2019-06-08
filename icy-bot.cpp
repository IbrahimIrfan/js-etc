/* HOW TO RUN
   1) Configure things in the Configuration class
   2) Compile: g++ -o bot.exe bot.cpp
   3) Run in loop: while true; do ./bot.exe; sleep 1; done
*/

// #include <bits/stdc++.h>

#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <unordered_map>
#include "Util.h"
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    // Be very careful with this boolean! It switches between test and prod
    bool test_mode = false;
    Configuration config(test_mode);
    Connection conn(config);
    State state;
    state.init_maximums();
    Utils util(config, conn, state);

    util.hello();
	while (true) {
		cout << conn.read_from_exchange() << endl;
        util.buy("BOND",999,10);
        util.sell("BOND",1001,10);
        usleep(1000 * 100);
	}

    return 0;
}
