/* LinuxでRS232Cを用いた同期・非カノニカルデータ受信
MCで指定した文字数読み込んだら表示 zを読みこんだら終了 (serial01.c)
マイコン側も同名のファイル */

#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<termios.h>
#include<unistd.h>
#include<fcntl.h>

#define BAUDRATE B9600 /* 通信速度の設定 */
#define MODEMDEVICE "serial1" /* デバイスファイルの指定:COM1 */

#define FALSE 0
#define TRUE 1
#define MC 4 /* 読み込む文字数 */
volatile int STOP=FALSE;

int main(void)
{
  int fd, c, res, i;                   /* fd:ファイルディスクリプタ res:受け取った文字数 */
  struct termios oldtio, newtio; /* 通信ポートを制御するためのインターフェイス */
  char buf[255];                 /* 受信文字を格納 */

  if((fd=open(MODEMDEVICE, O_RDWR | O_NOCTTY))== -1){
    /* O_RDWR:読み書き両用 O_NOCTTY:tty制御をしない */

    perror(MODEMDEVICE);
    exit(-1);
  }

  tcgetattr(fd, &oldtio);          /* 現在のシリアルポートの設定を退避させる */
  bzero(&newtio, sizeof(newtio));  /* 新しいポートの設定の構造体をクリア */

  newtio.c_cflag= (BAUDRATE | CS8 | CLOCAL | CREAD);
   /* CRTSCTS:フロー制御有り CS8:8ビット、ノンパリティ、ストップビット１
      CLOCAL:モデムの状態信号を無視 CREAD:受信可能にする */

  newtio.c_iflag=IGNPAR;          /* IGNPAR:パリティエラーの文字は無視 */

  newtio.c_oflag=0;               /* rawモード */
  newtio.c_lflag=0;               /* 非カノニカル入力 */

  newtio.c_cc[VTIME]=0;           /* キャラクタ間タイマは未使用 */
  newtio.c_cc[VMIN]=MC;            /* MC文字受け取るまでブロックする */

  tcflush(fd,TCIFLUSH);           /* ポートのクリア */
  tcsetattr(fd, TCSANOW, &newtio); /* ポートの設定を有効にする */

  while(STOP==FALSE){
    res=read(fd, buf, 255);
    for(i=0; i<res; i++){
      if(buf[i]=='z'){
        STOP=TRUE;
        break;
      }
      printf("%c", buf[i]);
      if((i%4)==0){
        printf("\n");
      }
    }
  }
  tcsetattr(fd, TCSANOW, &oldtio);  /* 退避させた設定に戻す */
  close(fd);                       /* COM1のシリアルポートを閉じる */
  return(0);
}
