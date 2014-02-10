/*
 * Copyright (C) 2013-2014 Universite de Versailles 
 * Copyright (C) 2012 Exascale Research Center
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "yokosrv.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

/* Default settings */
static const char *DEFAULT_COM_PATHS = "/dev/ttyS0,/dev/ttyUSB0";
static const speed_t DEFAULT_COM_SPEED = B9600;

/**
 * Internal helper structures
 */
typedef enum {
   V_AUTO,
   V_300
} voltage_t;

typedef enum {
   C_AUTO,
   C_05,
   C_1,
   C_2
} current_t;

typedef enum {
   UR_01,
   UR_025,
   UR_05
} update_rate_t;

/**
 * Internal functions declaration.
 */
static inline void send_cmd(yokoctx_t *ctx, const char *cmd);
#ifndef NDEBUG
static void dbg_req(yokoctx_t *ctx, const char *rqst);
#endif
static bool send_req(yokoctx_t *ctx, const char *cmd, char *res,
                     unsigned int res_sz);

static double read_measurement(yokoctx_t *ctx, unsigned int field);
static double parse_float_str(const char *str);

static void set_voltage(yokoctx_t *ctx, voltage_t v);
static void set_current(yokoctx_t *ctx, current_t c);
static void set_update_rate(yokoctx_t *ctx, update_rate_t ur);


/**
 * Helper function wich pushes the current command to the yokogawa.
 *
 * @param ctx The context to use.
 * @param cmd The command to send to the powermeter.
 */
static inline void send_cmd(yokoctx_t *ctx, const char *cmd)
{
   unsigned int bufsize;
   //char *err;

   // copy the command in an internal buffer
   bufsize = strlen(cmd) * sizeof(*cmd);
   strncpy(ctx->cmd, cmd, 1024);

#ifndef NDEBUG
   printf("sending %s", cmd);
#endif

   if (write(ctx->fd, ctx->cmd, bufsize) < bufsize)
   {
      fprintf(stderr, "Failed to send command '%s'\n", ctx->cmd);
   }

   /*if (cmd[strlen(cmd) - 2] != '?' && (err = check_error(ctx)) != NULL)
   {
      fprintf(stderr, "ERROR: command = %s", cmd);
      fprintf(stderr, "ERROR: %s\n", err);
      free(err);
   }*/
}


/**
 * Sends a request and retunrs the answer in res.
 *
 * @param ctx The context to use
 * @param cmd The command to send
 * @param res Where to put the result
 * @param res_sz The size (in bytes) of the result.
 *
 * @return false if any error occured, true otherwise
 */
static bool send_req(yokoctx_t *ctx, const char *cmd, char *res,
                     unsigned int res_sz)
{
   int rret;
   unsigned int ind;

   send_cmd(ctx, cmd);
   ind = 0;
   while (ind < res_sz && ((rret = read(ctx->fd, &res[ind], 1)) == 1))
   {
      // end of response
      if (res[ind] == '\n')
      {
         res[ind] = '\0';
         break;
      }

      ind++;
   }

   if (rret == -1)
   {
      perror("Read failed");
      return false;
   }
   else if (rret == 0)
   {
      fprintf(stderr, "Connection lost.\n");
      return false;
   }

   return true;
}


#ifndef NDEBUG
/**
 * Sends a requests and print the answer.
 *
 * @param ctx The context to use
 * @param rqst The request to send.
 */
static void dbg_req(yokoctx_t *ctx, const char *rqst)
{
   char buf[1024];
   if (send_req(ctx, rqst, buf, 1024))
   {
      printf("%s\n", buf);
   }
}
#endif


/**
 * Sets the desired voltage limit.
 *
 * @param ctx The device descriptor.
 * @param v The voltage to set.
 */
static void set_voltage(yokoctx_t *ctx, voltage_t v)
{
   if (ctx == NULL)
   {
      return;
   }

   // crest factor set to 3
   send_cmd(ctx, "CONF:CFAC 3\n");

   // setup the voltage
   if (v == V_AUTO)
   {
      send_cmd(ctx, "CONF:VOLT:AUTO ON\n");
   }
   else if (v == V_300)
   {
      send_cmd(ctx, "CONF:VOLT:AUTO OFF\n");
      send_cmd(ctx, "CONF:VOLT:RANG 300V\n");
   }
}


/**
 * Sets the desired current limit.
 *
 * @param ctx The device descriptor.
 * @param c The current to set.
 */
static void set_current(yokoctx_t *ctx, current_t c)
{
   if (ctx == NULL)
   {
      return;
   }

   // crest factor set to 3
   send_cmd(ctx, "CONF:CFAC 3\n");

   if (c == C_AUTO)
   {
      send_cmd(ctx, "CONF:CURR:AUTO ON\n");
   }
   else
   {
      send_cmd(ctx, "CONF:CURR:AUTO OFF\n");

      if (c == C_05)
      {
         send_cmd(ctx, "CONF:CURR:RANG 0.5A\n");
      }
      else if (c == C_1)
      {
         send_cmd(ctx, "CONF:CURR:RANG 1A\n");
      }
      else if (c == C_2)
      {
         send_cmd(ctx, "CONF:CURR:RANG 2A\n");
      }
   }
}


/**
 * Sets the desired update rate.
 *
 * @param ctx The context to update
 * @param ur The update rate to use.
 */
static void set_update_rate(yokoctx_t *ctx, update_rate_t ur)
{
   if (ctx == NULL)
   {
      return;
   }

   if (ur == UR_01)
   {
      send_cmd(ctx, "SAMP:RATE 0.1S\n");
   }
   else if (ur == UR_025)
   {
      send_cmd(ctx, "SAMP:RATE 0.25S\n");
   }
   else if (ur == UR_05)
   {
      send_cmd(ctx, "SAMP:RATE 0.5S\n");
   }
}


/**
 * Connects to the yokogawa and initializes it.
 *
 * @return A context which can be used to communicate with the yokogawa
 */
yokoctx_t *yoko_connect()
{
   yokoctx_t *ctx;
   int fd;
   struct termios pset;
   char *paths, *path;

   // allocate result
   ctx = malloc(sizeof(*ctx));
   ctx->last_e = 0;

   // test all the possible paths
   paths = strdup(DEFAULT_COM_PATHS);
   path = strtok_r(paths, ",", &ctx->tokptr);

   // empty string
   if (path == NULL)
   {
      fprintf(stderr, "Cannot locate the serial port\n");
      free(ctx);
      free(paths);
      return NULL;
   }

   // evaluate all the paths
   do
   {
      fd = open(path, O_RDWR | O_NOCTTY);

      if (fd >= 0)
      {
         break;
      }

      path = strtok_r(NULL, ",", &ctx->tokptr);
   } while (path != NULL);
   free(paths);

   // no more paths and none worked
   if (fd < 0)
   {
      fprintf(stderr, "Cannot open any of the serial port in %s\n",
           DEFAULT_COM_PATHS);
      free(ctx);
      return NULL;
   }

   // initialize the context
   ctx->fd = fd;

   // fetch current settings
   if (tcgetattr(ctx->fd, &pset) < 0)
   {
      fprintf(stderr, "Failed to get current serial port configuration.\n");
      close(ctx->fd);
      free(ctx);
      return NULL;
   }

   // port speed is 9600 bauds
   if (cfsetspeed(&pset, DEFAULT_COM_SPEED) < 0)
   {
      fprintf(stderr, "Failed to setup the port speed\n");
      close(ctx->fd);
      free(ctx);
      return NULL;
   }

   pset.c_cflag |= CREAD;     // enable reading characters?
   pset.c_cflag |= CLOCAL;    // no modem control
   pset.c_cflag &= ~PARENB;   // no parity
   pset.c_cflag &= ~CSTOPB;   // one bit stop
   pset.c_cflag &= ~CSIZE;    // we define the data size...
   pset.c_cflag |= CS8;       // ... to be 8 bits

   pset.c_iflag |= IGNPAR;    // ignore errorneous data

   pset.c_lflag &= ~ICANON;   // non canonical mode: no special processing
   pset.c_lflag &= ~ECHO;     // do not echo input chars

   // clear all control characters
   pset.c_cc[VINTR] = 0;
   pset.c_cc[VQUIT] = 0;
   pset.c_cc[VERASE] = 0;
   pset.c_cc[VKILL] = 0;
   pset.c_cc[VEOF] = 0;
   pset.c_cc[VTIME] = 0;
   pset.c_cc[VMIN] = 1;    // block until receiving one char
   pset.c_cc[VSWTC] = 0;
   pset.c_cc[VSTART] = 0;
   pset.c_cc[VSTOP] = 0;
   pset.c_cc[VSUSP] = 0;
   pset.c_cc[VEOL] = 0;
   pset.c_cc[VREPRINT] = 0;
   pset.c_cc[VDISCARD] = 0;
   pset.c_cc[VWERASE] = 0;
   pset.c_cc[VLNEXT] = 0;
   pset.c_cc[VEOL2] = 0;

   // apply settings
   if (tcsetattr(fd, TCSANOW, &pset) < 0)
   {
      fprintf(stderr, "Failed to setup communication rules\n");
      close(ctx->fd);
      free(ctx);
      return NULL;
   }

   // flush buffers
   if (tcflush(fd, TCIOFLUSH) < 0)
   {
      fprintf(stderr, "Failed to flush serial port\n");
      close(ctx->fd);
      free(ctx);
      return NULL;
   }

   // reset the powermeter
   send_cmd(ctx, "*RST\n");
   send_cmd(ctx, "*CLS\n");
   send_cmd(ctx, "CONFIGURE:MODE RMS\n");
   send_cmd(ctx, "CONFIGURE:FILTER OFF\n");
   send_cmd(ctx, "CONFIGURE:LFILTER OFF\n");
   send_cmd(ctx, "CONFIGURE:SCALING OFF\n");
   send_cmd(ctx, "CONFIGURE:AVERAGING OFF\n");

   // setup default settings
   set_voltage(ctx, V_300);
   set_current(ctx, C_1);
   set_update_rate(ctx, UR_01);
   
   // setup the measurements we want to perform
   send_cmd(ctx, "MEAS:NORM:ITEM:PRES INTEG\n");
   
   // setup integration
   send_cmd(ctx, "SAMPLE:HOLD ON\n"); 
   send_cmd(ctx, "INTEGRATE:RESET\n");
   send_cmd(ctx, "INTEGRATE:MODE NORMAL\n");
   send_cmd(ctx, "INTEGRATE:TIMER 0,0,0\n");
   send_cmd(ctx, "INTEGRATE:START\n");
   send_cmd(ctx, "SAMPLE:HOLD OFF\n");

   // synchronization
   send_cmd(ctx, "STATUS:FILTER1 RISE\n");   // wait for an "update" event
   send_cmd(ctx, "COMM:WAIT 1\n");
   // wait again for the same event as at least two values are required for the
   // integration to happen
   send_cmd(ctx, "*CLS\n");
   send_cmd(ctx, "STATUS:FILTER1 RISE\n");
   send_cmd(ctx, "COMM:WAIT 1\n");

   // flush before starting
   if (tcdrain(ctx->fd) < 0)
   {
      fprintf(stderr, "Failed to drain serial port content.\n");
      close(ctx->fd);
      free(ctx);
      return NULL;
   }

   return ctx;
}


/**
 * Close the connection with the yokogawa and free used resources.
 *
 * @param ctx The context to close and free.
 */
void yoko_disconnect(yokoctx_t *ctx)
{
   if (ctx == NULL)
   {
      return;
   }

   close(ctx->fd);
   free(ctx);
}


/**
 * Reads the measurements performed on the power meter and returns the specified
 * field.
 *
 * @param ctx The context to use
 * @param field The position of the field to read, first is 0
 * @return The value of the specified field in the last measurement or -1 in
 * case of error.
 */
static double read_measurement(yokoctx_t *ctx, unsigned int field)
{
   char buf[1024];
   char *tokptr, *tok;
   unsigned int i;

   if (ctx == NULL)
   {
      return -1.;
   }

   if (!send_req(ctx, "MEAS:NORM:VAL?\n", buf, 1024))
   {
      return -1.;
   }

   tok = strtok_r(buf, ",", &tokptr);
   for (i = 0; i < field; i++)
   {
      tok = strtok_r(NULL, ",", &tokptr);
   }

   return parse_float_str(tok);
}

/**
 * Reads the current energy counter and energy consumed since last measurement,
 * in Joules.
 *
 * @param ctx The context in which performing the measurement.
 * @return The energy consumed since last measurement in J or 0 in case of
 *  error.
 */
double yoko_read_energy(yokoctx_t *ctx)
{  
   double res;
   double cur = read_measurement(ctx, 1);
   
   if (cur < 0 || cur == NAN || cur == INFINITY)
   {
      return 0.;
   }

   res = (cur * 3600.) - ctx->last_e;
   ctx->last_e = cur * 3600.;

   return res;
}

/**
 * Reads the power counter, in Watts.
 *
 * @param ctx The context in which performing the measurement.
 * @return The power currently consumed or 0 in case of error
 */
double yoko_read_power(yokoctx_t *ctx)
{  
   double res = read_measurement(ctx, 0);
   
   if (res < 0 || res == NAN || res == INFINITY)
   {
      return 0.;
   }

   return res;
}


/**
 * Returns the floating point number corresponding to the string given in
 * parameter. This string has to follow the syntax of floating point numbers
 * used by the Yokogawa WT210.
 *
 * @param str The string to convert.
 * @return The converted string as a number or -1 if the string is NULL.
 */
static double parse_float_str(const char *str)
{
   double coeff = 0.;
   int sign = 0;
   int esign = 0;
   int dpos = -1;
   int epos = -1;
   int npos = -1;
   int i;
   int slen;
   double res = 0;
   int eval = 0;
   int enpos = -1;

   if (str == NULL)
   {
      return -1.;
   }

   // special cases
   if (!strcmp(str, "9.9E+37"))
   {
      return NAN;
   } else if (!strcmp(str, "9.91E+37"))
   {
      return INFINITY;
   }

   slen = strlen(str);

   // search for the . and E
   for (i = 0; i < slen; i++)
   {
      if (str[i] == '.')
      {
         dpos = i;
         break;
      }
   }

   for (i = (epos > 0 ? epos + 1 : 0); i < slen; i++)
   {
      if (str[i] == 'e' || str[i] == 'E')
      {
         epos = i;
         break;
      }
   }

   // where does the number really start?
   if (str[0] == '-')
   {
      sign = -1;
      npos = 1;
   }
   else if (str[0] == '+')
   {
      sign = 1;
      npos = 1;
   }
   else
   {
      sign = 1;
      npos = 0;
   }

   // digits after the .
   if (dpos != -1)
   {
      coeff = 0.1;
      for (i = 1; i < epos - dpos; i++)
      {
         res += (str[dpos + i] - '0') * coeff;
         coeff *= 0.1;
      }
   }

   // digits before the .
   coeff = 1.;
   for (i = (dpos == -1 ? epos - 1: dpos - 1); i >= npos; i--)
   {
      res += (str[i] - '0') * coeff;
      coeff *= 10;
   }

   // where does the number start after E
   if (str[epos + 1] == '+')
   {
      esign = 1;
      enpos = epos + 2;
   }
   else if (str[epos + 1] == '-')
   {
      esign = -1;
      enpos = epos + 2;
   }
   else
   {
      esign = 1;
      enpos = epos + 1;
   }

   // digits after the E
   coeff = 1.;
   for (i = slen - 1; i >= enpos; i--)
   {
      eval += (str[i] - '0') * coeff;
      coeff *= 10;
   }

   return sign * res * pow(10, esign * eval);
}
