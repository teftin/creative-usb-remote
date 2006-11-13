//

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
      cout
        <<"+got uref [" <<nb <<"]:" <<endl
        <<"+ type[0x" <<uref.report_type <<"], id[0x" <<uref.report_id <<"]" <<endl
        <<"+ f_idx[0x" <<uref.field_index <<"]" <<endl
        <<"+ u_idx[0x" <<uref.usage_index <<"], u_code[0x" <<uref.usage_code <<"]" <<endl
        <<"+ value[0x" <<uref.value <<"]" <<endl <<endl;
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
