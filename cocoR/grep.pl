#!perl
my $a; 
open FH, '<', 'cs.atg';

while ( <FH>){
$a = $a.$_;
}
$a =~s/\(\.(.|\s)*?\.\)//g;
$a =~s/\/\*(.|\s)*?\*\///g;
print $a;
close FH;

