gcc -o grademe-macos grademe.c \
    -I/opt/homebrew/Cellar/mysql-client/9.4.0/include \
    -L/opt/homebrew/Cellar/mysql-client/9.4.0/lib \
    -lmysqlclient \
    $(pkg-config --cflags --libs glib-2.0) \
    -lsqlite3

./grademe-macos