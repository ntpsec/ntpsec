#!/vol/perl-5.8/bin/perl -w

$found = 0;
$last = 0;

while (<>) {
    next if /^#/;
    next if /^\s*$/;
    if (/^struct req_pkt/) {
	$found = 1;
    }
    if (/^struct info_dns_assoc/) {
	$last = 1;
    }
    if ($found) {
	if (/^(struct\s*\w*)\s*{$/) {
	    $type = $1;
	    printf "  printf(\"sizeof($type) = %%d\\n\", \n\t (int) sizeof($type));\n";
	    next;
	}
	if (/\s*\w+\s*(\w*)(\[.*\])?;$/) {
	    $field = $1;
	    printf "  printf(\"offsetof($field) = %%d\\n\", \n\t (int) offsetof($type, $field));\n";
	    next;
	}
	if (/^};$/) {
	    printf "  printf(\"\\n\");\n\n";
	    $found = 0 if $last;
	    next;
	}
    }
}
