# default options
./phmmer fibronectin1.2.fa titin.fa >fibronectin_titin.out
./phmmer tt_query.fa tt_db.fa >tt_query_tt_db.out
./phmmer fibronectin1.2_1.fa titin.fa >fibronectin_1_titin.out
./phmmer fibronectin1.2_2.fa titin.fa >fibronectin_2_titin.out
./phmmer 05_query.fa 05_db.fa >05_query_05_db.out
./phmmer rnd_query.fa rnd_db.fa >rnd_query_rnd_db.out
./phmmer rnd_query2.fa rnd_db.fa >rnd_query2_rnd_db.out
./phmmer rnd_query.fa titin.fa >rnd_query_titin.out
./phmmer NT_078103_query1.fa NT_078103.fa >NT_078103_query1_NT_078103_db.out
./phmmer NT_078103_query2.fa NT_078103.fa >NT_078103_query2_NT_078103_db.out
./phmmer fib_simple.fa titin.fa >fib_simple_query_titin_db.out

# evalue tresholds
./phmmer -E 3.29e-123 --domE 1e-4 fibronectin1.2.fa titin.fa >fibronectin_titin_seqE_domE.out
./phmmer -E 1e-80 --domE 1e-77 rnd_query2.fa rnd_db.fa >rnd_query2_rnd_db_seqE_domE.out
./phmmer --seed 55 --domE 1e-200 -Z 10.0 --domZ 5.5 rnd_query.fa rnd_db.fa >rnd_query_rnd_db_seed_domE_z_domZ.out
./phmmer --seed 1024 --domE 4e-4 fibronectin1.2_1.fa titin.fa >fibronectin_1_titin_seed_domE.out

# score tresholds
./phmmer -T 60 --domT 5.0 fib_simple.fa titin.fa >fib_simple_query_titin_db_domT_seqT.out
./phmmer -T 9.77 tt_query.fa tt_db.fa >tt_query_tt_db_seqT.out
./phmmer --domT 200.0 NT_078103_query1.fa NT_078103.fa >NT_078103_query1_NT_078103_db_domT.out
./phmmer --domT 31.5 fibronectin1.2_2.fa titin.fa >fibronectin_2_titin_domT.out

# eval calibration
./phmmer --EvL 50 --EvN 100 --EfL 50 --EfN 100 --Eft 0.01 tt_query.fa tt_db.fa >tt_query_tt_db_eval_calibration.out
./phmmer --EvL 150 --EvN 190 --EfL 109 --EfN 250 --Eft 0.5 rnd_query2.fa rnd_db.fa >rnd_query2_rnd_db_eval_calibration.out
./phmmer --seed 110 --EvL 15 --EvN 230 --EfL 36 --EfN 33 --Eft 0.99 rnd_query.fa rnd_db.fa >rnd_query_rnd_db_seed_eval_calibration.out
./phmmer --seed 1000 --EvL 50 --EvN 30 --EfL 1000 --EfN 1300 --Eft 0.00001 fibronectin1.2_1.fa titin.fa >fibronectin_1_titin_seed_eval_calibration.out

#acceleration heuristics
./phmmer --max fib_simple.fa titin.fa >fib_simple_query_titin_db_ah_max.out
./phmmer --nobias fibronectin1.2.fa titin.fa >fibronectin_titin_nobias.out
./phmmer --nonull2 rnd_query2.fa rnd_db.fa >rnd_query2_rnd_db_nonull2.out
./phmmer --nobias --nonull2 NT_078103_query2.fa NT_078103.fa >NT_078103_query2_NT_078103_db_nobias_nonull2.out
./phmmer --F1 0.4 --F2 0.1 --F3 0.5 rnd_query.fa rnd_db.fa >rnd_query_rnd_db_f1_f2_f3.out

#scoring system
./phmmer --popen 0.35 --pextend 0.0001 fib_simple.fa titin.fa >fib_simple_query_titin_db_open_extend.out
./phmmer --popen 0 --pextend 0 rnd_query.fa rnd_db.fa >rnd_query_rnd_db_open_extend.out
./phmmer --popen 0.25 --pextend 0.5 fibronectin1.2_1.fa titin.fa >fibronectin_1_titin_open_extend.out
