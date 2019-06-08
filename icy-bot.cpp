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

    int valbzFairvalue = state.fairvalues["VALBZ"];
    int valeFairvalue = state.fairvalues["VALE"];

    if ((valbzFairvalue - valeFairvalue) > 15) {
      util.buy("VALE", valbzFairvalue + 1, 2);
      util.convert_to_stocks("VALBZ", 2);
      util.sell("VALE", valbzFairvalue - 1, 2);
    }
}

void buyXLF(Utils util, State state) {
    double gs = state.fairvalues["GS"];
    double ms = state.fairvalues["MS"];
    double wfc = state.fairvalues["WFC"];
    double xlf = state.fairvalues["XLF"];

    int margin = 15;
    int buy_factor = 3;

    if (gs <= 0 || ms <= 0 || wfc <= 0 || xlf <= 0) {
        return;
    }

    double sum_stocks = state.fair_xlf();
    if (sum_stocks > xlf + margin) {
        util.buy("XLF", xlf, 10 * buy_factor);
        util.convert_to_stocks("XLF", 10 * buy_factor);
        util.sell("GS", gs-1, 2 * buy_factor);
        util.sell("MS", ms-1, 3 * buy_factor);
        util.sell("WFC", wfc-1, 2 * buy_factor);
    } else if (xlf > sum_stocks + margin) {
        util.buy("GS", gs, 2 * buy_factor);
        util.buy("MS", ms, 3 * buy_factor);
        util.buy("WFC", wfc, 2 * buy_factor);
        util.convert_to_obj("XLF", 10 * buy_factor);
        util.sell("XLF", xlf-1, 10 * buy_factor);
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

void sellIfTooHigh(Utils *util, string symbol, int qnt) {
	util->sell(symbol, util->state.fairvalues[symbol], qnt);
}

void buyIfTooLow(Utils *util, string symbol, int qnt) {
	util->buy(symbol, util->state.fairvalues[symbol], qnt);
}


void checkLimits(Utils *util, State state) {

  BookEntry gs = state.our_book["GS"];
  BookEntry ms = state.our_book["MS"];
  BookEntry wfc = state.our_book["WFC"];

  if (gs.total_buy > 80) {
    sellIfTooHigh(util, "GS", 1);
  }
  else if (gs.total_sell < -80) {
    buyIfTooLow(util, "GS", 1);
  }


  if (ms.total_buy > 80) {
    sellIfTooHigh(util, "MS", 1);
  }
  else if (ms.total_sell < -80) {
    buyIfTooLow(util, "MS", 1);
  }

  if (wfc.total_buy > 80) {
    sellIfTooHigh(util, "WFC", 1);
  }
  else if (wfc.total_sell < -80) {
    buyIfTooLow(util, "WFC", 1);
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

        buyXLF(util, state);
		//pennyAllDaStocks(&util);
      //  checkLimits(&util, state);
        usleep(1000 * 100);
	}

    return 0;
}
