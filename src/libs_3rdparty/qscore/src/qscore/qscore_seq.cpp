#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

#if	0

const size_t MAX_FASTA_LINE = 4*4096;

void Seq::SetName(const char *ptrName)
	{
	delete[] m_ptrName;
	size_t n = strlen(ptrName) + 1;
	m_ptrName = new char[n];
	strcpy(m_ptrName, ptrName);
	}

void Seq::ToFASTAFile(TextFile &File) const
	{
	File.PutFormat(">%s\n", m_ptrName);
	unsigned uColCount = Length();
	for (unsigned n = 0; n < uColCount; ++n)
		{
		if (n > 0 && n%60 == 0)
			File.PutString("\n");
		File.PutChar(at(n));
		}
	File.PutString("\n");
	}

// Return true on end-of-file
bool Seq::FromFASTAFile(TextFile &File)
	{
	Clear();

	char szLine[MAX_FASTA_LINE];
	bool bEof = File.GetLine(szLine, sizeof(szLine));
	if (bEof)
		return true;
	if ('>' != szLine[0])
		Quit_Qscore("Expecting '>' in FASTA file %s line %u",
		  File.GetFileName(), File.GetLineNr());

// Truncate name at first blank
// TODO: This is a hack because readseq does MSF -> FASTA
// conversion in a very strange way with the annotation.
	//char *ptrBlank = strchr(szLine, ' ');
	//if (0 != ptrBlank)
	//	*ptrBlank = 0;

	size_t n = strlen(szLine);
	if (1 == n)
		Quit_Qscore("Missing name following '>' in FASTA file %s line %u",
		  File.GetFileName(), File.GetLineNr());

	m_ptrName = new char[n];
	strcpy(m_ptrName, szLine + 1);

	TEXTFILEPOS Pos = File.GetPos();
	for (;;)
		{
		bEof = File.GetLine(szLine, sizeof(szLine));
		if (bEof)
			{
			if (0 == size())
				{
				Quit_Qscore("Empty sequence in FASTA file %s line %u",
				  File.GetFileName(), File.GetLineNr());
				return true;
				}
			return false;
			}
		if ('>' == szLine[0])
			{
			if (0 == size())
				Quit_Qscore("Empty sequence in FASTA file %s line %u",
				  File.GetFileName(), File.GetLineNr());
		// Rewind to beginning of this line, it's the start of the
		// next sequence.
			File.SetPos(Pos);
			return false;
			}
		const char *ptrChar = szLine;
		while (char c = *ptrChar++)
			push_back(c);
		Pos = File.GetPos();
		}
	}

void Seq::ExtractUngapped(MSA &msa) const
	{
	msa.Clear();
	unsigned uColCount = Length();
	msa.SetSize(1, 1);
	unsigned uUngappedPos = 0;
	for (unsigned n = 0; n < uColCount; ++n)
		{
		char c = at(n);
		if (!IsGap(c))
			msa.SetChar(0, uUngappedPos++, c);
		}
	msa.SetSeqName(0, m_ptrName);
	}

void Seq::Copy(const Seq &rhs)
	{
	clear();
	const unsigned uLength = rhs.Length();
	for (unsigned uColIndex = 0; uColIndex < uLength; ++uColIndex)
		push_back(rhs.at(uColIndex));
	const char *ptrName = rhs.GetName();
	size_t n = strlen(ptrName) + 1;
	m_ptrName = new char[n];
	strcpy(m_ptrName, ptrName);
	}

void Seq::CopyReversed(const Seq &rhs)
	{
	clear();
	const unsigned uLength = rhs.Length();
	const unsigned uBase = rhs.Length() - 1;
	for (unsigned uColIndex = 0; uColIndex < uLength; ++uColIndex)
		push_back(rhs.at(uBase - uColIndex));
	const char *ptrName = rhs.GetName();
	size_t n = strlen(ptrName) + 1;
	m_ptrName = new char[n];
	strcpy(m_ptrName, ptrName);
	}

void Seq::StripGaps()
	{
	for (CharVect::iterator p = begin(); p != end(); )
		{
		char c = *p;
		if (IsGap(c))
			erase(p);
		else
			++p;
		}
	}

void Seq::ToUpper()
	{
	for (CharVect::iterator p = begin(); p != end(); ++p)
		{
		char c = *p;
		if (islower(c))
			*p = toupper(c);
		}
	}

unsigned Seq::GetLetter(unsigned uIndex) const
	{
	assert(uIndex < Length());
	char c = operator[](uIndex);
	return CharToLetterAmino(c);
	}

bool Seq::EqIgnoreCase(const Seq &s) const
	{
	const unsigned n = Length();
	if (n != s.Length())
		{
		return false;
		}
	for (unsigned i = 0; i < n; ++i)
		{
		const char c1 = at(i);
		const char c2 = s.at(i);
		if (IsGap(c1))
			{
			if (!IsGap(c2))
				return false;
			}
		else
			{
			if (toupper(c1) != toupper(c2))
				{
				return false;
				}
			}
		}
	return true;
	}

bool Seq::EqIgnoreCaseAndGaps(const Seq &s) const
	{
	const unsigned uThisLength = Length();
	const unsigned uOtherLength = s.Length();
	
	unsigned uThisPos = 0;
	unsigned uOtherPos = 0;

	int cThis;
	int cOther;
	for (;;)
		{
		if (uThisPos == uThisLength && uOtherPos == uOtherLength)
			break;

	// Set cThis to next non-gap character in this string
	// or -1 if end-of-string.
		for (;;)
			{
			if (uThisPos == uThisLength)
				{
				cThis = -1;
				break;
				}
			else
				{
				cThis = at(uThisPos);
				++uThisPos;
				if (!IsGap(cThis))
					{
					cThis = toupper(cThis);
					break;
					}
				}
			}

	// Set cOther to next non-gap character in s
	// or -1 if end-of-string.
		for (;;)
			{
			if (uOtherPos == uOtherLength)
				{
				cOther = -1;
				break;
				}
			else
				{
				cOther = s.at(uOtherPos);
				++uOtherPos;
				if (!IsGap(cOther))
					{
					cOther = toupper(cOther);
					break;
					}
				}
			}

	// Compare characters are corresponding ungapped position
		if (cThis != cOther)
			return false;
		}
	return true;
	}

unsigned Seq::GetUngappedLength() const
	{
	unsigned uUngappedLength = 0;
	for (CharVect::const_iterator p = begin(); p != end(); ++p)
		{
		char c = *p;
		if (!IsGap(c))
			++uUngappedLength;
		}
	return uUngappedLength;
	}

void Seq::ListMe() const
	{
	List(">%s\n", m_ptrName);
	const unsigned n = Length();
	for (unsigned i = 0; i < n; ++i)
		List("%c", at(i));
	List("\n");
	}

void Seq::FromString(const char *pstrSeq, const char *pstrName)
	{
	clear();
	const unsigned uLength = (unsigned) strlen(pstrSeq);
	for (unsigned uColIndex = 0; uColIndex < uLength; ++uColIndex)
		push_back(pstrSeq[uColIndex]);
	size_t n = strlen(pstrName) + 1;
	m_ptrName = new char[n];
	strcpy(m_ptrName, pstrName);
	}

bool Seq::HasGap() const
	{
	for (CharVect::const_iterator p = begin(); p != end(); ++p)
		{
		char c = *p;
		if (IsGap(c))
			return true;
		}
	return false;
	}
#endif
