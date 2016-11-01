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
#define MODEMDEVICE0 "serial0" /* デバイスファイルの指定:COM1 */
#define MODEMDEVICE1 "serial1" /* デバイスファイルの指定:COM1 */

#define FALSE 0
#define TRUE 1
#define MC 4 /* 読み込む文字数 */
volatile int STOP=FALSE;

int main(void)
{
  int fd0, fd1, c, res, i;
  struct termios oldtio0, oldtio1, newtio;
  char buf[255];

  /* O_RDWR  :読み書き両用 
     O_NOCTTY:tty制御をしない */
  if((fd0=open(MODEMDEVICE0, O_RDWR | O_NOCTTY))== -1){
    perror(MODEMDEVICE0);
    exit(-1);
  }
  if((fd1=open(MODEMDEVICE1, O_RDWR | O_NOCTTY))== -1){
    perror(MODEMDEVICE1);
    exit(-1);
  }

  tcgetattr(fd0, &oldtio0);
  tcgetattr(fd1, &oldtio1);
  bzero(&newtio, sizeof(newtio));

  /* 
  CRTSCTS:フロー制御有り
  CS8    :8ビット、ノンパリティ、ストップビット１
  CLOCAL :モデムの状態信号を無視
  CREAD  :受信可能にする 
  */
  newtio.c_cflag= (BAUDRATE | CS8 | CLOCAL | CREAD);

  newtio.c_iflag=IGNPAR;/* IGNPAR:パリティエラーの文字は無視 */

  newtio.c_oflag=0;     /* rawモード */
  newtio.c_lflag=0;     /* 非カノニカル入力 */

  newtio.c_cc[VTIME]=0; /* キャラクタ間タイマは未使用 */
  newtio.c_cc[VMIN]=MC; /* MC文字受け取るまでブロックする */

  tcflush(fd0,TCIFLUSH);/* ポートのクリア */
  tcflush(fd1,TCIFLUSH);/* ポートのクリア */
  tcsetattr(fd0, TCSANOW, &newtio); /* ポートの設定を有効にする */
  tcsetattr(fd1, TCSANOW, &newtio); /* ポートの設定を有効にする */

  while(STOP==FALSE){
    res=read(fd0, buf, 1);
    write(fd1,buf,1);
    printf("0->1:%d\n", buf[0]);
    res=read(fd1, buf, 1);
    write(fd0,buf,1);
    printf("1->0:%d\n", buf[0]);
  }
  tcsetattr(fd0, TCSANOW, &oldtio0);  /* 退避させた設定に戻す */
  tcsetattr(fd1, TCSANOW, &oldtio1);  /* 退避させた設定に戻す */
  close(fd0);                       /* COM1のシリアルポートを閉じる */
  close(fd1);                       /* COM1のシリアルポートを閉じる */
  return(0);
}
