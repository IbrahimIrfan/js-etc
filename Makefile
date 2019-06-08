CXX = g++
CXX_FLAGS = -Wall -O3 -std=c++14

icy_bot: icy-bot.cpp
	$(CXX) $(CXX_FLAGS) -o icy-bot icy-bot.cpp
