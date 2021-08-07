#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

struct VALUE_OPT
	{
	const char *m_pstrName;
	const char *m_pstrValue;
	};

struct FLAG_OPT
	{
	const char *m_pstrName;
	bool m_bSet;
	};

static VALUE_OPT ValueOpts[] =
	{
	{ "test",					0, },
	{ "ref",					0, },
	{ "sab_test",				0, },
	{ "sab_ref",				0, },
	};
static int ValueOptCount = sizeof(ValueOpts)/sizeof(ValueOpts[0]);

static FLAG_OPT FlagOpts[] =
	{
	{ "truncname",			false, },
	{ "ignoretestcase",		false, },
	{ "ignorerefcase",		false, },
	{ "quiet",				false, },
	{ "cline",				false, },
	{ "modeler",			false, },
	{ "slow",				false, },
	{ "version",			false, },
	{ "gapscore",			false, },
	{ "seqdiffwarn",		false, },
	{ "ignoremissingseqs",	false, },
	{ "perseq",				false, },
	{ "verbose",			false, },
	{ "stripx",				false, },
	{ "stripb",				false, },
	{ "stripz",				false, },
	};
static int FlagOptCount = sizeof(FlagOpts)/sizeof(FlagOpts[0]);

static bool TestSetFlagOpt(const char *Arg)
	{
	for (int i = 0; i < FlagOptCount; ++i)
		if (!strcmp(Arg, FlagOpts[i].m_pstrName))
			{
			FlagOpts[i].m_bSet = true;
			return true;
			}
	return false;
	}

static bool TestSetValueOpt(const char *Arg, const char *Value)
	{
	for (int i = 0; i < ValueOptCount; ++i)
		if (!strcmp(Arg, ValueOpts[i].m_pstrName))
			{
			if (0 == Value)
				{
				//k_printf( "Option -%s must have value\n", Arg);
				Quit_Qscore("Option -%s must have value\n", Arg);
				}
			ValueOpts[i].m_pstrValue = strdup(Value);
			return true;
			}
	return false;
	}

bool FlagOpt_QScore(const char *Name)
	{
	for (int i = 0; i < FlagOptCount; ++i)
		if (!strcmp(Name, FlagOpts[i].m_pstrName))
			return FlagOpts[i].m_bSet;
	Quit_Qscore("FlagOpt(%s) invalid", Name);
	return false;
	}

const char *ValueOpt_QScore(const char *Name)
	{
	for (int i = 0; i < ValueOptCount; ++i)
		if (!strcmp(Name, ValueOpts[i].m_pstrName))
			return ValueOpts[i].m_pstrValue;
	Quit_Qscore("ValueOpt(%s) invalid", Name);
	return 0;
	}

const char *RequiredValueOpt(const char *Name)
	{
	const char *s = ValueOpt_QScore(Name);
	if (0 == s)
		Quit_Qscore("Required option -%s not specified", Name);
	return s;
	}
