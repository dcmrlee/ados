#!/bin/bash

which clang-format

if [[ $? -ne 0 ]]; then
  echo "error, can not find clang-format"
  exit 1
fi

find ./src -regex '.*\.cc\|.*\.h\|.*\.proto' -and -not -regex '.*\.pb\.cc\|.*\.pb\.h' | xargs clang-format -i --style=file
echo "clang-format done"
