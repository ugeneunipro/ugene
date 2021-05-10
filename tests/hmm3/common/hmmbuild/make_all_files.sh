# default options
./hmmbuild 1-cysPrx_C.hmm 1-cysPrx_C.sto
./hmmbuild 14-3-3.hmm 14-3-3.sto
./hmmbuild 2-Hacid_dh.hmm 2-Hacid_dh.sto
./hmmbuild 2-Hacid_dh_C.hmm 2-Hacid_dh_C.sto
./hmmbuild 2-oxoacid_dh.hmm 2-oxoacid_dh.sto
./hmmbuild 2_msa.hmm 2_msa.sto
./hmmbuild 5_msa.hmm 5_msa.sto
./hmmbuild ABC_tran.hmm ABC_tran.sto
./hmmbuild Asp.hmm Asp.sto
./hmmbuild ATP-synt_ab.hmm ATP-synt_ab.sto
./hmmbuild Caudal_act.hmm Caudal_act.sto
./hmmbuild fn3.hmm fn3.sto
./hmmbuild --dna simple_msa.hmm simple_msa.sto
./hmmbuild --dna _names_.hmm _names_.sto

# fast, blosum
./hmmbuild --seed 100 --fast --wblosum --wid 0.88 --eclust 14-3-3_wblosum_fast_eclust.hmm 14-3-3.sto
./hmmbuild --seed 40 --fast --wblosum --eclust --eid 0.44 ATP-synt_ab_wblosum_fast_eclust.hmm ATP-synt_ab.sto
./hmmbuild --fast --wblosum --wid 0.9 --eent fn3_wblosum_fast_eent.hmm fn3.sto
./hmmbuild --fast --wblosum --eent --ere 7.9 1-cysPrx_C_wblosum_fast_eent2.hmm 1-cysPrx_C.sto
./hmmbuild --seed 15 --fast --wblosum --wid 0.57 --enone 2-Hacid_dh_C_wblosum_fast_enone.hmm 2-Hacid_dh_C.sto

# fast gsc
./hmmbuild --fast --wgsc --eclust 2-oxoacid_dh_gsc_fast_eclust.hmm 2-oxoacid_dh.sto
./hmmbuild --seed 51 --fast --wgsc --eclust --eid 0.3 Asp_gsc_fast_eclust.hmm Asp.sto
./hmmbuild --fast --wgsc --eent fn3_gsc_fast_eent.hmm fn3.sto
./hmmbuild --seed 19 --fast --wgsc --eent --ere 1.5 ABC_tran_gsc_fast_eent.hmm ABC_tran.sto
./hmmbuild --seed 10 --fast --wgsc --eent 1-cysPrx_C_gsc_fast_eent3.hmm 1-cysPrx_C.sto
./hmmbuild --fast --wgsc --enone fn3_gsc_fast_enone.hmm fn3.sto
./hmmbuild --fast --wgsc --enone --EvL 110 --EvN 215 --EfL 90 --EfN 178 --Eft 0.23 ABC_tran_gsc_fast_enone.hmm ABC_tran.sto
./hmmbuild --seed 22 --fast --wgsc --eset 100 2-Hacid_dh_C_gsc_fast_eset.hmm 2-Hacid_dh_C.sto

# fast none
./hmmbuild --fast --wnone --eclust --eid 0.75 14-3-3_wnone_fast_eclust.hmm 14-3-3.sto
./hmmbuild --seed 157 --fast --wnone --eent --ere 1.7 fn3_wnone_fast_eent.hmm fn3.sto
./hmmbuild --seed 1 --fast --wnone --eent 1-cysPrx_C_wnone_fast_eent.hmm 1-cysPrx_C.sto
./hmmbuild --seed 101 --fast --wnone --enone 2-Hacid_dh_C_wnone_fast_enone.hmm 2-Hacid_dh_C.sto
./hmmbuild --fast --wnone --eset 11.4 --EvL 150 --EvN 220 --EfL 123 --EfN 197 --Eft 0.01 2-Hacid_dh_wnone_fast_eset.hmm 2-Hacid_dh.sto

# fast pb
./hmmbuild --seed 1024 --fast --wpb --eclust fn3_wpb_fast_eclust.hmm fn3.sto
./hmmbuild --fast --wpb --eclust --eid 0.77 1-cysPrx_C_wpb_fast_eclust2.hmm 1-cysPrx_C.sto
./hmmbuild --seed 512 --fast --wpb --eent --ere 1.91 Asp_wpb_fast_eent.hmm Asp.sto
./hmmbuild --seed 39 --fast --wpb --eent 2-Hacid_dh_C_wpb_fast_eent.hmm 2-Hacid_dh_C.sto
./hmmbuild --fast --wpb --enone ATP-synt_ab_wpb_fast_enone.hmm ATP-synt_ab.sto

# hand blosum
./hmmbuild --hand --wblosum --wid 0.88 --eclust ABC_tran_wblosum_hand_eclust.hmm ABC_tran.sto
./hmmbuild --hand --wblosum --eclust --eid 0.44 ATP-synt_ab_wblosum_hand_eclust.hmm ATP-synt_ab.sto
./hmmbuild --seed 69 --hand --wblosum --wid 0.9 --eent 2-Hacid_dh_wblosum_hand_eent.hmm 2-Hacid_dh.sto
./hmmbuild --seed 99 --hand --wblosum --eent --ere 7.9 2-Hacid_dh_wblosum_hand_eent2.hmm 2-Hacid_dh.sto
./hmmbuild --seed 31 --hand --wblosum --wid 0.57 --enone Asp_wblosum_hand_enone.hmm Asp.sto

# hand gsc
./hmmbuild --seed 44 --hand --wgsc --eclust 2-Hacid_dh_gsc_hand_eclust.hmm 2-Hacid_dh.sto
./hmmbuild --hand --wgsc --eclust --eid 0.3 Asp_gsc_hand_eclust.hmm Asp.sto
./hmmbuild --seed 4343 --hand --wgsc --eent 2-Hacid_dh_gsc_hand_eent.hmm 2-Hacid_dh.sto
./hmmbuild --hand --wgsc --eent --ere 1.5 ABC_tran_gsc_hand_eent.hmm ABC_tran.sto
./hmmbuild --hand --wgsc --eent ABC_tran_gsc_hand_eent3.hmm ABC_tran.sto
./hmmbuild --seed 10000 --hand --wgsc --enone Asp_gsc_hand_enone.hmm Asp.sto
./hmmbuild --hand --wgsc --enone --EvL 110 --EvN 215 --EfL 90 --EfN 178 --Eft 0.23 ABC_tran_gsc_hand_enone.hmm ABC_tran.sto
./hmmbuild --seed 202 --hand --wgsc --eset 100 2-Hacid_dh_gsc_hand_eset.hmm 2-Hacid_dh.sto

# hand none
./hmmbuild --hand --wnone --eclust --eid 0.75 ABC_tran_wnone_hand_eclust.hmm ABC_tran.sto
./hmmbuild --seed 31 --hand --wnone --eent --ere 1.7 Asp_wnone_hand_eent.hmm Asp.sto
./hmmbuild --seed 1001 --hand --wnone --eent ABC_tran_wnone_hand_eent.hmm ABC_tran.sto
./hmmbuild --seed 788 --hand --wnone --enone 2-Hacid_dh_wnone_hand_enone.hmm 2-Hacid_dh.sto
./hmmbuild --hand --wnone --eset 11.4 --EvL 150 --EvN 220 --EfL 123 --EfN 197 --Eft 0.01 2-Hacid_dh_wnone_hand_eset.hmm 2-Hacid_dh.sto

# hand pb
./hmmbuild --seed 100 --hand --wpb --eclust ABC_tran_wpb_hand_eclust.hmm ABC_tran.sto
./hmmbuild --hand --wpb --eclust --eid 0.77 ABC_tran_wpb_hand_eclust2.hmm ABC_tran.sto
./hmmbuild --hand --wpb --eent --ere 1.91 Asp_wpb_hand_eent.hmm Asp.sto
./hmmbuild --hand --wpb --eent 2-Hacid_dh_wpb_hand_eent.hmm 2-Hacid_dh.sto
./hmmbuild --hand --wpb --enone ATP-synt_ab_wpb_hand_enone.hmm ATP-synt_ab.sto
