
rm ./LoggerSender -f
make clean all

valgrind    --tool=memcheck         \
            --leak-check=full       \
            --show-leak-kinds=all   \
            --show-reachable=yes    \
            --track-origins=yes     \
            --num-callers=20        \
            --track-fds=yes         \
            -v                      \
         ./LoggerSender
