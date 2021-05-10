# file contains commands to make all .out files needed to hmmer3 search tests
#general tests

#default_options
./hmmsearch simple.hmm simple.fa >g_simple_hmm_simple_fa.out
./hmmsearch dna_def_4.hmm 4.fa >g_4_hmm_4_fa.out
./hmmsearch tt.hmm 3.fa >g_tt_hmm_3_fa.out
./hmmsearch simple.hmm 3.fa >g_simple_hmm_3_fa.out
./hmmsearch fn3.hmm titin.fa >g_fn3_hmm_titin_fa.out
./hmmsearch Asp.hmm titin.fa >g_asp_hmm_titin_fa.out
./hmmsearch Asp.hmm pepsin.fa >g_asp_hmm_pepsin_fa.out
./hmmsearch 14-3-3.hmm 14-3-3_epsilon2.fa >g_14-3-3_hmm_epsilon2_fa.out
./hmmsearch Asp.hmm pepsin2.fa >g_asp_hmm_pepsin2_fa.out
./hmmsearch COI.hmm COI.fa >g_COI_hmm_COI_fa.out
./hmmsearch COI.hmm COI2.fa >g_COI_hmm_COI2_fa.out
./hmmsearch simple.hmm COI.fa >g_simple_hmm_COI_fa.out
./hmmsearch COIrd.hmm COIrd.fa >g_COIrd_hmm_COIrd_fa.out
./hmmsearch simpler.hmm COIrd.fa >g_simpler_hmm_COIrd_fa.out
./hmmsearch COIre.hmm COIre.fa >g_COIre_hmm_COIre_fa.out
./hmmsearch simpler.hmm simpler.fa >g_simpler_hmm_simpler_fa.out

#eval thresholds
./hmmsearch --seed 100 -E 1.34e-139 Asp.hmm pepsin.fa >e_asp_hmm_pepsin_fa.out
./hmmsearch -E 1.0e-100 -Z 20 Asp.hmm pepsin2.fa >e_z_asp_hmm_pepsin2_fa.out
./hmmsearch --domE 1.0e-17 fn3.hmm titin.fa >e_dom_fn3_hmm_titin_fa.out
./hmmsearch -E 1.0e-80 --domE 1.0e-40 --domZ 50 COI.hmm COI2.fa >e_seq_domZ_COI_hmm_COI2_fa.out
./hmmsearch --seed 200 --domE 4.2e-27 COIre.hmm COIre.fa >e_dom_COIre_hmm_COIre_fa.out

#score thresholds
./hmmsearch --seed 21 --domT 100 COIre.hmm COIre.fa >s_dom_COIre_hmm_COIre_fa.out
./hmmsearch -T 200 --domT 150 COI.hmm COI2.fa >s_seq_dom_COI_hmm_COI2_fa.out
./hmmsearch --seed 100 -T 440 Asp.hmm pepsin.fa >s_asp_hmm_pepsin_fa.out
./hmmsearch -T 630 Asp.hmm pepsin2.fa >s2_asp_hmm_pepsin2_fa.out
./hmmsearch --domT 47 fn3.hmm titin.fa >s_dom_fn3_hmm_titin_fa.out

#acceleration heuristics
./hmmsearch --max 14-3-3.hmm 14-3-3_epsilon2.fa >ah_14-3-3_hmm_epsilon2_fa.out
./hmmsearch --nobias Asp.hmm pepsin.fa >ah_asp_hmm_pepsin_fa.out
./hmmsearch --nonull2 Asp.hmm pepsin2.fa >ah_asp_hmm_pepsin2_fa.out
./hmmsearch --nobias --nonull2 fn3.hmm titin.fa >ah_fn3_hmm_titin_fa.out
