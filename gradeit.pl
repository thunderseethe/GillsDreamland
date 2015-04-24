#!/usr/bin/perl
# 2014-12-05
# example grading script for proj4 - mycloud server
# 

use strict;
use warnings;

my $mcget = "./mcget";
my $mcput = "./mcput";
my $mcdel = "./mcdel";
my $mclst = "./mclist";
my $host  = "192.168.77.232";
my $port  = "1234";
my $skey  = "12345";

my $md5   = "/usr/bin/md5sum";


#
#            filename, name on server, delete flag
#
# delete flag = 1, we're going to delete as a check of "mcdel"
#

my @files	= ( 'mcput.c mcput.c 0',
			'mcget.c mcget.c 0',
			'mclist.c mclist.c 0',
			'mcdel.c mcdel.c 0',
			'mcput mcput 0',
			'mcget mcget 0',
			'mclist mclist 0',
			'Makefile Makefile 0',
			'mycloud_server.c mycloud_server.c 0',
			'/etc/hosts EtcHosts 0',
			'/etc/passwd EtcPasswd 0');


#
# MCPUT
#
print "Testing $mcput: \n";
my $thelist = "";
my $thedellist = "";
my $afile = "";
foreach $afile (@files) {
	my $tfile = "";
	my $nfile = "";
	my $delete = "";
	my $sys = "";
	my $rc = "";
	($tfile, $nfile, $delete) = split ' ', $afile;
	print "   Trying to PUT $tfile as $nfile: ";
	$sys = "/bin/cat ".$tfile." | ".$mcput." ".$host." ".$port." ".$skey;
	$sys.= " ".$nfile;
	print "\n      $sys\n";
	$rc = `$sys`;
	$thelist .= $nfile."\n";
	if (!$delete) {
		$thedellist .= $nfile."\n";
	}
}
my $list2 = "";
$list2=`/bin/echo "$thelist" | sort`;
print "Listing should look like:\n$list2";
my $thedellist2 = "";
$thedellist2=`/bin/echo "$thedellist" | sort`;


#
# MCLIST
#
print "\nTesting $mclst: \n";
my $listem = "";
$listem = $mclst." ".$host." ".$port." ".$skey;

my $rc1 = `$listem`;
my $list3 = "";
$list3=`/bin/echo "$rc1" | sort`;

print "Output from $mclst:\n$list3";

print "Comparing the two: ";
if ($list2 eq $list3) {
	print "Outputs are the same!\n";
} else {
	print "Outputs differ!\n";
}

#
# MCGET
#
print "\n\nTesting $mcget \n";

foreach $afile (@files) {
	my $tfile = "";
	my $nfile = "";
	my $delete = "";
	my $sys = "";
	my $rc = "";
	my $get = "";
	my $md5sum="";

	($tfile, $nfile, $delete) = split ' ', $afile;
	$md5sum=`cat $tfile | $md5`;
	print "   Trying to GET $nfile: ";

	$get = $mcget." ".$host." ".$port." ".$skey." ".$nfile;
	print "\n      $get\n";
	$rc = `$get | $md5`;
#	print "DEBUG: ($rc, $md5sum)\n";
	if ($md5sum eq $rc) {
		print "         retrieved, and equal!\n";
	} else {
		print "         not equal!\n";
	}
}


#
# MCDEL/MCLIST
#
print "\n\nTesting $mcdel: \n";

foreach $afile (@files) {
	my $tfile = "";
	my $nfile = "";
	my $delete = "";
	my $sys = "";
	my $rc = "";
	my $delone = "";

	($tfile, $nfile, $delete) = split ' ', $afile;

	if ($delete) {
		print "   Trying to DEL $nfile: ";
		$delone = $mcdel." ".$host." ".$port." ".$skey." ".$nfile;
		print "\n      $delone\n";
		$rc = `$delone`;
	}
}

my $rc3 = `$listem`;
my $list4 = "";
$list4=`/bin/echo "$rc3" | sort`;

print "Listing should look like:\n$thedellist2";

print "Output from $mclst:\n$list4";

print "Comparing the two: ";
if ($list4 eq $thedellist2) {
	print "Outputs are the same!\n";
} else {
	print "Outputs differ!\n";
}


exit;
