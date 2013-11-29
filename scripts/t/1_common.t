use strict;
use warnings;
use Test::More tests => 30;
use Test::Command;
use List::MoreUtils qw(any);

my @script_list = qw(
    calc_tickadj/calc_tickadj ntp-wait/ntp-wait ntpsweep/ntpsweep
    ntptrace/ntptrace summary plot_summary
);

for my $script (@script_list) {
    fail("$script not a regular file") if (!-f $script);
    pass("$script is a regular file");
    fail("$script not an executabe") if (!-x $script);
    pass("$script is an executable");
    fail("$script is not readable") if (!-r $script);
    pass("$script is readable");

    TODO: {
        #todo_skip "$script - Broken script/no perl", 2
        #    if any { $script eq $_ } qw(freq_adj ntp-groper ntp-status plot_summary);

        require_ok("./$script");
        cmd_is("./$script --rubbish", '', qr/^Unknown option:/, 1, 
            "$script reports unkown option");
    }
}
