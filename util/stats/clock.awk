# awk program to scan clockstat files and report errors/statistics
#
# usage: awk -f check.awk clockstats
#
# This program works for the following radios:
# Arbiter 1088 GPS receiver
# Spectracom GPS receiver
# IRIG audio decoder
#
BEGIN {
	etf_min = osc_vmin = osc_tmin = 1e9
	etf_max = osc_vmax = osc_tmax = -1e9
}
#
# scan all records in file
#
{
	#
	# select Arbiter GPS records
	# 96 190 00:00:37.000 0 V=08 S=44 T=3 P=10.6 E=00
	# N39:42:00.951 W075:46:54.880 210.55      2.50 0.00
	#
	if (NF >= 4 && $3 == "127.127.11.1") {
		if (NF > 8) {
			arb_count++
			if ($7 != 0)
				arb_sync++
			x = substr($10, 3, 1)
			if (x == "0")
				arb_0++
			else if (x == "1")
				arb_1++
			else if (x == "2")
				arb_2++
			else if (x == "3")
				arb_3++
			else if (x == "4")
				arb_4++
			else if (x == "5")
				arb_5++
			else if (x == "6")
			arb_6++
		} else if (NF == 8) {
			arbn++
			arb_mean += $7
			arb_rms += $7 * $7
			if (arbn > 0) {
				x = $7 - arb_val
				arb_var += x * x
			}
			arb_val = $7
		}
		continue
	}
	#
	# select Spectracom records
	# see summary for decode
	#   96 189 23:59:32.248  D
	#
	if (NF >= 4 && $3 == "127.127.4.1") {
		if ($4 == "SIGNAL" || NF > 7)
			printf "%s\n", $0
		else {
			wwvb_count++
			if ($4 ~ /\?/)
				wwvb_x++
			else if ($4 ~ /A/)
				wwvb_a++
			else if ($4 ~ /B/)
				wwvb_b++
			else if ($4 ~ /C/)
				wwvb_c++
			else if ($4 ~ /D/)
				wwvb_d++
		}
		continue
	}
	#
	# select IRIG audio decoder records
	# see summary for decode
	#
	if (NF >= 4 && $3 == "127.127.6.0") {
		irig_count++
		if ($5 ~ /\?/)
			irig_error++
		continue
	}
} END {
#
# Arbiter 1088 summary data
#
# gps		record count
# err		error count
# sats(0-6)	satellites tracked
# mean		1 PPS mean (us)
# rms		1 PPS rms error (us)
# var		1 PPS Allan variance
#
	if (arb_count > 0) {
		printf "gps %d, err %d, sats(0-6) %d %d %d %d %d %d %d", arb_count, arb_sync, arb_0, arb_1, arb_2, arb_3, arb_4, arb_5, arb_6
		if (arbn > 1) {
			arb_mean /= arbn
			arb_rms = sqrt(arb_rms / arbn - arb_mean * arb_mean)
			arb_var = sqrt(arb_var / (2 * (arbn - 1)))
			printf ", mean %.2f, rms %.2f, var %.2e\n", arb_mean, arb_rms, arb_var * 1e-6
		} else {
			printf "\n"
		}
	}
#
# irig summary data
#
# irig		record count
# err		error count
#
	if (irig_count > 0)
		printf "irig %d, err %d\n", irig_count, irig_error
}
