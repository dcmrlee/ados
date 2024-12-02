# Copyright (C) 2024. All rights reserved.

macro(add_werror target)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${target}
      PRIVATE -Wall
              -Wextra
              -Wno-unused-parameter
              -Wno-missing-field-initializers
              -Wno-implicit-fallthrough
              -Wno-stringop-overflow
              -Werror)
  endif()
endmacro()
