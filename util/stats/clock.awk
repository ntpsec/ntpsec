# awk program to scan clockstat files and report errors/statistics
#
# usage: awk -f check.awk clockstats
#
# This program works for the following radios:
# Arbiter 1088 GPS receiver
# Spectracom GPS receiver
# IRIG audio decoder
# Austron 2200A/2201A GPS receiver
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
	#
	# select Austron ITF records
	# see summary for decode
	#
	else if (NF >= 13 && $5 == "ITF" && $12 >= 100) {
		itf_count++
		if ($9 > 200e-9 || $9 < -200e-9)
			itf_200++
		else if ($9 > 100e-9 || $9 < -100e-9)
			itf_100++
		itf_mean += $9
		itf_rms += $9 * $9
		itf_var += $10 * $10
		continue
	}
	#
	# select Austron ETF records
	# see summary for decode
	#
	else if (NF >= 13 && $5 == "ETF" && $13 >= 100) {
		etf_count++
		if ($6 > etf_max)
			etf_max = $6
		else if ($6 < etf_min)
			etf_min = $6
		etf_mean += $6
		etf_rms += $6 * $6
		etf_var += $9 * $9
		continue
	}
	#
	# select Austron TRSTAT records
	# see summary for decode
	#
	else if (NF >= 5 && $5 == "TRSTAT") {
		trstat_count++
		j = 0
		for (i = 6; i <= NF; i++)
			if ($i == "T")
				j++
		trstat_sat[j]++
		continue
	}
	#
	# select Austron ID;OPT;VER records
	#
	# config GPS 2201A TTY1 TC1 IN OUT1 B.00 B.00 28-Apr-93
	#
	# GPS 2201A	receiver model
	# TTY1		rs232 moduel
	# TC1		IRIG module
	# IN		input module
	# OUT1		output module
	# B.00 B.00	firmware revision
	# 28-Apr-9	firmware date3
        #
	else if (NF >= 5 && $5 == "ID;OPT;VER") {
		id_count++
		id_temp = ""
		for (i = 6; i <= NF; i++)
			id_temp = id_temp " " $i
		if (id_string != id_temp)
			printf "config%s\n", id_temp
		id_string = id_temp
		continue	
	}
	#
	# select Austron POS;PPS;PPSOFF records
	#
	# position +39:40:48.425 -075:45:02.392 +74.09 Stored UTC 0 200 0
	#
	# +39:40:48.425	position north latitude
	# -075:45:02.392 position east longitude
	# +74.09	elevation (meters)
	# Stored	position is stored
	# UTC		time is relative to UTC
	# 0 200 0	PPS offsets
	#
	else if (NF >= 5 && $5 == "POS;PPS;PPSOFF") {
		pos_count++
		pos_temp = ""
		for (i = 6; i <= NF; i++)
			pos_temp = pos_temp " " $i
		if (pos_string != pos_temp)
			printf "position%s\n", pos_temp
		pos_string = pos_temp
	continue
	}
	#
	# select Austron OSC;ET;TEMP records
	#
	# loop 1121 Software Control Locked
	#
	# 1121		oscillator type
	# Software Control loop is under software control
	# Locked	loop is locked
	#
	else if (NF >= 5 && $5 == "OSC;ET;TEMP") {
		osc_count++
		osc_temp = $6 " " $7 " " $8 " " $9
		if (osc_status != osc_temp)
			printf "loop %s\n", osc_temp
		osc_status = osc_temp
		if ($10 > osc_vmax)
			osc_vmax = $10
		if ($10 < osc_vmin)
			osc_vmin = $10
		if ($11 > osc_tmax)
			osc_tmax = $11
		if ($11 < osc_tmin)
			osc_tmin = $11
	continue
	}
	#
	# select Austron UTC records
	# these ain't ready yet
	#
	else if (NF >= 5 && $5 == "UTC") {
		utc_count++
		utc_temp = ""
		for (i = 6; i <= NF; i++)
			utc_temp = utc_temp " " $i
		if (utc_string != utc_temp)
#			printf "utc%s\n", utc_temp
                utc_string = utc_temp
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
#
# itf summary data
#
# itf		record count
# rms		itf rms error (ns)
# >200		itf error > 200 ns
# >100		itf error > 100 ns
# var		Allan variance
#
	if (itf_count > 1) { 
		itf_mean /= itf_count
		itf_rms = sqrt(itf_rms / itf_count - itf_mean * itf_mean) * 1e9
		itf_var = sqrt(itf_var / (2 * (itf_count - 1)))
		printf "itf %d, rms %.1f, >200 %d, >100 %d, var %.2e\n", itf_count, itf_rms, itf_200, itf_100, itf_var
	}
#
# etf summary data
#
# etf		record count
# mean		etf mean (ns)
# rms		etf rms error (ns)
# max		etf maximum (ns)
# min		etf minimum (ns)
# var		Allan variance
#
	if (etf_count > 0) {
                etf_mean /= etf_count
		etf_rms = sqrt(etf_rms / etf_count - etf_mean * etf_mean)
		etf_var = sqrt(etf_var / (2 * (etf_count - 1)))
		printf "etf %d, mean %.1f, rms %.1f, max %d, min %d, var %.2e\n", etf_count, etf_mean, etf_rms, etf_max, etf_min, etf_var
	}
#
# trstat summary data
#
# trstat	record count
# sat		histogram of tracked satellites (0 - 7)
#
	if (trstat_count > 0)
		printf "trstat %d, sat %d %d %d %d %d %d %d %d\n", trstat_count, trstat_sat[0], trstat_sat[1], trstat_sat[2], trstat_sat[2], trstat_sat[3], trstat_sat[4], trstat_sat[5], trstat_sat[6], trstat_sat[7]
#
# osc summary data
#
# osc		record count
# control	control midrange (V) +/- deviation (mV)
# temp		oven temperature midrange +/- deviation (deg C)
#
	if (osc_count > 0)
		printf "osc %d, control %.3f+/-%.3f, temp %.1f+/-%.2f\n", osc_count, (osc_vmax + osc_vmin) / 2, (osc_vmax - osc_vmin) / 2 * 1e3, (osc_tmax + osc_tmin) / 2, (osc_tmax - osc_tmin) / 2
}
