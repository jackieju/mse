#!perl
my $a; 
open FH, '<', 'Ncpp.atg';

while ( <FH>){
$a = $a.$_;
}
$a =~s/\(\.(.|\s)*?\.\)//g;
$a =~s/\/\*(.|\s)*?\*\///g;
print $a;
close FH;

