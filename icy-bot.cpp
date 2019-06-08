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

void buyValbzVales(Utils util, State state) {
    // int valbzFairvalue = state.fairvalues["VALBZ"];
    // int valbzBuyBookvalue = state.book_vals["VALBZ"].first;
    // int valbzSellBookvalue = state.book_vals["VALBZ"].second;
    // //int valeFairvalue = state.fairvalues["VALE"];
    // // int valeBuyBookvalue = state.book_vals["VALE"].first;
    // // int valeSellBookvalue = state.book_vals["VALE"].second;
    // if ((valbzSellBookvalue - valbzBuyBookvalue) > 10) {
    //   util.buy("VALE", min(valbzBuyBookvalue, valbzFairvalue) + 1, 2);
    //   util.buy("VALBZ", min(valbzBuyBookvalue, valbzFairvalue) + 1, 2);
    //   util.sell("VALE", min(valbzSellBookvalue, valbzFairvalue) - 1, 2);
    //   util.sell("VALBZ", min(valbzSellBookvalue, valbzFairvalue) - 1, 2);
    // }
}

void buyXLF(Utils util, State state) {

  int bond = 1001;
  int gs = state.fairvalues["GS"];
  int ms = state.fairvalues["MS"];
  int wfc = state.fairvalues["WFC"];
  int xlf = state.fairvalues["XLF"];

  cout << "fairvalues gs: " << gs <<  " ms: " << ms << " wfc: " << wfc << " xlf: " << xlf << endl;

  if ((state.fair_xlf() > xlf) && (gs > 0) && (ms > 0) && (wfc > 0)) {
      int profit = 3*bond + 2*gs + 3*ms + 2*wfc;

      if (xlf+105 > profit) {
          util.buy("XLF", xlf+1, 10);
          cout << "Arbitrage? Sum of stocks: " << state.fair_xlf() << " XLF: " << state.fairvalues["XLF"] << endl;
          util.convert_to_stocks("XLF", 10);
          util.sell("BOND", 1001, 3);
          util.sell("GS", gs-1, 2);
          util.sell("MS", ms-1, 3);
          util.sell("WFC", wfc-1, 2);

      }
  }

}

void pennyAllDaStocks(Utils *util){
	vector<string> stocks = {"GS", "MS", "WFC"};

	for (int i = 0; i < stocks.size(); i++) {
		unordered_map<string, pair<int, int>> bv = util->state.book_vals;
		if (bv.find(stocks[i]) != bv.end()) {
			double fv = util->state.fairvalues[stocks[i]];
			if (bv[stocks[i]].first + 3 < fv) {
				if (fv > 0) {
					util->buy(stocks[i], bv[stocks[i]].first + 1, 1);
					util->sell(stocks[i], bv[stocks[i]].second - 1, 1);
				}
			}
		}

	}
}

int main(int argc, char *argv[])
{
    // Be very careful with this boolean! It switches between test and prod
    string test_var(getenv("TEST"));
    bool test_mode = test_var == "true";

    if (!test_mode) {
        cout << "RUNNING IN PROD" << endl;
    }

    Configuration config(test_mode);
    Connection conn(config);
    State state;
    state.init();
    Utils util(config, conn, state);

    util.hello();

    thread read_from_server(parse_loop, &util);

	while (true) {
        if (state.our_book["BOND"].buys[999] < 50) {
            util.buy("BOND", 999, 5);
        }
        if (state.our_book["BOND"].buys[998] < 25) {
            util.buy("BOND", 998, 5);
        }
        if (state.our_book["BOND"].buys[997] < 15) {
            util.buy("BOND", 997, 5);
        }
        if (state.our_book["BOND"].buys[996] < 10) {
            util.buy("BOND", 996, 5);
        }

        if (state.our_book["BOND"].sells[1001] < 50) {
            util.sell("BOND", 1001, 5);
        }
        if (state.our_book["BOND"].sells[1002] < 25) {
            util.sell("BOND", 1002, 5);
        }
        if (state.our_book["BOND"].sells[1003] < 15) {
            util.sell("BOND", 1003, 5);
        }
        if (state.our_book["BOND"].sells[1004] < 10) {
            util.sell("BOND", 1004, 5);
        }

        //buyXLF(util, state);
		//pennyAllDaStocks(&util);
        usleep(1000 * 100);
	}

    return 0;
}
