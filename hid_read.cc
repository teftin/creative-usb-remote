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
//        cout <<"+xmlrec initialized" <<endl;

//        ifstream hid("/dev/usb/hiddev0", ios_base::in | ios_base::binary );
//
//        if ( not hid.is_open() )
//            throw string("cannot open hiddev");

        hid_s = open( "/dev/usb/hiddev0", O_RDONLY );

        if ( hid_s < 0 )
            throw string("open(): ") + strerror(errno);

        cout <<hex <<setfill('0');

        int flags;
        if ( ioctl(hid_s, HIDIOCGFLAG, &flags) < 0 )
            throw string("ioctl(GET): ") + strerror(errno);
        cout <<"*flags: 0x" <<flags <<endl;
//        flags |= HIDDEV_FLAG_UREF;
//        if ( ioctl(hid_s, HIDIOCSFLAG, &flags) )
//            throw string("ioctl(SET): ") + strerror(errno);
//        cout <<"*flags: setting" <<endl;

        struct hiddev_event event;
        ssize_t nb;

        while ( (nb = read(hid_s, &event, sizeof(event))) >= 0 ) {
            cout <<"+got event [" <<nb <<"]: hid[0x" <<event.hid <<"], val[0x" <<event.value <<"]" <<endl; 

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
