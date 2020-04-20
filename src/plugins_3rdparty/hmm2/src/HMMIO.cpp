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

#include "HMMIO.h"
#include "hmmer2/funcs.h"
#include "TaskLocalStorage.h"

#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Task.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/TextUtils.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Gui/DialogUtils.h>

#include <QFileInfo>

namespace U2 {

/* TRANSLATOR U2::IOAdapter */

static const char * prob2ascii(float p, float null) {
  HMMERTaskLocalData *tld = getHMMERTaskLocalData();
  //static char buffer[8];
  char *buffer = tld->buffer;
  if (p == 0.0) {
      return "*";
  }
  snprintf(buffer, 8, "%6d", Prob2Score(p, null));
  return buffer;
}

static float ascii2prob(char *s, float null) {
        return (*s == '*') ? 0. : Score2Prob(atoi(s), null);
}

static void multiline(QString& res, const QString& prefix, const char* s) {
    if (s == NULL) {
        return;
    }
    QStringList lines = QString(s).split('\n');
    foreach(const QString& line, lines) {
        res+= prefix + line + '\n';
    }
}

void HMMIO::writeHMM2(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, plan7_s *hmm)
{	
    if (hmm->atype == hmmNOTSETYET) {
        si.setError(  tr("Alphabet is not set") );
        return;
    }
    //get HMMERTaskLocalData
	HMMERTaskLocalData *tld = getHMMERTaskLocalData();
	alphabet_s &al = tld->al;

    SetAlphabet(hmm->atype);

    QString res;
    res+=QString().sprintf("HMMER2.0  [%s]\n", PACKAGE_VERSION);  // magic header

    //write header information
    res+=QString().sprintf("NAME  %s\n", hmm->name);
    if (hmm->flags & PLAN7_ACC) {
        res+=QString().sprintf("ACC   %s\n", hmm->acc);
    }
    if (hmm->flags & PLAN7_DESC) {
        res+=QString().sprintf("DESC  %s\n", hmm->desc);
    }
    res+=QString().sprintf("LENG  %d\n", hmm->M);
    res+=QString().sprintf("ALPH  %s\n", (al.Alphabet_type == hmmAMINO) ? "Amino":"Nucleic");
    res+=QString().sprintf("RF    %s\n", (hmm->flags & PLAN7_RF)  ? "yes" : "no");
    res+=QString().sprintf("CS    %s\n", (hmm->flags & PLAN7_CS)  ? "yes" : "no");
    res+=QString().sprintf("MAP   %s\n", (hmm->flags & PLAN7_MAP) ? "yes" : "no");
    multiline(res, "COM   ", hmm->comlog);
    res+=QString().sprintf("NSEQ  %d\n", hmm->nseq);
    res+=QString().sprintf("DATE  %s\n", hmm->ctime); 
    res+=QString().sprintf("CKSUM %d\n", hmm->checksum);
    if (hmm->flags & PLAN7_GA) {
        res+=QString().sprintf("GA    %.1f %.1f\n", hmm->ga1, hmm->ga2);
    }
    if (hmm->flags & PLAN7_TC) {
        res+=QString().sprintf("TC    %.1f %.1f\n", hmm->tc1, hmm->tc2);
    }
    if (hmm->flags & PLAN7_NC) {
        res+=QString().sprintf("NC    %.1f %.1f\n", hmm->nc1, hmm->nc2);
    }

    // Specials
    res+="XT     ";
    for (int k = 0; k < 4; k++) {
        for (int x = 0; x < 2; x++) {
            res+=QString().sprintf("%6s ", prob2ascii(hmm->xt[k][x], 1.0));
        }
    }
    res+="\n";
    

    /* Save the null model first, so HMM readers can decode
    * log odds scores on the fly. Save as log odds probabilities
    * relative to 1/Alphabet_size (flat distribution)
    */
    res+=QString().sprintf("NULT  ");
    res+=QString().sprintf("%6s ", prob2ascii(hmm->p1, 1.0)); /* p1 */
    res+=QString().sprintf("%6s\n", prob2ascii(1.0-hmm->p1, 1.0));   /* p2 */
    res+="NULE  ";
    for (int x = 0; x < al.Alphabet_size; x++) {
        res+=QString().sprintf("%6s ", prob2ascii(hmm->null[x], 1/(float)(al.Alphabet_size)));
    }
    res+="\n";

    // EVD statistics
    if (hmm->flags & PLAN7_STATS)  {
        res+=QString().sprintf("EVD   %10f %10f\n", hmm->mu, hmm->lambda);
    }

    // Print header
    res+=QString().sprintf("HMM      ");
    for (int x = 0; x < al.Alphabet_size; x++) {
        res+=QString().sprintf("  %c    ", al.Alphabet[x]);
    }
    res+=QString().sprintf("\n");
    res+=QString().sprintf("       %6s %6s %6s %6s %6s %6s %6s %6s %6s\n", 
                          "m->m", "m->i", "m->d", "i->m", "i->i", "d->m", "d->d", "b->m", "m->e");

    //Print HMM parameters (main section of the save file)
    res+=QString().sprintf("       %6s %6s ", prob2ascii(1-hmm->tbd1, 1.0), "*");
    res+=QString().sprintf("%6s\n", prob2ascii(hmm->tbd1, 1.0));
    for (int k = 1; k <= hmm->M; k++) {
        /* Line 1: k, match emissions, map */
        res+=QString().sprintf(" %5d ", k);
        for (int x = 0; x < al.Alphabet_size; x++)  {
            res+=QString().sprintf("%6s ", prob2ascii(hmm->mat[k][x], hmm->null[x]));
        }
        if (hmm->flags & PLAN7_MAP) res+=QString().sprintf("%5d", hmm->map[k]);
        res+="\n";
        /* Line 2: RF and insert emissions */
        res+=QString().sprintf(" %5c ", (hmm->flags & PLAN7_RF) ? hmm->rf[k] : '-');
        for (int x = 0; x < al.Alphabet_size; x++)  {
            res+=QString().sprintf("%6s ", (k < hmm->M) ? prob2ascii(hmm->ins[k][x], hmm->null[x]) : "*");
        }
        res+="\n";
        /* Line 3: CS and transition probs */
        res+=QString().sprintf(" %5c ", (hmm->flags & PLAN7_CS) ? hmm->cs[k] : '-');
        for (int ts = 0; ts < 7; ts++) {
            res+=QString().sprintf("%6s ", (k < hmm->M) ? prob2ascii(hmm->t[k][ts], 1.0) : "*"); 
        }
        res+=QString().sprintf("%6s ", prob2ascii(hmm->begin[k], 1.0));
        res+=QString().sprintf("%6s ", prob2ascii(hmm->end[k], 1.0));

        res+="\n";
    }
    res+="//\n";
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        si.setError(L10N::errorOpeningFileWrite(url));
        return;
    }
    int len = io->writeBlock(res.toLatin1());
    if (len != res.length()) {
        si.setError(L10N::errorWritingFile(url));
        return;
    }
    io->close();
}

void HMMIO::readHMM2(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si,  plan7_s **ret_hmm)
{
#define BUFF_SIZE 512

    char  buffer[BUFF_SIZE+1];
    char *s;
    int   M;
    float p;
    int   k, x;

	//get HMMERTaskLocalData
	HMMERTaskLocalData *tld = getHMMERTaskLocalData();
	alphabet_s &al = tld->al;
    
    struct plan7_s *hmm = NULL;
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        si.setError(L10N::errorOpeningFileRead(url));
        return;
    }
    const QByteArray& upper = TextUtils::UPPER_CASE_MAP;
    const QBitArray& lineBreaks = TextUtils::LINE_BREAKS;
    do { //use loop to be able to use 'break' out of it
        bool lineOk = true;
        int len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
        if (!lineOk) {
            si.setError(  tr("Illegal line") );
            break;
        }
        if (strncmp(buffer, "HMMER2.0", 8) != 0) {
            if (strncmp(buffer, "HMMER3", 6) == 0) {
                si.setError(tr("Model \"%1\" is a HMMER3 model, it can't be used with HMMER2.").arg(url));
            } else {
                si.setError(tr("File format is not supported"));
            }
            break;
        }

        //Get the header information: tag/value pairs in any order,
        //ignore unknown tags, stop when "HMM" is reached (signaling start of main model)
    
        hmm = AllocPlan7Shell();
        M = -1;
        char* next = NULL;
        while ((len  = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk)) != 0) {
            if (!lineOk) {
                si.setError(  tr("Illegal line") );
                break;
            }
            next = NULL;
            buffer[len] = '\0';

            if      (strncmp(buffer, "NAME ", 5) == 0) Plan7SetName(hmm, buffer+6);
            else if (strncmp(buffer, "ACC  ", 5) == 0) Plan7SetAccession(hmm, buffer+6);
            else if (strncmp(buffer, "DESC ", 5) == 0) Plan7SetDescription(hmm, buffer+6);
            else if (strncmp(buffer, "LENG ", 5) == 0) M = atoi(buffer+6);
            else if (strncmp(buffer, "NSEQ ", 5) == 0) hmm->nseq = atoi(buffer+6);
            else if (strncmp(buffer, "ALPH ", 5) == 0) {  // Alphabet type 
                TextUtils::translate(upper, buffer+6, 7);
                int   atype = hmmNOTSETYET; 
                if      (strncmp(buffer+6, "AMINO",   5) == 0) atype = hmmAMINO;
                else if (strncmp(buffer+6, "NUCLEIC", 7) == 0) atype = hmmNUCLEIC;
                else {
                    si.setError(  tr("Value is illegal: %1").arg("ALPH") );
                    break;
                };
                SetAlphabet(atype);
                hmm->atype = atype;
            } else if (strncmp(buffer, "RF   ", 5) == 0)  { // Reference annotation present? 
                if (upper.at(*(buffer+6)) == 'Y') {
                    hmm->flags |= PLAN7_RF;
                }
            } else if (strncmp(buffer, "CS   ", 5) == 0) {  // Consensus annotation present?
                    if (upper.at(*(buffer+6)) == 'Y') hmm->flags |= PLAN7_CS;
            } else if (strncmp(buffer, "MAP  ", 5) == 0) {  // Map annotation present? 
                if (upper.at(*(buffer+6)) == 'Y') hmm->flags |= PLAN7_MAP;
            } else if (strncmp(buffer, "COM  ", 5) == 0)  { // Command line log
                StringChop(buffer+6);
                if (hmm->comlog == NULL) {
                    hmm->comlog = Strdup(buffer+6);
                } else {
                    int oldSize = strlen(hmm->comlog) + 1;
                    int newSize = sizeof(char *) * (oldSize + strlen(buffer+6));
                    hmm->comlog = (char*)ReallocOrDie(hmm->comlog, newSize);
                    sre_strlcat(hmm->comlog, "\n", newSize);
                    sre_strlcat(hmm->comlog, buffer+6, newSize);
                }
            } else if (strncmp(buffer, "DATE ", 5) == 0) { // Date file created
                    StringChop(buffer+6);
                    hmm->ctime= Strdup(buffer+6); 
            } else if (strncmp(buffer, "GA   ", 5) == 0) {
                if ((s = strtok_r(buffer+6, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("GA") );
                    break;
                }
                hmm->ga1 = atof(s);
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("GA") );
                    break;
                }
                hmm->ga2 = atof(s);
                hmm->flags |= PLAN7_GA;
            } else if (strncmp(buffer, "TC   ", 5) == 0) {
                if ((s = strtok_r(buffer+6, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("TC") );
                    break;
                }
                hmm->tc1 = atof(s);
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("TC") );
                    break;
                }
                hmm->tc2 = atof(s);
                hmm->flags |= PLAN7_TC;
            } else if (strncmp(buffer, "NC   ", 5) == 0) {
                if ((s = strtok_r(buffer+6, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("NC") );
                    break;
                }
                hmm->nc1 = atof(s);
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("NC") );
                    break;
                }
                hmm->nc2 = atof(s);
                hmm->flags |= PLAN7_NC;
            } else if (strncmp(buffer, "XT   ", 5) == 0)  { // Special transition section
                if ((s = strtok_r(buffer+6, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("XT") );
                    break;
                }
                for (k = 0; k < 4; k++) {
                    for (x = 0; x < 2; x++) {
                        if (s == NULL) {
                            si.setError(  tr("Invalid file structure near %1").arg("XT") );
                            break;
                        }
                        hmm->xt[k][x] = ascii2prob(s, 1.0);
                        s = strtok_r(NULL, " \t\n", &next);
                    }
                }
            } else if (strncmp(buffer, "NULT ", 5) == 0) { // Null model transitions
                if ((s = strtok_r(buffer+6, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("NULT") );
                    break;
                }
                hmm->p1 = ascii2prob(s, 1.);
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("NULT") );
                    break;
                }
                hmm->p1 = hmm->p1 / (hmm->p1 + ascii2prob(s, 1.0));
            } else if (strncmp(buffer, "NULE ", 5) == 0) { //Null model emissions 
                if ( al.Alphabet_type == hmmNOTSETYET) {
                    si.setError(  tr("ALPH must precede NULE in HMM save files") );
                    break;
                }
                s = strtok_r(buffer+6, " \t\n", &next);
                for (x = 0; x < al.Alphabet_size; x++) {
                    if (s == NULL)  {
                        si.setError(  tr("Invalid file structure near %1").arg("NULE") );
                        break;
                    }
                    hmm->null[x] = ascii2prob(s, 1./(float)al.Alphabet_size);    
                    s = strtok_r(NULL, " \t\n", &next);
                }
            } else if (strncmp(buffer, "EVD  ", 5) == 0) { // EVD parameters
                hmm->flags |= PLAN7_STATS;
                if ((s = strtok_r(buffer+6, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("EVD - mu") );
                    break;
                }
                hmm->mu = atof(s);
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("EVD - lambda") );
                    break;
                }
                hmm->lambda = atof(s);
            } else if (strncmp(buffer, "CKSUM", 5) == 0) {
                hmm->checksum = atoi(buffer+6);
            } else if (strncmp(buffer, "HMM  ", 5) == 0) {
                break;
            }
        }
        if (si.hasError()) {
            break;
        }
        // partial check for mandatory fields
        if (M < 1)   {
            si.setError(  tr("Value is illegal: %1").arg("M") );
            break;
        }
        if (hmm->name == NULL) {
            si.setError(  tr("Value is illegal: %1").arg("name") );
            break;
        }
        if (al.Alphabet_type == hmmNOTSETYET) {
            si.setError(  tr("Value is not set for '%1'").arg("ALPH") );
            break;
        }

        // Main model section. Read as integer log odds, convert to probabilities
        AllocPlan7Body(hmm, M);  
        // skip an annotation line
        len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
        if (!lineOk) {
            si.setError(  tr("Illegal line") );
            break;
        }
        // parse tbd1 line
        len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
        if (!lineOk) {
            si.setError(  tr("Illegal line") );
            break;
        }
        buffer[len] = '\0';
        next = NULL;
        if ((s = strtok_r(buffer, " \t\n", &next)) == NULL) {
            si.setError(  tr("Invalid file structure near %1").arg("tbd1") );
            break;
        }
        p = ascii2prob(s, 1.0);
        if ((s = strtok_r(NULL,   " \t\n", &next)) == NULL) {
            si.setError(  tr("Invalid file structure near %1").arg("tbd1") );
            break;
        }
        if ((s = strtok_r(NULL,   " \t\n", &next)) == NULL) {
            si.setError(  tr("Invalid file structure near %1").arg("tbd1") );
            break;
        }
        hmm->tbd1 = ascii2prob(s, 1.0);
        hmm->tbd1 = hmm->tbd1 / (p + hmm->tbd1);

        // main model
        for (k = 1; k <= hmm->M; k++) {
            // Line 1: k, match emissions, map
            len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
            if (!lineOk) {
                si.setError(  tr("Illegal line") );
                break;
            }   
            next = NULL;
            buffer[len] = '\0';
            if ((s = strtok_r(buffer, " \t\n", &next)) == NULL) {
                si.setError(  tr("Invalid file structure near %1").arg("main model 1") );
                break;
            }
            if (atoi(s) != k) {
                si.setError(  tr("Invalid file structure near %1").arg("main model (k)") );
                break;
            }
            for (x = 0; x < al.Alphabet_size; x++) {
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("main model (mat)") );
                    break;
                }
                hmm->mat[k][x] = ascii2prob(s, hmm->null[x]);
            }
            if (hmm->flags & PLAN7_MAP) {
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                    si.setError(  tr("Invalid file structure near %1").arg("main model (map)") );
                    break;
                }
                hmm->map[k] = atoi(s);
            }

            // Line 2:  RF and insert emissions
            len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
            if (!lineOk) {
                si.setError(  tr("Illegal line") );
                break;
            }   
            buffer[len] = '\0';
            next = NULL;
            if ((s = strtok_r(buffer, " \t\n", &next)) == NULL) {
                si.setError(  tr("Invalid file structure near %1").arg("main model 2") );
                break;
            }
            if (hmm->flags & PLAN7_RF) {
                hmm->rf[k] = *s;
            }
            if (k < hmm->M) {
                for (x = 0; x < al.Alphabet_size; x++) {
                    if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                        si.setError(  tr("Invalid file structure near %1").arg("main model (ins)") );
                        break;
                    }
                    hmm->ins[k][x] = ascii2prob(s, hmm->null[x]);
                }
            }

            // Line 3: CS and transitions
            len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
            if (!lineOk) {
                si.setError(  tr("Illegal line") );
                break;
            }   
            buffer[len] = '\0';
            next = NULL;
            if ((s = strtok_r(buffer, " \t\n", &next)) == NULL) {
                si.setError(  tr("Invalid file structure near %1").arg("main model 3") );
                break;
            }
            if (hmm->flags & PLAN7_CS) {
                hmm->cs[k] = *s;
            }
            for (x = 0; x < 7; x++) {
                if ((s = strtok_r(NULL, " \t\n", &next)) == NULL)  {
                    si.setError(  tr("Invalid file structure near %1").arg("main model (t)") );
                    break;
                }
                if (k < hmm->M) hmm->t[k][x] = ascii2prob(s, 1.0);
            }
            if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                si.setError(  tr("Invalid file structure near %1").arg("main model (begin)") );
                break;
            }
            hmm->begin[k] = ascii2prob(s, 1.0);
            if ((s = strtok_r(NULL, " \t\n", &next)) == NULL) {
                si.setError(  tr("Invalid file structure near %1").arg("main model (end)") );
                break;
            }
            hmm->end[k] = ascii2prob(s, 1.0);

        } // end loop over main model
        if (si.hasError()) {
            break;
        }

        // Advance to record separator
        do {
            len = io->readUntil(buffer, BUFF_SIZE, lineBreaks, IOAdapter::Term_Include, &lineOk);
            if (!lineOk) {
                si.setError(  tr("No '//' symbol found") );
                break;
            }
        } while (strncmp(buffer, "//", 2) != 0);
        if (si.hasError()) {
            break;
        }

        // Set flags and return
        hmm->flags |= PLAN7_HASPROB;  /* probabilities are valid */
        hmm->flags &= ~PLAN7_HASBITS; /* scores are not valid    */
		Plan7Renormalize(hmm);
        *ret_hmm = hmm;
    } while (false);
    
    io->close();
    
    if (si.hasError()) {
        if (hmm  != NULL) {
            FreePlan7(hmm);
        }
        *ret_hmm = NULL;
    }
}

const QString HMMIO::HMM_ID("hmmer");
const QString HMMIO::HMM_EXT("hmm");

plan7_s * HMMIO::cloneHMM( plan7_s * src ) {
    assert( src );
    int M = src->M;
    assert( M > 0 );
    plan7_s * dst = AllocPlan7( M );

    //copying model annotations
    assert( src->name ); //name is mandatory
    Plan7SetName( dst, src->name );
    
    if( src->acc ) {
        Plan7SetAccession( dst, src->acc );
    }
    if( src->desc ) {
        Plan7SetDescription( dst, src->desc );
    }

    if( src->rf ) {
        qCopy( src->rf, src->rf + M+2, &dst->rf[0] );
    }
    if( src->cs ) {
        qCopy( src->cs, src->cs + M+2, &dst->cs[0] );
    }
    if( src->ca ) {
        qCopy( src->ca, src->ca + M+2, &dst->ca[0] );
    }
    if( src->comlog ) {
        dst->comlog = Strdup( src->comlog );
    }
    if( src->ctime ) {
        dst->ctime = Strdup( src->ctime );
    }
    if( src->map ) {
        qCopy( src->map, src->map + M+1, &dst->map[0] );
    }
    dst->nseq     = src->nseq;
    dst->checksum = src->checksum;

    //Pfam-specific cutoffs:
    dst->ga1      = src->ga1;
    dst->ga2      = src->ga2;
    dst->tc1      = src->tc1;
    dst->tc2      = src->tc2;
    dst->nc1      = src->nc1;
    dst->nc2      = src->nc2;

    //probabilities
    dst->M        = src->M;
    //hack!
    qCopy( src->t[0],   src->t[0]   + M*7,           dst->t[0]    );
    qCopy( src->mat[0], src->mat[0] + (M+1)*MAXABET, dst->mat[0] );
    qCopy( src->ins[0], src->ins[0] + M*MAXABET,     dst->ins[0] );
    dst->tbd1     = src->tbd1;

    qCopy( &src->xt[0][0], &src->xt[0][0] + 8, &dst->xt[0][0] );
    qCopy( src->begin,      src->begin + M+1,  &dst->begin[0] );
    qCopy( src->end,        src->end +   M+1,  &dst->end[0]   );

    qCopy( src->null, src->null + MAXABET, &dst->null[0] );
    dst->p1       = src->p1;

    long tsc_s = (7*M);
    long msc_s = (MAXCODE*(M+1));
    long isc_s = (MAXCODE*(M));
    long bsc_s = (M+1);
    long esc_s = (M+1);
    long xsc_s = 8;

    qCopy( src->tsc_mem, src->tsc_mem + tsc_s, &dst->tsc_mem[0] );
    qCopy( src->msc_mem, src->msc_mem + msc_s, &dst->msc_mem[0] );
    qCopy( src->isc_mem, src->isc_mem + isc_s, &dst->isc_mem[0] );
    qCopy( src->bsc_mem, src->bsc_mem + bsc_s, &dst->bsc_mem[0] );
    qCopy( src->esc_mem, src->esc_mem + esc_s, &dst->esc_mem[0] );
    qCopy( &src->xsc[0][0], &src->xsc[0][0] + xsc_s, &dst->xsc[0][0] );

    //scoring parameters

    assert( !src->dnai && !src->dnam );

    dst->dna2     = src->dna2;
    dst->dna4     = src->dna4;
    dst->mu       = src->mu;
    dst->lambda   = src->lambda;
    dst->flags    = src->flags;
    dst->atype    = src->atype;
    
    return dst;
}

QString HMMIO::getHMMFileFilter() {
    return DialogUtils::prepareFileFilter(tr("HMM models"), QStringList(HMM_EXT));
}

DNAAlphabetType HMMIO::convertHMMAlphabet(int atype) {
    if (atype == hmmAMINO) {
        return DNAAlphabet_AMINO;
    } else if (atype == hmmNUCLEIC) {
        return DNAAlphabet_NUCL;
    }
    return DNAAlphabet_RAW;
}

HMMReadTask::HMMReadTask(const QString& _url) 
: Task("", TaskFlag_None), hmm(NULL), url(_url) 
{
    QString tn = tr("Read HMM profile '%1'.").arg(QFileInfo(url).fileName());
    setTaskName(tn);
}

HMMReadTask::~HMMReadTask() {
    if (hmm!=NULL) {
        FreePlan7(hmm);
    }
}

void HMMReadTask::run() {
    TaskLocalData::createHMMContext(getTaskId(), true);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    HMMIO::readHMM2(iof, url, stateInfo, &hmm);
    TaskLocalData::freeHMMContext(getTaskId());
}

HMMWriteTask::HMMWriteTask(const QString& url, plan7_s* s, uint f) 
: Task("", TaskFlag_None), url(url), hmm(s), fileMode(f)
{
    QString tn = tr("Write HMM profile '%1'").arg(QFileInfo(url).fileName());
    setTaskName(tn);
}

void HMMWriteTask::run() {
    TaskLocalData::createHMMContext(getTaskId(), true);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (fileMode & SaveDoc_Roll && !GUrlUtils::renameFileWithNameRoll(url, stateInfo)) {
        return;
    }
    HMMIO::writeHMM2(iof, url, stateInfo, hmm);
    TaskLocalData::freeHMMContext(getTaskId());
}

}//namespace
