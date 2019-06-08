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
#include <unistd.h>
#include "Util.h"
#include "Order.h"
#include <thread>

using namespace std;

bool running = true;

void parse_loop(Utils *util) {
    while (running) {
        util->read_and_parse();
    }
}

void buyValbzVales() {
    // int valbzFairvalue = fairvalues["VALBZ"];
    // int valbzBuyBookvalue = state.book_vals["VALBZ"].first;
    // int valbzSellBookvalue = state.book_vals["VALBZ"].first;
    // int valeFairvalue = fairvalues["VALE"];
    // int valeBuyBookvalue = state.book_vals["VALE"].first;
    // int valeSellBookvalue = state.book_vals["VALE"].first;
    //
    // util.buy("VALE", min(valbzBuyBookvalue, valbzFairvalue) + 1, 2);
    // util.buy("VALBZ", min(valbzBuyBookvalue, valbzFairvalue) + 1, 2);
    // util.sell("VALE", min(valeSellBookvalue, valbzFairvalue) - 1, 2);
    // util.sell("VALBZ", min(valbzSellBookvalue, valbzFairvalue) - 1, 2);
}

int main(int argc, char *argv[])
{
    // Be very careful with this boolean! It switches between test and prod
    bool test_mode = false;
    Configuration config(test_mode);
    Connection conn(config);
    State state;
    state.init();
    Utils util(config, conn, state);

    util.hello();

    thread read_from_server(parse_loop, &util);

	while (true) {
        // ETF arbitrage detection
        if (state.fair_xlf() > state.fairvalues["XLF"].second) {
            //util.buy("XLF", state.book_vals["XLF"], 10);
            cout << "Arbitrage? Sum of stocks: " << state.fair_xlf() << " XLF: " << state.fairvalues["XLF"].second << endl;
        }

        util.buy("BOND", 999, 5);
        util.sell("BOND", 1001, 5);
        usleep(1000 * 100);
	}

    return 0;
}
