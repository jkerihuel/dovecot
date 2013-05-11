#!/usr/bin/env perl
use strict;

my (%titlecase8, %uni8_decomp);
my (@titlecase16_keys, @titlecase16_values);
my (@titlecase32_keys, @titlecase32_values);
my (@uni16_decomp_keys, @uni16_decomp_values);
my (@uni32_decomp_keys, @uni32_decomp_values);
my (@multidecomp_keys, @multidecomp_offsets, @multidecomp_values);
while (<>) {
  chomp $_;
  my @arr = split(";");
  my $code = eval("0x".$arr[0]);
  my $decomp = $arr[5];
  my $titlecode = $arr[14];
  
  if ($titlecode ne "") {
    # titlecase mapping
    my $value = eval("0x$titlecode");
    if ($value == $code) { 
      # the same character, ignore
    } elsif ($code <= 0xff) {
      die "Error: We've assumed 8bit keys have max. 16bit values" if ($value > 0xffff);
      $titlecase8{$code} = $value;
    } elsif ($code <= 0xffff) {
      die "Error: We've assumed 16bit keys have max. 16bit values" if ($value > 0xffff);
      push @titlecase16_keys, $code;
      push @titlecase16_values, $value;
    } else {
      push @titlecase32_keys, $code;
      push @titlecase32_values, $value;
    }
  } elsif ($decomp =~ /(?:\<[^>]*> )?(.+)/) {
    # decompositions
    my $decomp_codes = $1;
    if ($decomp_codes =~ /^([0-9A-Z]*)$/i) {
      # unicharacter decomposition. use separate lists for this
      my $value = eval("0x$1");
      if ($value > 0xffffffff) {
	print STDERR "Error: We've assumed decomposition codes are max. 32bit\n";
	exit 1;
      }
      if ($code <= 0xff) {
        $uni8_decomp{$code} = $value;
      } elsif ($code <= 0xffff) {
	push @uni16_decomp_keys, $code;
	push @uni16_decomp_values, $value;
      } else {
	push @uni32_decomp_keys, $code;
	push @uni32_decomp_values, $value;
      }
    } else {
      # multicharacter decomposition.
      if ($code > 0xffffffff) {
	print STDERR "Error: We've assumed multi-decomposition key codes are max. 32bit\n";
	exit 1;
      }
      
      push @multidecomp_keys, $code;
      push @multidecomp_offsets, scalar(@multidecomp_values);

      foreach my $dcode (split(" ", $decomp_codes)) {
	my $value = eval("0x$dcode");
	if ($value > 0xffffffff) {
	  print STDERR "Error: We've assumed decomposition codes are max. 32bit\n";
	  exit 1;
	}
	push @multidecomp_values, $value;
      }
      push @multidecomp_values, 0;
    }
  }
}

sub print_list {
  my @list = @{$_[0]};
  
  my $last = $#list;
  my $n = 0;
  foreach my $key (@list) {
    printf("0x%05x", $key);
    last if ($n == $last);
    print ",";
    
    $n++;
    if (($n % 8) == 0) {
      print "\n\t";
    } else {
      print " ";
    }
  }
}

print "/* This file is automatically generated by unicodemap.pl from UnicodeData.txt

   NOTE: decompositions for characters having titlecase characters
   are not included, because we first translate everything to titlecase */\n";

sub print_map8 {
  my %map = %{$_[0]};
  my @list;
  for (my $i = 0; $i <= 0xff; $i++) {
    if (defined($map{$i})) {
      push @list, $map{$i};
    } else {
      push @list, $i;
    }
  }
  print_list(\@list);
}

print "static const uint16_t titlecase8_map[256] = {\n\t";
print_map8(\%titlecase8);
print "\n};\n";

print "static const uint16_t titlecase16_keys[] = {\n\t";
print_list(\@titlecase16_keys);
print "\n};\n";

print "static const uint16_t titlecase16_values[] = {\n\t";
print_list(\@titlecase16_values);
print "\n};\n";

print "static const uint32_t titlecase32_keys[] = {\n\t";
print_list(\@titlecase32_keys);
print "\n};\n";

print "static const uint32_t titlecase32_values[] = {\n\t";
print_list(\@titlecase32_values);
print "\n};\n";

print "static const uint16_t uni8_decomp_map[256] = {\n\t";
print_map8(\%uni8_decomp);
print "\n};\n";

print "static const uint16_t uni16_decomp_keys[] = {\n\t";
print_list(\@uni16_decomp_keys);
print "\n};\n";

print "static const uint32_t uni16_decomp_values[] = {\n\t";
print_list(\@uni16_decomp_values);
print "\n};\n";

print "static const uint32_t uni32_decomp_keys[] = {\n\t";
print_list(\@uni32_decomp_keys);
print "\n};\n";

print "static const uint32_t uni32_decomp_values[] = {\n\t";
print_list(\@uni32_decomp_values);
print "\n};\n";

print "static const uint32_t multidecomp_keys[] = {\n\t";
print_list(\@multidecomp_keys);
print "\n};\n";

print "static const uint16_t multidecomp_offsets[] = {\n\t";
print_list(\@multidecomp_offsets);
print "\n};\n";

print "static const uint32_t multidecomp_values[] = {\n\t";
print_list(\@multidecomp_values);
print "\n};\n";
