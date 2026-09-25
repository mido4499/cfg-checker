set -e
g++ -std=c++17 -Wall -Wextra -g -O2 -fsanitize=address,undefined -o app parser.cpp && ./app