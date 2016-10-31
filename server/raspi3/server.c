/*
This document is copyrighted (c) 1997 Peter Baumann, (c) 2001 Gary Frerking and is distributed under the terms of the Linux Documentation Project (LDP) license, stated below.

Unless otherwise stated, Linux HOWTO documents are copyrighted by their respective authors. Linux HOWTO documents may be reproduced and distributed in whole or in part, in any medium physical or electronic, as long as this copyright notice is retained on all copies. Commercial redistribution is allowed and encouraged; however, the author would like to be notified of any such distributions.

All translations, derivative works, or aggregate works incorporating any Linux HOWTO documents must be covered under this copyright notice. That is, you may not produce a derivative work from a HOWTO and impose additional restrictions on its distribution. Exceptions to these rules may be granted under certain conditions; please contact the Linux HOWTO coordinator at the address given below.

In short, we wish to promote dissemination of this information through as many channels as possible. However, we do wish to retain copyright on the HOWTO documents, and would like to be notified of any plans to redistribute the HOWTOs.

If you have any questions, please contact <linux-howto@metalab.unc.edu>
see also: http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/intro.html#COPYRIGHT
*/
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BAUDRATE B9600
#define MODEMDEVICE0 "./serial0"
#define MODEMDEVICE1 "./serial1"
#define _POSIX_SOURCE 1 /* POSIX 準拠のソース */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 
int wait_flag=TRUE;                    /* シグナルを受け取っていなければ TRUE */

void main(void){
  int fd0, fd1, c, res;
  struct timeval Timeout;
  int loop = 1;
  int maxfd;
  fd_set readfs;
  struct termios oldtio,newtio;
  struct sigaction saio;           /* シグナルを受けた時の動作を定義 */
  char buf[255];
  /* デバイスを非ブロッキングモードでオープンする
     (read は即座に戻ってくるようになる) */
  fd0 = open(MODEMDEVICE0, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd0<0) {perror(MODEMDEVICE0); exit(-1); }
  fd1 = open(MODEMDEVICE1, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd1<0) {perror(MODEMDEVICE1); exit(-1); }
  maxfd = MAX(fd0,fd1)+1;

  /* デバイスを非同期モードにする前に，シグナルハンドラを設定する */
  saio.sa_handler = NULL;
  sigemptyset(&saio.sa_mask);
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO,&saio,NULL);
  
  /* プロセスが SIGIO を受け取れるようにする */
  fcntl(fd0, F_SETOWN, getpid());
  fcntl(fd1, F_SETOWN, getpid());
  /* ファイルデスクリプタを非同期モードにする (オンラインマニュアルに
     よれば，O_APPEND と O_NONBLOCK だけが F_SETFL で動作するとのこと… 
   */
  fcntl(fd0, F_SETFL, FASYNC);
  fcntl(fd1, F_SETFL, FASYNC);

  tcgetattr(fd0,&oldtio); /* save current port settings */
  tcgetattr(fd1,&oldtio); /* save current port settings */
  /* 新しいポートの設定をカノニカル入力処理に設定する */
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;
  newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
  newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
  newtio.c_cc[VERASE]   = 0;     /* del */
  newtio.c_cc[VKILL]    = 0;     /* @ */
  newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
  newtio.c_cc[VTIME]    = 0;
  newtio.c_cc[VMIN]     = 1;
  newtio.c_cc[VSWTC]    = 0;     /* '\0' */
  newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
  newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
  newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
  newtio.c_cc[VEOL]     = 0;     /* '\0' */
  newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
  newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
  newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
  newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
  newtio.c_cc[VEOL2]    = 0;     /* '\0' */
  tcflush(fd0, TCIFLUSH);
  tcflush(fd1, TCIFLUSH);
  tcsetattr(fd0,TCSANOW,&newtio);
  tcsetattr(fd1,TCSANOW,&newtio);
 
  /* 入力のためのループ */
  while (loop) {
    FD_SET(fd0, &readfs);  /* ソース 0 を調べることを指定 */
    FD_SET(fd1, &readfs);  /* ソース 1 を調べることを指定 */
    /* block until input becomes available */
    Timeout.tv_usec = 0;
    Timeout.tv_sec  = 1;
    res = select(maxfd, &readfs, NULL, NULL, &Timeout);
    if(res){
      if (FD_ISSET(fd0,&readfs)){         /* ソース 0 からの入力が可能 */
        read(fd0,buf,1);
        printf("0:%2d\n",buf[0]);
        fflush(stdout);
        write(fd1,buf,1);
      }
      if (FD_ISSET(fd1,&readfs)){         /* ソース 1 からの入力が可能 */
        read(fd1,buf,1);
        printf("1:%2d\n",buf[0]);
        fflush(stdout);
        write(fd0,buf,1);
      }
    }
  }
}

