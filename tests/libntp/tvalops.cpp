#include "libntptest.h"

extern "C" {
#include <math.h>
#include "timevalops.h"
}

#include <string>
#include <sstream>

class timevalTest : public libntptest {
protected:
	static const long MICROSECONDS;
	// that's it...
	struct lfpfracdata {
		long	usec;
		u_int32	frac;
	};
	static const lfpfracdata fdata[];
};

const long timevalTest::MICROSECONDS = 1000000;
const timevalTest::lfpfracdata timevalTest::fdata [] = {
	{      0, 0x00000000 }, {   7478, 0x01ea1405 },
	{  22077, 0x05a6d699 }, { 125000, 0x20000000 },
	{ 180326, 0x2e29d841 }, { 207979, 0x353e1c9b },
	{ 250000, 0x40000000 }, { 269509, 0x44fe8ab5 },
	{ 330441, 0x5497c808 }, { 333038, 0x5541fa76 },
	{ 375000, 0x60000000 }, { 394734, 0x650d4995 },
	{ 446327, 0x72427c7c }, { 500000, 0x80000000 },
	{ 517139, 0x846338b4 }, { 571953, 0x926b8306 },
	{ 587353, 0x965cc426 }, { 625000, 0xa0000000 },
	{ 692136, 0xb12fd32c }, { 750000, 0xc0000000 },
	{ 834068, 0xd5857aff }, { 848454, 0xd9344806 },
	{ 854222, 0xdaae4b02 }, { 861465, 0xdc88f862 },
	{ 875000, 0xe0000000 }, { 910661, 0xe921144d },
	{ 922162, 0xec12cf10 }, { 942190, 0xf1335d25 }
};


class TVAL {
public:
	struct timeval V;

	TVAL()
		{ ZERO(V); }
	TVAL(time_t hi, long lo)
		{ V.tv_sec = hi; V.tv_usec = lo; }
	bool operator == (const TVAL& rhs) const
		{ return timeval_cmp(&V, &rhs.V) == 0; }
	bool valid() const
		{ return timeval_isnormal(&V); }
	operator struct timeval* () 
		{ return &V; }
	operator struct timeval& ()
		{ return V; }
	TVAL& operator = (const TVAL& rhs)
		{ V = rhs.V; return *this; }
	TVAL& operator = (const struct timeval& rhs)
		{ V = rhs; return *this; }
};

std::ostream&
operator << (std::ostream& os, const TVAL& val)
{
	os << timeval_tostr(&val.V);
	return os;
}


class LFP {
public:
	l_fp V;

	LFP()
		{ ZERO(V); }
	LFP(u_int32 hi, u_int32 lo)
		{ V.l_ui = hi; V.l_uf = lo; }
	bool operator == (const LFP& rhs) const
		{ return L_ISEQU(&V, &rhs.V); }
	operator l_fp* () 
		{ return &V; }
	operator l_fp& ()
		{ return V; }
	LFP& operator = (const LFP& rhs)
		{ V = rhs.V; return *this; }
	LFP& operator = (const l_fp& rhs)
		{ V = rhs; return *this; }
};

static std::ostream&
operator << (std::ostream& os, const LFP& val)
{
	os << ulfptoa(&val.V, 10);
	return os;
}


// ---------------------------------------------------------------------
// test support stuff
// ---------------------------------------------------------------------

TEST_F(timevalTest, Normalise) {
	for (long ns = -2000000000; ns <= 2000000000; ns += 10000000) {
		TVAL	x(0, ns);

		timeval_norm(x);
		ASSERT_TRUE(x.valid());
	}
}

TEST_F(timevalTest, SignNoFrac) {
	// sign test, no fraction
	for (int i = -4; i <= 4; ++i) {
		TVAL	a(i, 0);
		int	E = (i > 0) - (i < 0);
		int	r = timeval_test(a);

		ASSERT_EQ(E, r);
	}
}

TEST_F(timevalTest, SignWithFrac) {
	// sign test, with fraction
	for (int i = -4; i <= 4; ++i) {
		TVAL	a(i, 10);
		int	E = (i >= 0) - (i < 0);
		int	r = timeval_test(a);

		ASSERT_EQ(E, r);
	}
}

// test compare
TEST_F(timevalTest, CmpFracEQ) {
	// fractions are equal
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL	a(i, 200);
			TVAL	b(j, 200);
			int	E = (i > j) - (i < j);
			int	r = timeval_cmp(a, b);

			ASSERT_EQ(E, r);
		}
}

TEST_F(timevalTest, CmpFracGT) {
	// fraction a bigger fraction b
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL	a( i , 999800);
			TVAL	b( j , 200);
			int	E = (i >= j) - (i < j);
			int	r = timeval_cmp(a, b);

			ASSERT_EQ(E, r);
		}
}

TEST_F(timevalTest, CmpFracLT) {
	// fraction a less fraction b
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL	a(i, 200);
			TVAL	b(j, 999800);
			int	E = (i > j) - (i <= j);
			int	r = timeval_cmp(a, b);

			ASSERT_EQ(E, r);
		}
}

// Test addition (sum)
TEST_F(timevalTest, AddFullNorm) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL a(i, 200);
			TVAL b(j, 400);
			TVAL E(i + j, 200 + 400);
			TVAL c;

			timeval_add(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timevalTest, AddFullOflow1) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL a(i, 200);
			TVAL b(j, 999900);
			TVAL E(i + j + 1, 100);
			TVAL c;
			timeval_add(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timevalTest, AddUsecNorm) {
	for (int i = -4; i <= 4; ++i) {
		TVAL a(i, 200);
		TVAL E(i, 600);
		TVAL c;

		timeval_addus(c, a, 400);
		ASSERT_EQ(E, c);
	}
}

TEST_F(timevalTest, AddUsecOflow1) {
	for (int i = -4; i <= 4; ++i) {
		TVAL a(i, 200);
		TVAL E(i + 1, 100);
		TVAL c;

		timeval_addus(c, a, MICROSECONDS - 100);
		ASSERT_EQ(E, c);
	}
}

// test subtraction (difference)
TEST_F(timevalTest, SubFullNorm) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL a(i, 600);
			TVAL b(j, 400);
			TVAL E(i - j, 600 - 400);
			TVAL c;

			timeval_sub(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timevalTest, SubFullOflow) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TVAL a(i, 100);
			TVAL b(j, 999900);
			TVAL E(i - j - 1, 200);
			TVAL c;

			timeval_sub(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timevalTest, SubUsecNorm) {
	for (int i = -4; i <= 4; ++i) {
		TVAL a(i, 600);
		TVAL E(i, 200);
		TVAL c;

		timeval_subus(c, a, 600 - 200);
		ASSERT_EQ(E, c);
	}
}

TEST_F(timevalTest, SubUsecOflow) {
	for (int i = -4; i <= 4; ++i) {
		TVAL a(i, 100);
		TVAL E(i - 1, 200);
		TVAL c;

		timeval_subus(c, a, MICROSECONDS - 100);
		ASSERT_EQ(E, c);
	}
}

// test negation
TEST_F(timevalTest, Neg) {
	for (int i = -4; i <= 4; ++i) {
		TVAL a(i, 100);
		TVAL b;
		TVAL c;
		timeval_neg(b, a);
		timeval_add(c, a, b);
		ASSERT_EQ(0, timeval_test(c));
	}
}

// test abs value
TEST_F(timevalTest, AbsNoFrac) {
	for (int i = -4; i <= 4; ++i) {
		TVAL	a(i, 0);
		TVAL	b;
		int	c;

		c = timeval_abs(b, a);
		ASSERT_EQ((i < 0), c);
		ASSERT_EQ((i != 0), timeval_test(b));
	}
}

TEST_F(timevalTest, AbsWithFrac) {
	for (int i = -4; i <= 4; ++i) {
		TVAL	a(i, 100);
		TVAL	b;
		int	c;

		c = timeval_abs(b, a);
		ASSERT_EQ((i < 0), c);
		ASSERT_EQ(1, timeval_test(b));
	}
}

// conversion to l_fp
TEST_F(timevalTest, ToLFPrelPos) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		TVAL	a(1, fdata[i].usec);
		LFP	E(1, fdata[i].frac);
		LFP	r;
		double	Ef;
		double	rf;

		timeval_reltolfp(r, a);
		LFPTOD(&E.V, Ef);
		LFPTOD(&r.V, rf);
		ASSERT_NEAR(Ef, rf, 1. / MICROSECONDS);
	}
}

TEST_F(timevalTest, ToLFPrelNeg) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		TVAL	a(-1, fdata[i].usec);
		LFP	E(~0, fdata[i].frac);
		LFP	r;
		double	Ef;
		double	rf;

		timeval_reltolfp(r, a);
		LFPTOD(&E.V, Ef);
		LFPTOD(&r.V, rf);
		ASSERT_NEAR(Ef, rf, 1. / MICROSECONDS);
	}
}

TEST_F(timevalTest, ToLFPabs) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		TVAL	a(1, fdata[i].usec);
		LFP	E(1 + JAN_1970, fdata[i].frac);
		LFP	r;
		double	Ef;
		double	rf;

		timeval_abstolfp(r, a);
		LFPTOD(&E.V, Ef);
		LFPTOD(&r.V, rf);
		ASSERT_NEAR(Ef, rf, 1. / MICROSECONDS);
	}
}

TEST_F(timevalTest, ToString) {
	static const struct {
		time_t		sec;
		long		usec;
		const char *	repr;
	} data [] = {
		{ 0, 0,	 "0.000000" },
		{ 2, 0,	 "2.000000" },
		{-2, 0, "-2.000000" },
		{ 0, 1,	 "0.000001" },
		{ 1,-1,	 "0.999999" },
		{-1, 1, "-0.999999" }
	};
	for (int i = 0; i < COUNTOF(data); ++i) {
		TVAL a(data[i].sec, data[i].usec);
		std::string E(data[i].repr);
		std::string r(timeval_tostr(a));

		ASSERT_EQ(E, r);
	}
}

// -*- EOF -*-
