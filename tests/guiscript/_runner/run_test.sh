#!/bin/bash

if [ ! -r $1 ] || [ ! -f $1 ]; then
  echo "Please select test list..."
  echo "Example: ./run_test.sh test.list exclude.list"
  exit 1
fi

source set_env.sh
TEST_LIST="./_runner/$1"

if ((${#2} < 1)); then
  EXCLUDE_LIST="no"
else
  if [ -r $2 ] || [ -f $2 ]; then
    EXCLUDE_LIST="./_runner/$2"
  else
    EXCLUDE_LIST="no"
  fi
fi
TST_NUM=0
FAIL_TST_NUM=0
EXCLUDED_TST_NUM=0

cd $TEST_BASE_ROOT
CUR_DIR=$(pwd)
TMP_LIST=$(mktemp list.XXXXXX)
TMP_LIST=$CUR_DIR/$TMP_LIST
function create_full_list() {
  while read test; do
    if ((${#test} > 0)); then
      if [ -d $test ]; then
        list_inc=$(ls $test)
        CUR_DIR=$(pwd)
        TEST_LIST_INC=$(mktemp list.tmp.XXXXXX)
        TEST_LIST_INC=$CUR_DIR/$TEST_LIST_INC
        for inc_test in $list_inc; do
          echo $test/$inc_test >>$TEST_LIST_INC
        done
        create_full_list <$TEST_LIST_INC
        rm list.tmp.*
      else
        if [ -e $test ] && [ -f $test ]; then
          echo $test >>$TMP_LIST
        fi
      fi
    fi
  done
}

create_full_list <$TEST_LIST
TEST_LIST=$TMP_LIST

if [ $EXCLUDE_LIST != "no" ]; then
  CUR_DIR=$(pwd)
  TMP_LIST=$(mktemp list.XXXXXX)
  TMP_LIST=$CUR_DIR/$TMP_LIST
  create_full_list <$EXCLUDE_LIST
  EXCLUDE_LIST=$TMP_LIST
fi

function testing_list() {
  while read test; do
    echo -n "    Test $test: " >>$REP_FILE
    let TST_NUM+=1
    EXCLUDE_TEST=""
    if [ $EXCLUDE_LIST != "no" ]; then
      EXCLUDE_TEST=$(grep $test <$EXCLUDE_LIST)
    fi
    if ((${#EXCLUDE_TEST} < 1)); then
      if [ $USE_LOG_RUNING == "NO" ]; then
        if
          $TESTED_PROG $test >/dev/null 2>&1
        then
          echo "pass" >>$REP_FILE
        else
          echo "fail" >>$REP_FILE
          let FAIL_TST_NUM+=1
        fi
      else
        log_dir=$(echo "$test" | cut -d '/' -f2)
        i=1
        while ([ $log_dir != "" ]); do
          let i+=1
          log_dir=$(echo "$test" | cut -d '/' -f2-$i)
          if [ ! -e _runner/_tmp/$log_dir ] && [ -d $log_dir ]; then
            mkdir _runner/_tmp/$log_dir
          fi
          if [ -f $log_dir ]; then
            break
          fi
        done
        if
          $TESTED_PROG $test >_runner/_tmp${test#.}.log 2>&1
        then
          echo "pass" >>$REP_FILE
        else
          echo "fail" >>$REP_FILE
          let FAIL_TST_NUM+=1
        fi

      fi
    else
      echo "excluded" >>$REP_FILE
      let EXCLUDED_TST_NUM+=1
    fi
  done
}

if [ $USE_LOG_RUNING != "NO" ]; then
  if [ ! -e _runner/_tmp ]; then
    mkdir _runner/_tmp
  fi
fi

echo "Start testing..."
echo "Start testing..." >$REP_FILE
testing_list <$TEST_LIST
rm $TEST_LIST
rm $EXCLUDE_LIST
echo "End testing..."
echo "End testing..." >>$REP_FILE
let PASS_TST_NUM=$TST_NUM-$FAIL_TST_NUM-$EXCLUDED_TST_NUM
echo "-------------------------------------------------------" >>$REP_FILE
echo "Tests processed: $TST_NUM" >>$REP_FILE
echo >>$REP_FILE
echo "Tests passed: $PASS_TST_NUM/$TST_NUM" >>$REP_FILE
echo "Tests failed: $FAIL_TST_NUM/$TST_NUM" >>$REP_FILE
if [ $EXCLUDE_LIST != "no" ]; then
  echo "Tests excluded: $EXCLUDED_TST_NUM/$TST_NUM" >>$REP_FILE
fi
let TESTED_NUM=$TST_NUM-$EXCLUDED_TST_NUM
if (($TESTED_NUM != 0)); then
  let SCORE=$PASS_TST_NUM*100/$TESTED_NUM
  echo "Total score: $SCORE%" >>$REP_FILE
else
  echo "Total score: 0%" >>$REP_FILE
fi
exit 0
