// $Id: hid_read.cc,v 1.7 2007/01/25 21:25:54 ecto Exp $

//written by Stan Sawa teftin(at)gmail.com
//take it and share ...

#include <iostream>
#include <string>
#include <iomanip>
#include <map>
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


class rm1500_hiddev {
	int hid_s;

public:
	rm1500_hiddev( const string &path );
	~rm1500_hiddev();
	int get_key();
	void devinfo(ostream &ost);

};

rm1500_hiddev::rm1500_hiddev( const string &path ) {
	hid_s = open( path.c_str(), O_RDONLY );
	if ( hid_s < 0 )
		throw string("open(): ") + strerror(errno);

	int flags = HIDDEV_FLAG_UREF | HIDDEV_FLAG_REPORT;
	if ( ioctl(hid_s, HIDIOCSFLAG, &flags) )
		throw string("ioctl(HIDIOCSFLAG): ") + strerror(errno);

}

rm1500_hiddev::~rm1500_hiddev() {
	if ( hid_s >= 0 )
		close(hid_s);

}

int rm1500_hiddev::get_key() {
	ssize_t nb;
	struct hiddev_usage_ref uref;

	while ( (nb = read(hid_s, &uref, sizeof(uref))) >= 0 )
    	if ( uref.field_index == HID_FIELD_INDEX_NONE ) {
//			uref.report_type = HID_REPORT_TYPE_INPUT;
//			uref.report_id = 3;
			uref.field_index = 0;
			uref.usage_index = 3;
			ioctl(hid_s, HIDIOCGUCODE, &uref, sizeof(uref));
			ioctl(hid_s, HIDIOCGUSAGE, &uref, sizeof(uref));

			return uref.value;

//		}else { we are not interested in other stuff, as we get the proper button from actual report
		}

	if ( nb < 0 )
		throw string("read(): ") + strerror(errno);

	return -1;

}

void rm1500_hiddev::devinfo(ostream &ost) {
	hiddev_report_info h_rep;

	//get all reports
	h_rep.report_type = HID_REPORT_TYPE_INPUT;
	h_rep.report_id = HID_REPORT_ID_FIRST;
	while ( ioctl(hid_s, HIDIOCGREPORTINFO, &h_rep, sizeof(h_rep)) == 0 ) {
		ost <<"report id[0x" <<h_rep.report_id <<"]: field no: 0x" <<h_rep.num_fields <<endl;

		//get all fields in report
		hiddev_field_info h_field;

		h_field.report_type = HID_REPORT_TYPE_INPUT;
		h_field.report_id = h_rep.report_id;
		for ( __u32 i = 0; i < h_rep.num_fields; ++i ) {
			h_field.field_index = i;
			ioctl(hid_s, HIDIOCGFIELDINFO, &h_field, sizeof(h_field));
			ost
				<<" field[0x" <<i <<"]" <<endl
				<<"  usage: 0x" <<h_field.maxusage <<", flags: 0x" <<h_field.flags <<endl
				<<"  phys: 0x" <<h_field.physical <<", min: 0x" <<h_field.physical_minimum <<", max: 0x" <<h_field.physical_maximum <<endl
				<<"  logi: 0x" <<h_field.logical <<", min: 0x" <<h_field.logical_minimum <<", max: 0x" <<h_field.logical_maximum<<endl
				<<"  app: 0x" <<h_field.application <<", unit: 0x" <<h_field.unit <<", unit_exp: 0x" <<h_field.unit_exponent <<endl;

		}

		h_rep.report_id = HID_REPORT_ID_NEXT;
	}

}

class kcode2label {
	typedef map<int, string> k2l_t;
	k2l_t k2l;

public:
	kcode2label();
	inline const string &getlabel(int kc) const;

};

kcode2label::kcode2label() {
	struct {
		const char *label;
		int code;
	} *kcode_p, kcode_init[] = {
		{ "power", 0x86 },
		{ "1", 0xd1 },
		{ "2", 0xf1 },
		{ "3", 0x09 },
		{ "4", 0x51 },
		{ "5", 0x21 },
		{ "6", 0x1e },
		{ "7", 0x91 },
		{ "8", 0xc1 },
		{ "9", 0xee },
		{ "0", 0x01 },
		{ "cmss", 0x8e },
		{ "mute", 0x76 },
		{ "rec", 0xce },
		{ "vol-", 0xc6 },
		{ "vol+", 0x46 },
		{ "stop-eject", 0xa1 },
		{ "play-pause", 0x9e },
		{ "slow", 0xbe },
		{ "prev", 0xfe },
		{ "next", 0x5e },
		{ "step", 0x7e },
		{ "eax", 0x31 },
		{ "options", 0x41 },
		{ "display", 0x6e },
		{ "return", 0x71 },
		{ "start", 0x11 },
		{ "cancel", 0x3e },
		{ "up", 0xde },
		{ "left", 0xe1 },
		{ "ok", 0x81 },
		{ "right", 0xae },
		{ "down", 0xb1 },
		{ NULL, 0x00 }
 	};

	for ( kcode_p = kcode_init; kcode_p->label != NULL; ++kcode_p )
		k2l[kcode_p->code] = kcode_p->label;

}

const string &kcode2label::getlabel(int kc) const {
	k2l_t::const_iterator it = k2l.find(kc);
	if ( it == k2l.end() )
		throw string("cannot find keycode");
	return it->second;

}


int main() {
	cout <<"***irread test***" <<endl;

	try {
		cout <<"*start" <<endl;
		cout <<hex <<setfill('0');

		rm1500_hiddev ir_reader("/dev/usb/rm1500");
		ir_reader.devinfo(cout);

		kcode2label k2l;

		int keycode;
		while ( ( keycode = ir_reader.get_key() ) >= 0 ) {
			const string &l = k2l.getlabel(keycode);
			cout <<" + got key(" <<setw(2) <<keycode <<"): " <<l <<endl;  
		}

		cout <<"+finished normaly" <<endl;

	}catch ( const string &ex ) {
		cout <<"!got exception: " <<ex <<endl;

		return 1;

	}

	return 0;

}
