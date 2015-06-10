

open IN, "<", "zerocross.txt" or die $!;

my $counter = 0;
while(my $line = <IN>){
	if ($line =~ m/1/){
		print "$counter\n";
		$counter = 0;
	}else{
		$counter++;
		print "0\n";
	}
}

close IN;