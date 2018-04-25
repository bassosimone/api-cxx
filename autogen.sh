#!/bin/sh
download=0
while [ $# -gt 0 ]; do
  if [ "$1" = "--context=ci" ]; then
    download=1
  fi
  shift
done
if [ "$download" -ne 0 ]; then
  set -ex
  virtualenv venv
  . ./venv/bin/activate
  pip install cget
  which cget
  cget install measurement-kit/measurement-kit
fi
