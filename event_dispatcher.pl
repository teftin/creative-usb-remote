#!/usr/bin/perl

# $Id: event_dispatcher.pl,v 1.2 2006/12/08 23:14:40 ecto Exp $

use strict;
use warnings;

use IO::Handle;
use POSIX;
use Xmms::Remote;

$| = 1;


$SIG{PIPE} = sub {
    our $mp_fifo;
    $mp_fifo->close;
};

my $mode = "nil";

my $xmms_remote = new Xmms::Remote;

sub mplayer_cmd($) {
    our $mp_fifo;
    unless ( defined $mp_fifo and $mp_fifo->opened ) {
        print "(cannot connect)" and return unless sysopen $mp_fifo, "/home/ecto/.mplayer/input.fifo", O_NONBLOCK|O_WRONLY;
    }
    syswrite $mp_fifo, $_[0]."\n"
}

my %ev_glob = (
    "1" => sub { print "MODE[xmms]"; $mode = "xmms" },
    "2" => sub { print "MODE[mplayer]"; $mode = "mplayer" },
);

my %ev_mode = (
    "xmms" => {
	    "mute" => sub { $xmms_remote->set_main_volume(0) },
	    "vol-" => sub { $xmms_remote->set_main_volume($xmms_remote->get_main_volume - 5) },
	    "vol+" => sub { $xmms_remote->set_main_volume($xmms_remote->get_main_volume + 5) },
	    "stop-eject" => sub { $xmms_remote->stop },
	    "play-pause" => sub { $xmms_remote->play_pause },
	    "prev" => sub { $xmms_remote->playlist_prev },
	    "next" => sub { $xmms_remote->playlist_next },
	    "options" => sub { $xmms_remote->toggle_shuffle },
    },
    "mplayer" => {
	    "play-pause" => sub { mplayer_cmd("pause") },
        "vol-" => sub { mplayer_cmd("volume -1") },
	    "vol+" => sub { mplayer_cmd("volume +1") },
	    "mute" => sub { mplayer_cmd("mute") },
	    "display" => sub { mplayer_cmd("vo_fullscreen") },
	    "ok" => sub { mplayer_cmd("osd") },
	    "left" => sub { mplayer_cmd("seek -5") },
	    "right" => sub { mplayer_cmd("seek 5") },
	    "down" => sub { mplayer_cmd("seek -30") },
	    "up" => sub { mplayer_cmd("seek 30") },
	    "power" => sub { mplayer_cmd("quit") },
    },
);

open EVENT, './hid_read|';

while ( <EVENT> ) {
	print and next unless /^ \+ got key\(..\): (.*)$/;
	print "key[$1]: ";

    if ( defined $ev_glob{$1} ) {
        &{$ev_glob{$1}};

    }else {

        if ( defined %{$ev_mode{$mode}} ) {
            print $mode;
            if ( defined $ev_mode{$mode}->{$1} ) {
                &{$ev_mode{$mode}->{$1}};

            }else {
                print " no event";

            }

        }else {
            print "nil";

        }

    }

	print ".\n" 

}

close EVENT;

__DATA__
all possible keycodes:

"power"
"1"
"2"
"3"
"4"
"5"
"6"
"7"
"8"
"9"
"0"
"cmss"
"mute"
"rec"
"vol-"
"vol+"
"stop-eject"
"play-pause"
"slow"
"prev"
"next"
"step"
"eax"
"options"
"display"
"return"
"start"
"cancel"
"up"
"left"
"ok"
"right"
"down"
