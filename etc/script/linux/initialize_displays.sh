#!/bin/bash

checkExistence() {
  if ! type "${1}" >/dev/null 2>/dev/null; then
    echo "${1} not found"
    exit 1
  fi
}

initializeDisplay() {
  echo "Initializing display ${1}"

  NUMBER=${1#":"}

  rm "/tmp/.X${NUMBER}-lock" -f
  rm "/tmp/.X11-unix/X${NUMBER}" -f
  rm -rf "/tmp/Xvfb_${NUMBER}"
  mkdir -p "/tmp/Xvfb_${NUMBER}"

  Xvfb "${1}" -screen 0 1920x1200x24 -fbdir "/tmp/Xvfb_${NUMBER}" &
  sleep 1
  DISPLAY=${1} startlxde >/dev/null 2>&1 &
  sleep 1
}

killDisplay() {
  echo "Stopping display ${1}"
  xvfb_pid=$(ps -C Xvfb -o pid,cmd | grep Xvfb | grep "${1}" | awk \{'print $1 '\})
  if [ -n "${xvfb_pid}" ]; then
    kill "${xvfb_pid}"
  fi
  sleep 1
}

checkDisplayVars() {
  if [ -z "${FIRST_DISPLAY}" ]; then
    echo "FIRST_DISPLAY is not set"
    exit 1
  fi
  if [ -z "${NUMBER_OF_DISPLAYS}" ]; then
    echo "${NUMBER_OF_DISPLAYS} is not set"
    exit 1
  fi
}

checkExistence "Xvfb"
checkExistence "xfce4-session"

case "$1" in
start)
  for i in $(ipcs -m | grep ugene | cut -d ' ' -f2); do ipcrm -m "${i}"; done

  if [ -z "${2}" ]; then
    checkDisplayVars
    i=$FIRST_DISPLAY
    while [ "${i}" -lt $((FIRST_DISPLAY + NUMBER_OF_DISPLAYS)) ]; do
      initializeDisplay ":${i}"
      i=$((i + 1))
    done
  else
    initializeDisplay "${2}"
  fi
  ;;

stop)
  echo "Shutting down displays"
  if [ -n "${2}" ] && [[ ${2} =~ ^:[0-9]+$ ]]; then
    killDisplay "${2}"
  else
    checkDisplayVars
    i=${FIRST_DISPLAY}
    while [ "${i}" -lt $((FIRST_DISPLAY + NUMBER_OF_DISPLAYS)) ]; do
      killDisplay ":${i}"
      i=$((i + 1))
    done
  fi
  ;;

restart)
  $0 stop
  $0 start
  ;;

status)
  netstat -an | grep tcp | grep 59
  ;;
*)
  echo "Usage: $0 {start|stop|restart}"
  exit 1
  ;;
esac
