#include "libntptest.h"

extern "C" {
#include "ntp_calendar.h"
}

#include <string>
#include <sstream>

class caljulianTest : public libntptest {
protected:
	std::string CalendarToString(const calendar &cal) {
		std::ostringstream ss;
		ss << cal.year << "-" << (u_int)cal.month << "-" << (u_int)cal.monthday
		   << " (" << cal.yearday << ") " << (u_int)cal.hour << ":"
		   << (u_int)cal.minute << ":" << (u_int)cal.second;
		return ss.str();
	}
};

TEST_F(caljulianTest, RegularDate) {
	u_long date1 = 1;
	u_long date2 = 10;
	u_long date0 =  0;

	calendar cal1, cal2, cal0;

	caljulian(date1, &cal1);
	caljulian(date2, &cal2);
	caljulian(date0, &cal0);

	ADD_FAILURE() << CalendarToString(cal1).c_str();
	ADD_FAILURE() << CalendarToString(cal2).c_str();
	ADD_FAILURE() << CalendarToString(cal0).c_str();
}
