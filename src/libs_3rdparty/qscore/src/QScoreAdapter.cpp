#include "QScoreAdapter.h"
#include "qscore/qscore.h"

extern double QScore(MSA_QScore* _msaTest, MSA_QScore* _msaRef);

namespace U2 {
  
  static void convertMAlignment2MSA(MSA_QScore& muscleMSA, const MultipleSequenceAlignment& ma){

    for (int i=0, n = ma->getRowCount(); i<n; i++) {
      const MultipleSequenceAlignmentRow aseq = ma->getMsaRow(i);

      char *sequence = new char[ma->getLength() + 1];
      for(int position = 0;position < ma->getLength();position++) {
          sequence[position] = aseq->charAt(position);
      }
      sequence[ma->getLength()] = '\0';

      char* name = new char[aseq->getName().length() + 1];
      memcpy(name, aseq->getName().toLocal8Bit().data(), aseq->getName().length());
      name[aseq->getName().length()] = '\0';

      muscleMSA.AppendSeq(sequence, ma->getLength(), name);
    }
  }

  double QScore(const MultipleSequenceAlignment& maTest, const MultipleSequenceAlignment& maRef, TaskStateInfo& ti) {
    MSA_QScore msaTest, msaRef;
    try {
      convertMAlignment2MSA(msaTest, maTest);
      convertMAlignment2MSA(msaRef, maRef);
      return QScore(&msaTest, &msaRef);
    } catch(const QScoreException &e) {
      ti.setError(QString("QScore throws exception: %1").arg(e.str));
      return 0;
    }

  }
} // namespace U2 
