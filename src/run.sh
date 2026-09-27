set -e

if [ "$#" -eq 0 ]; then
    echo "Usage: ./run.sh <file1.cpp> [file2.cpp ...]"
    exit 1
fi
g++ -std=c++17 -Wall -Wextra -g -O2 -fsanitize=address,undefined -o app "$@" && ./app