#include "libntptest.h"

extern "C" {
#include "timespecops.h"
}

#include <string>
#include <sstream>

class timespecTest : public libntptest {
protected:
	static const long NANOSECONDS;
	// that's it...
	struct lfpfracdata {
		long	nsec;
		u_int32 frac;
	};
	static const lfpfracdata fdata[];
};
const long timespecTest::NANOSECONDS = 1000000000;
const timespecTest::lfpfracdata timespecTest::fdata [] = {
		{	  0, 0x00000000 }, {   2218896, 0x00916ae6 },
		{  16408100, 0x0433523d }, { 125000000, 0x20000000 },
		{ 250000000, 0x40000000 }, { 287455871, 0x4996b53d },
		{ 375000000, 0x60000000 }, { 500000000, 0x80000000 },
		{ 518978897, 0x84dbcd0e }, { 563730222, 0x90509fb3 },
		{ 563788007, 0x9054692c }, { 583289882, 0x95527c57 },
		{ 607074509, 0x9b693c2a }, { 625000000, 0xa0000000 },
		{ 645184059, 0xa52ac851 }, { 676497788, 0xad2ef583 },
		{ 678910895, 0xadcd1abb }, { 679569625, 0xadf84663 },
		{ 690926741, 0xb0e0932d }, { 705656483, 0xb4a5e73d },
		{ 723553854, 0xb93ad34c }, { 750000000, 0xc0000000 },
		{ 763550253, 0xc3780785 }, { 775284917, 0xc6791284 },
		{ 826190764, 0xd3813ce8 }, { 875000000, 0xe0000000 },
		{ 956805507, 0xf4f134a9 }, { 982570733, 0xfb89c16c }
};

class TSPEC {
public:
	struct timespec V;

	TSPEC()
		{ ZERO(V); }
	TSPEC(time_t hi, long lo)
		{ V.tv_sec = hi; V.tv_nsec = lo; }
	bool operator == (const TSPEC& rhs) const
		{ return timespec_cmp(&V, &rhs.V) == 0; }
	bool valid() const
		{ return timespec_isnormal(&V); }
	operator struct timespec* () 
		{ return &V; }
	operator struct timespec& ()
		{ return V;	}
	TSPEC& operator = (const TSPEC& rhs)
		{ V = rhs.V; return *this; }
	TSPEC& operator = (const struct timespec& rhs)
		{ V = rhs; return *this; }
};

static std::ostream&
operator << (std::ostream& os, const TSPEC& val)
{
	os << timespec_tostr(&val.V);
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

std::ostream&
operator << (std::ostream& os, const LFP &val)
{
	os << ulfptoa(&val.V, 10);
	return os;
}


// ---------------------------------------------------------------------
// test support stuff
// ---------------------------------------------------------------------

TEST_F(timespecTest, Normalise) {
	for (long ns = -2000000000; ns <= 2000000000; ns += 10000000) {
		TSPEC x(0, ns);
		timespec_norm(x);
		ASSERT_TRUE(x.valid());
	}
}

TEST_F(timespecTest, SignNoFrac) {
	// sign test, no fraction
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i, 0);
		int E = (i > 0) - (i < 0);
		int r = timespec_test(a);
		ASSERT_EQ(E, r);
	}
}

TEST_F(timespecTest, SignWithFrac) {
	// sign test, with fraction
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i, 10);
		int E = (i >= 0) - (i < 0);
		int r = timespec_test(a);

		ASSERT_EQ(E, r);
	}
}

// test compare
TEST_F(timespecTest, CmpFracEQ) {
	// fractions are equal
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a( i , 200);
			TSPEC b( j , 200);
			int   E = (i > j) - (i < j);
			int   r = timespec_cmp(a, b);

			ASSERT_EQ(E, r);
		}
}

TEST_F(timespecTest, CmpFracGT) {
	// fraction a bigger fraction b
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a(i, 999999800);
			TSPEC b(j, 200);
			int   E = (i >= j) - (i < j);
			int   r = timespec_cmp(a, b);

			ASSERT_EQ(E, r);
		}
}

TEST_F(timespecTest, CmpFracLT) {
	// fraction a less fraction b
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a(i, 200);
			TSPEC b(j, 999999800);
			int   E = (i > j) - (i <= j);
			int   r = timespec_cmp(a, b);

			ASSERT_EQ(E, r);
		}
}

// Test addition (sum)
TEST_F(timespecTest, AddFullNorm) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a(i, 200);
			TSPEC b(j, 400);
			TSPEC E(i + j, 200 + 400);
			TSPEC c;

			timespec_add(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timespecTest, AddFullOflow1) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a(i, 200);
			TSPEC b(j, 999999900);
			TSPEC E(i + j + 1, 100);
			TSPEC c;

			timespec_add(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timespecTest, AddNsecNorm) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i, 200);
		TSPEC E(i, 600);
		TSPEC c;

		timespec_addns(c, a, 400);
		ASSERT_EQ(E, c);
	}
}

TEST_F(timespecTest, AddNsecOflow1) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i, 200);
		TSPEC E(i + 1, 100);
		TSPEC c;

		timespec_addns(c, a, NANOSECONDS - 100);
		ASSERT_EQ(E, c);
	}
}

// test subtraction (difference)
TEST_F(timespecTest, SubFullNorm) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a( i , 600);
			TSPEC b( j , 400);
			TSPEC E(i-j, 200);
			TSPEC c;

			timespec_sub(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timespecTest, SubFullOflow) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			TSPEC a(  i  , 100);
			TSPEC b(  j  , 999999900);
			TSPEC E(i-j-1, 200);
			TSPEC c;

			timespec_sub(c, a, b);
			ASSERT_EQ(E, c);
		}
}

TEST_F(timespecTest, SubNsecNorm) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i, 600);
		TSPEC E(i, 200);
		TSPEC c;

		timespec_subns(c, a, 400);
		ASSERT_EQ(E, c);
	}
}

TEST_F(timespecTest, SubNsecOflow) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a( i , 100);
		TSPEC E(i-1, 200);
		TSPEC c;

		timespec_subns(c, a, NANOSECONDS - 100);
		ASSERT_EQ(E, c);
	}
}

// test negation
TEST_F(timespecTest, Neg) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a( i , 100);
		TSPEC b;
		TSPEC c;

		timespec_neg(b, a);
		timespec_add(c, a, b);
		ASSERT_EQ(0, timespec_test(c));
	}
}

// test abs value
TEST_F(timespecTest, AbsNoFrac) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i , 0);
		TSPEC b;
		int   c;

		c = timespec_abs(b, a);
		ASSERT_EQ((i < 0), c);
		ASSERT_EQ((i != 0), timespec_test(b));
	}
}

TEST_F(timespecTest, AbsWithFrac) {
	for (int i = -4; i <= 4; ++i) {
		TSPEC a(i , 100);
		TSPEC b;
		int   c;

		c = timespec_abs(b, a);
		ASSERT_EQ((i < 0), c);
		ASSERT_EQ(1, timespec_test(b));
	}
}

// conversion to l_fp
TEST_F(timespecTest, ToLFPrelPos) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		TSPEC a(1, fdata[i].nsec);
		LFP   E(1, fdata[i].frac);
		LFP   r;

		timespec_reltolfp(r, a);
		ASSERT_EQ(E, r);
	}
}

TEST_F(timespecTest, ToLFPrelNeg) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		TSPEC a(-1, fdata[i].nsec);
		LFP   E(~0, fdata[i].frac);
		LFP   r;

		timespec_reltolfp(r, a);
		ASSERT_EQ(E, r);
	}
}

TEST_F(timespecTest, ToLFPabs) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		TSPEC a(1, fdata[i].nsec);
		LFP   E(1 + JAN_1970, fdata[i].frac);
		LFP   r;

		timespec_abstolfp(r, a);
		ASSERT_EQ(E, r);
	}
}


TEST_F(timespecTest, ToString) {
	static const struct {
		time_t		sec;
		long		nsec;
		const char *	repr;
	} data [] = {
		{ 0, 0,	 "0.000000000" },
		{ 2, 0,	 "2.000000000" },
		{-2, 0, "-2.000000000" },
		{ 0, 1,	 "0.000000001" },
		{ 1,-1,	 "0.999999999" },
		{-1, 1, "-0.999999999" }
	};
	for (int i = 0; i < COUNTOF(data); i++) {
		TSPEC a(data[i].sec, data[i].nsec);
		std::string E(data[i].repr);
		std::string r(timespec_tostr(a));

		ASSERT_EQ(E, r);
	}
}

// -*- EOF -*-
