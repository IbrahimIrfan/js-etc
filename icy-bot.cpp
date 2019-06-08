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

using namespace std;

unordered_map<string, int> positions;
unordered_map<string, int> maximums;
string resp_order[] = {
    "BOND",
    "GS",
    "MS",
    "USD",
    "VALBZ",
    "VALE",
    "WFC",
    "XLF"
};

void get_positions_from_exchange(string resp) {
    cout << "Getting positions" << endl;
    stringstream ss(resp);
    string trash;

    for (string s : resp_order) {
        positions[s] = 0;

        getline(ss, trash, ':');
        ss >> positions[s];
        cout << s << ": " << positions[s] << endl;
    }
}

void init() {
    maximums["BOND"] = 100;
    maximums["VALBZ"] = 10;
    maximums["VALE"] = 10;
    maximums["GS"] = 100;
    maximums["MS"] = 100;
    maximums["WFC"] = 100;
    maximums["XLF"] = 100;
}

int main(int argc, char *argv[])
{
    // Be very careful with this boolean! It switches between test and prod
    bool test_mode = true;
    Configuration config(test_mode);
    Connection conn(config);

    init();

    vector<string> data;
    data.push_back(string("HELLO"));
    data.push_back(config.team_name);

    vector<string> data_add;
    data_add.push_back(string("ADD"));
    data_add.push_back(string("5"));
    data_add.push_back(string("BOND"));
    data_add.push_back(string("BUY"));
    data_add.push_back(string("999"));
    data_add.push_back(string("5"));
    /*
      A common mistake people make is to conn.send_to_exchange() > 1
      time for every conn.read_from_exchange() response.
      Since many write messages generate marketdata, this will cause an
      exponential explosion in pending messages. Please, don't do that!
    */
    conn.send_to_exchange(join(" ", data));
    string resp = conn.read_from_exchange();
    get_positions_from_exchange(resp);

    conn.send_to_exchange(join(" ", data_add));
    string resp_add = conn.read_from_exchange();
    get_positions_from_exchange(resp_add);

    return 0;
}
