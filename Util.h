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

class Utils {
public:
    int order_id;
    Connection& conn;
    Utils(Connection& conn);

    void buy(string sym, int price, int qty);
    void sell(string sym, int price, int qty);
};

#endif