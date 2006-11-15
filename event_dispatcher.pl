#!/usr/bin/perl

# $Id: event_dispatcher.pl,v 1.1 2006/11/15 00:38:37 ecto Exp $

use strict;
use warnings;

use Xmms::Remote;

$| = 1;


my $xmms_remote = new Xmms::Remote;

my %event_map = (
#	"power" => sub {},
#	"1" => sub {},
#	"2" => sub {},
#	"3" => sub {},
#	"4" => sub {},
#	"5" => sub {},
#	"6" => sub {},
#	"7" => sub {},
#	"8" => sub {},
#	"9" => sub {},
#	"0" => sub {},
#	"cmss" => sub {},
	"mute" => sub { $xmms_remote->set_main_volume(0) },
#	"rec" => sub {},
	"vol-" => sub { $xmms_remote->set_main_volume($xmms_remote->get_main_volume - 5) },
	"vol+" => sub { $xmms_remote->set_main_volume($xmms_remote->get_main_volume + 5) },
	"stop-eject" => sub { $xmms_remote->stop },
	"play-pause" => sub { $xmms_remote->play_pause },
#	"slow" => sub {},
	"prev" => sub { $xmms_remote->playlist_prev },
	"next" => sub { $xmms_remote->playlist_next },
#	"step" => sub {},
#	"eax" => sub {},
#	"options" => sub {},
#	"display" => sub {},
#	"return" => sub {},
#	"start" => sub {},
#	"cancel" => sub {},
#	"up" => sub {},
#	"left" => sub {},
#	"ok" => sub {},
#	"right" => sub {},
#	"down" => sub {},
);

open EVENT, './hid_read|';

while ( <EVENT> ) {
	print and next unless /^ \+ got key\(..\): (.*)$/;
	print "key[$1]: ";
	print "no event for this key\n" and next unless defined $event_map{$1};
	&{$event_map{$1}};
	print "event dispatched\n" 

}

close EVENT;
