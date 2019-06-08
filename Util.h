#ifndef UTIL_H
#define UTIL_H
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
#include <unordered_set>
#include <queue>
#include "Order.h"
#include "Constants.h"
using namespace std;

/** Join a vector of strings together, with a separator in-between
    each string. This is useful for space-separating things */
string join(string sep, vector<string> strs);


/* The Configuration class is used to tell the bot how to connect
   to the appropriate exchange. The `test_exchange_index` variable
   only changes the Configuration when `test_mode` is set to `true`.
*/
class Configuration {
private:
  /*
    0 = prod-like
    1 = slower
    2 = empty
  */
  static int const test_exchange_index = 0;
public:
  string team_name;
  string exchange_hostname;
  int exchange_port;
  /* replace REPLACEME with your team name! */
  Configuration(bool test_mode);
};

/* Connection establishes a read/write connection to the exchange,
   and facilitates communication with it */
class Connection {
private:
  FILE * in;
  FILE * out;
  int socket_fd;
public:
  Connection(Configuration configuration);

  /** Send a string to the server */
  void send_to_exchange(string input);

  /** Read a line from the server, dropping the newline at the end */
  string read_from_exchange();
};

class BookEntry {
public:
    int total_buy;
    unordered_map<int, int> buys;
    int total_sell;
    unordered_map<int, int> sells;

    BookEntry();

    unordered_map<int, int>& get_by_dir(string dir);
};


class State {
public:
    unordered_map<string, int> positions;
    unordered_map<string, int> maximums;
    unordered_map<int, Order> orders;
    unordered_map<string, pair<int, int>> book_vals;
    unordered_set<string> open;
    unordered_map<string, double> fairvalues; // <stock, <count, price>>
    unordered_map<string, BookEntry> our_book;
    unordered_map<string, queue<double>> trade_values;

    void get_positions_from_exchange(stringstream& resp);
    void init();
    void print_positions();
    double fair_xlf();
};

class Utils {
public:
    int order_id;
    Configuration& config;
    Connection& conn;
    State& state;
    Utils(Configuration& config, Connection& conn, State& state);

    void hello();
    void buy(string sym, int price, int qty);
    void sell(string sym, int price, int qty);
    void convert_to_stocks(string sym, int qty);
    void convert_to_obj(string sym, int qty);
    void cancel(int order_id);

    void parse_message(string message);
    void read_and_parse();
};

#endif
