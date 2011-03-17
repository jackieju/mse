#!perl
my $a; 
open FH, '<', 'cs.atg';

while ( <FH>){
$a = $a.$_;
}
# remove code between (. and .)
$a =~s/\(\.(.|\s)*?\.\)//g;
# remove code between /* and */
$a =~s/\/\*(.|\s)*?\*\///g;
print $a;
close FH;

