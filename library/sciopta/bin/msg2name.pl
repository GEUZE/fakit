#!/usr/bin/perl

#
# use C::Scan module to do the job
# 
use C::Scan;
use File::Find::Rule;

#
# Check command-line
# 
if ( $#ARGV == -1 ){
     print STDERR "Usage: msg2name.pl [-t (C|ASM)] {<base-file.msg}\n";
     exit;
}
my @inc=();

my $type=0;          # plain
my $nostart=0;       # print start seq.
my $noend=0;         # print end seq.

while( $#ARGV >= 0 && $ARGV[0] =~ m/-/ ){
    if ( $ARGV[0] eq "-ns" ){
	shift @ARGV;
	$nostart=1;
    }
    if ( $ARGV[0] eq "-ne" ){
	shift @ARGV;
	$noend=1;
    }
    if ( $ARGV[0] eq "-t" ){
	shift @ARGV;
	die "Usage: msg2name.pl [-t (C|ASM)] {<base-file.msg}\n" if $#ARGV < 0;
	if ( $ARGV[0] eq "C" ){
	    $type = 1;
	} elsif ( $ARGV[0] eq "ASM" ){
	    $type = 2;
	}
	shift @ARGV;
    }
    if ( $ARGV[0] eq "-I" ){
	shift @ARGV;
	die "Missing path" if ( $#ARGV < 0 );
	push(@inc,$ARGV[0]);
	shift @ARGV;
    }	    
}
    
die "Usage: msg2name.pl [-t (C|ASM)] {<base-file.msg}\n" if ( $#ARGV == -1 );

if ( $type == 1 && $nostart == 0){
    printf "msgdb_t msgdb[] = {\n";
}
if ( $type == 2 && $nostart == 0){
    printf "msgdb:\n";
}
	    
my $cppflags=''; #'-DMSGID_16BIT';

#
# helper variables
#
my $result = '';
my $mark = '';
my $targetfile = '';
my %ids=();

#
# scan all msg found from current position
#
my @files = File::Find::Rule->file
                            ->name('*.msg')
			    ->in ('.');

foreach $targetfile (@files) {   
    #
    # first argument must be the target msg file
    # every additional argument will be used for resolving
    # 
    my $target = new C::Scan (filename => $targetfile,
			      add_cppflags => $cppflags,
			      includeDirs => [ @inc ]
			      );
    
    #
    # get all defines with in the first file and use all succeeded files for
    # dereferencing the values
    # 
    my $hash_ref = $target->get ('defines_no_args');
    while (my ($key, $value) = each (%$hash_ref)) {
	chomp $value; #remove NL
	$value =~ s/\r//;                     # remove \r
	if ($key !~ m/_BASE/) {
#	    printf STDERR "define $key $value\n";
	    $value =~ s/\((.*)\)/\1/g;
	    $result = '';
	    $mark = '';
	    druid_resolv ($targetfile, $value);	  
	    if ($mark =~ m/msg/) {
		if ( exists($ids{$result}) ) {
		    printf STDERR "Double msg id, omitting : $key, 0x%08lx\n",$result;
		    printf STDERR "Previous msg: %s,0x%08lx\n",$ids{$result},$result;
		} else {
		    $ids{$result} = $key;
		}
	    }
	}
    }  
}

foreach $result (sort keys %ids){
    $key = $ids{$result};
    if ( $type == 0 ){
	printf ("0x%08lx %s\n", $result, $key);
    } elsif ( $type == 1 ){
	printf ("{0x%08lx,\"%s\"},\n", $result, $key);
    } elsif ( $type == 2 ){
	printf (" .align 2\n.long 0x%08lx\n .byte \"%s\",0\n", $result, $key);
    }
}
if ( $type == 1 && $noend == 0){
    printf "};\n";
}
#
# resolve a define and display only if a MSG_BASE is part of the define
#
sub druid_resolv {
  my @line = split (/\ *\+\ */, $_[1]);
  foreach $elem (@line) {
    my $value;
    if (!($value = druid_check ($_[0], $elem))) {
      my $i = 0;
      while ($i < $#ARGV + 1 && !($value = druid_check (@ARGV[$i], $elem))) {
        $i += 1;
      }
    }
    chomp $value; # remove NL
    $value =~ s/\r//;                     # remove \r
    if ($value) {
      $value =~ s/\((.*)\)/\1/g;
      druid_resolv ($_[0], $value);
    }
    else {
      if ($elem =~ s/0x//g) {
	$elem = hex ($elem);
      }
      $result += $elem;
    }
    if ($elem =~ m/_BASE/) {
      $mark = 'msg';
    }
  }
}

#
# check if this is part of this define map
#
sub druid_check {
  my $target = new C::Scan (filename => $_[0],
			    add_cppflags => $cppflags,
			    includeDirs => [ @inc ]
			    );

  my $hash_ref = $target->get ('defines_no_args');
  my $tmp = $hash_ref->{$_[1]};
  if ($tmp) {
    return $tmp;
  }
  else {
    return;
  }
}

