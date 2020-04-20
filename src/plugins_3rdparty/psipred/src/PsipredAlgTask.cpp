/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <QDir>
#include <QTemporaryFile>

#include <U2View/SecStructPredictUtils.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/Counter.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include "PsipredAlgTask.h"
#include "sspred_avpred.h"
#include "sspred_hmulti.h"
#include "sspred_utils.h"

namespace U2 {

QMutex PsipredAlgTask::runLock;

PsipredAlgTask::PsipredAlgTask(const QByteArray& inputSeq) : SecStructPredictTask(inputSeq)
{
    GCOUNTER( cvar, tvar, "PsipredAlgTask" );
}

#define PSIPRED_ANNOTATION_NAME "psipred_results"

void PsipredAlgTask::run() 
{
    QMutexLocker runLocker( &runLock ); //TODO: BUG-0000808
    //TODO: get rid of this limit
    const int MAXSIZE = 10000;
    if (sequence.size() > MAXSIZE) {
        stateInfo.setError(SecStructPredictTask::tr("psipred: sequence is too long, max seq size is 10000"));
        return;
    }

    if(sequence.size() < 5){ //seq2mtx constraint
        stateInfo.setError(SecStructPredictTask::tr("psipred: sequence is too short, min seq size is 5"));
        return;
    }

    QTemporaryFile matrixFile; 
    try{
    seq2mtx(sequence.constData(), sequence.length(), &matrixFile);
    }catch(const char* msg){
        stateInfo.setError(QString("psipred error: %1").arg(msg));
        return;
    }
    matrixFile.reset();
    
    {
        QStringList weightFileNames;
        weightFileNames << ":psipred/datafiles/weights_s.dat";
        weightFileNames << ":psipred/datafiles/weights_s.dat2";
        weightFileNames << ":psipred/datafiles/weights_s.dat3";

        PsiPassOne pass1 (&matrixFile, weightFileNames);
        try{
            pass1.runPsiPass();
        }catch(const char* msg){
            stateInfo.setError(QString("psipred error: %1").arg(msg));
            return;
        }
    }

    const char* psipass2_args[] = 
    { 
        "empty", 
        ":psipred/datafiles/weights_p2.dat", "1", "1.0", "1.0", 
    };
    
    {
        PsiPassTwo pass2;
        try{
            pass2.runPsiPass(5, psipass2_args, output);
        }catch(const char* msg){
            stateInfo.setError(QString("psipred error: %1").arg(msg));
            return;
        }
    }

    results = SecStructPredictUtils::saveAlgorithmResultsAsAnnotations(output, PSIPRED_ANNOTATION_NAME);
    
    QString outputFileName=AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath() + QDir::separator() + "output.ss"; //File created at sspred_avpred.cpp in method predict
    QString outputFileName2=AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath() + QDir::separator() + "output.ss2"; //File created at sspred_hmulti.cpp in method runPsiPass
    QDir curDir;
    curDir.remove(outputFileName);
    curDir.remove(outputFileName2);
}



} //namespace



