#include "utils.h"

enum MSG_TYPE get_message_type(char* message) {
    enum MSG_TYPE type;
    sscanf(message, "%d", (int*) &type);
    return type;
}

char* get_message_data(char* message) {
    int tmp;
    char* content = (char*) calloc(MAX_MSG_LEN - 1, sizeof(char));
    sscanf(message, "%d:%[^:]", &tmp, content);
    return content;
}

message* read_message(int sock_fd) {
    message* msg = (message*) malloc(sizeof(message));
    char* msg_raw = (char*) calloc(MAX_MSG_LEN, sizeof(char));
    if(read(sock_fd, (void*) msg_raw, MAX_MSG_LEN) < 0) perror("read");
    msg->type = get_message_type(msg_raw);
    strcpy(msg->data, get_message_data(msg_raw));
    free(msg_raw);
    return msg;
}

message* read_message_nonblocking(int sock_fd) {
    message* msg = (message*) malloc(sizeof(message));
    char* msg_raw = (char*) calloc(MAX_MSG_LEN, sizeof(char));
    if(recv(sock_fd, (void*) msg_raw, MAX_MSG_LEN, MSG_DONTWAIT) < 0) return NULL;
    msg->type = get_message_type(msg_raw);
    strcpy(msg->data, get_message_data(msg_raw));
    free(msg_raw);
    return msg;
}

void send_message(int sock_fd, MSG_TYPE type, char* content) {
    char* msg_raw = (char*) calloc(MAX_MSG_LEN, sizeof(char));
    sprintf(msg_raw, "%d:%s", (int) type, content);
    if(write(sock_fd, (void*) msg_raw, MAX_MSG_LEN) < 0) perror("write");
    free(msg_raw);
}

player* create_player(int fd, char* name) {
    player* pl = (player*) malloc(sizeof(player));
    pl->fd = fd;
    pl->alive = 1;
    strcpy(pl->name, name);
    return pl;
}

game* create_new_game(int player1_idx, int player2_idx) {
    game* g = (game*) malloc(sizeof(game));
    g->player1_idx = player1_idx;
    g->player2_idx = player2_idx;
    for(int i = 0; i < 9; i++) g->board[i] = EMPTY;
    return g;
}

void make_move(game* g, int idx, FIELD mark) {
    g->board[idx] = mark;
}

char* parse_board(game* g) {
    char* board = (char*) calloc(13, sizeof(char));
    for(int i = 0; i < 9; i++) {
        board[i + i/3] = g->board[i] == X ? 'X' : (g->board[i] == O ? 'O' : i + '0');
    }
    board[3] = '\n';
    board[7] = '\n';
    board[11] = '\n';
    return board;
}

GAME_STATUS check_game_status(game* g) {
    int lines[8][3] = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };
    for(int i = 0; i < 8; i++) {
        int line_sum = g->board[lines[i][0]] + g->board[lines[i][1]] + g->board[lines[i][2]];
        if(line_sum == 0) return O_WIN;
        if(line_sum == 3) return X_WIN;
    }
    for(int i = 0; i < 9; i++) if(g->board[i] == EMPTY) return PLAYING;
    return DRAW;
}