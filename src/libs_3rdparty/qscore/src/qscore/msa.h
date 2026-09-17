#ifndef	MSA_h
#define MSA_h

#include <string>
#include <vector>
#include <map>

extern void Quit_Qscore(const char szFormat[], ...);

class MSA_QScore
	{
public:
	MSA_QScore();
	virtual ~MSA_QScore();

public:
	void FromFASTAFile(FILE *f);

	void SetSeqCount(unsigned uSeqCount);
	bool IsGap(unsigned uSeqIndex, unsigned uColIndex) const;

	void SetChar(unsigned uSeqIndex, unsigned uColIndex, char c);
	char GetChar(unsigned uSeqIndex, unsigned uIndex) const
		{
#if	_DEBUG
		if (uSeqIndex >= m_uSeqCount || uIndex >= m_uColCount)
			Quit_Qscore("MSA::GetChar(%u/%u,%u/%u)", uSeqIndex, m_uSeqCount, uIndex, m_uColCount);
#endif

		char c = m_szSeqs[uSeqIndex][uIndex];
	//	assert(IsLegalChar(c));
		return c;
		}

	void SetSeqName(unsigned uSeqIndex, const char szName[]);
	const char *GetSeqName(unsigned uSeqIndex) const;

	bool GetSeqIndex(const char *ptrSeqName, unsigned *ptruSeqIndex) const;

	unsigned GetCharCount(unsigned uSeqIndex, unsigned uColIndex) const;
	const char *GetSeqBuffer(unsigned uSeqIndex) const;
	unsigned GetSeqLength(unsigned uSeqIndex) const;

	void GetPairMap(unsigned uSeqIndex1, unsigned uSeqIndex2, int iMap1[],
	  int iMap2[]) const;
	unsigned GetUngappedColIndex(unsigned uSeqIndex, unsigned uColIndex);
	unsigned GetGappedColIndex(unsigned uSeqIndex, unsigned uUngappedColIndex);
	void AppendSeq(char *ptrSeq, unsigned uSeqLength, char *ptrLabel);
	void ExpandCache(unsigned uSeqCount, unsigned uColCount);

	void Free();
	void Clear()
		{
		Free();
		}
	unsigned GetSeqCount() const
		{
		return m_uSeqCount;
		}
	unsigned GetColCount() const
		{
		return m_uColCount;
		}
	char LetterToChar(unsigned uLetter) const
		{
		return ::LetterToChar(uLetter);
		}
	char CharToLetter(char c) const
		{
		return ::CharToLetter(c);
		}

private:
	void MakeGapMap();
	void MakeGapMapSeq(unsigned uSeqIndex);
	void MakeUngapMap();
	void MakeUngapMapSeq(unsigned uSeqIndex);

private:
	unsigned m_uSeqCount;
	unsigned m_uColCount;
	unsigned m_uCacheSeqCount;
	char **m_szSeqs;
	//char **m_szNames;
	std::vector<std::string> m_SeqNames;
	char *m_SeqBuffer;
	unsigned **m_UngapMap;
	unsigned **m_GapMap;
	unsigned *m_SeqLengths;
	std::map<std::string, unsigned> m_SeqNameToIndex;
	};

#endif	// MSA_h
