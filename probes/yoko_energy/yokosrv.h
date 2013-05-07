#pragma once


typedef struct {
   char cmd[1024];   // command to be send/sent
   char *tokptr;     // internal buffer for strtok
   int fd;           // serial port
   double last_e;    // least energy measured
} yokoctx_t;

yokoctx_t *yoko_connect();
void yoko_disconnect(yokoctx_t *ctx);
double yoko_read_energy(yokoctx_t *ctx);
double yoko_read_power(yokoctx_t *ctx);

