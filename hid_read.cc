// $Id: hid_read.cc,v 1.3 2006/11/13 23:52:01 ecto Exp $

#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/types.h>
#include <linux/hiddev.h>

int main() {
  cout <<"***irread test***" <<endl;

  int hid_s = -1;

  try {
    cout <<"*start" <<endl;
    hid_s = open( "/dev/usb/hiddev0", O_RDONLY );

    if ( hid_s < 0 )
      throw string("open(): ") + strerror(errno);

    cout <<hex <<setfill('0');

    int flags;
    flags = HIDDEV_FLAG_UREF | HIDDEV_FLAG_REPORT;
    if ( ioctl(hid_s, HIDIOCSFLAG, &flags) )
      throw string("ioctl(SET): ") + strerror(errno);
    if ( ioctl(hid_s, HIDIOCGFLAG, &flags) < 0 )
      throw string("ioctl(GET): ") + strerror(errno);
    cout <<"*flags: 0x" <<flags <<endl;
    
    ssize_t nb;

    struct hiddev_usage_ref uref;
    while ( (nb = read(hid_s, &uref, sizeof(uref))) >= 0 ) {

//      if ( uref.usage_index == 0 )
//        cout <<"+key event" <<endl;
//      else
//        cout <<"+ [0x" <<setw(2) <<uref.usage_index <<"]: " <<(uref.value?"up":"down") <<endl;

      cout
        <<"+got uref [" <<nb <<"]:" <<endl
        <<"+ type[0x" <<uref.report_type <<"], id[0x" <<uref.report_id <<"]" <<endl
        <<"+ f_idx[0x" <<uref.field_index <<"]" <<endl
        <<"+ u_idx[0x" <<uref.usage_index <<"], u_code[0x" <<uref.usage_code <<"]" <<endl
        <<"+ value[0x" <<uref.value <<"]" <<endl <<endl;

      if ( uref.usage_index ) {
        if ( (uref.usage_code - uref.usage_index) != 0xfeffffff )
          cout <<"PUK!" <<endl;

      }else {

        if ( uref.usage_index )
          cout <<"PUK!" <<endl;

      }
    }

    if ( nb < 0 )
      throw string("read(): ") + strerror(errno);

    close(hid_s);
    cout <<"+finished normaly" <<endl;

  }catch ( const string &ex ) {
    cout <<"!got exception: " <<ex <<endl;
    if ( hid_s >= 0 )
      close(hid_s);

    return 1;

  }

  return 0;

}


/*

power      0x86 0x79
1          0xd1 0x2e
2          0xf1 0x0e
3          0x09 0xf6
4          0x51 0xae
5          0x21 0xde
6          0x1e 0xe1
7          0x91 0x6e
8          0x3e
9          0xee 0x11
0          0x01 0xfe
cmss       0x8e 0x71
mute       0x76 0x89
rec        0xce 0x31
vol-       0x8e 0x71
vol+       0x31 0xce
stop-eject 0xa1 0x5e
play-pause 0x9e 0x61
slow       0xbe 0x41
prev       0xfe 0x01
next       0x5e 0xa1
step       0x7e 0x81
eax        0x31 0xce
options    0x41 0xbe
display    0x6e 0x91
return     0x71 0x8e
start      0x11 0xee
cancel     0x3e
up         0xde 0x21
left       0xe1 0xee
ok         0x81 0x7e
right      0xae 0x51
down       0xbe 0x4e

*/
