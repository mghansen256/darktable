include(CheckCCompilerFlag)

macro (CHECK_C_COMPILER_FLAG_AND_ENABLE_IT _FLAG)
  set(_RESULT "C_COMPILER_UNDERSTANDS_${_FLAG}")

  set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS}")

  CHECK_C_COMPILER_FLAG("${_FLAG}" ${_RESULT})

  if(${${_RESULT}})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_FLAG}")
  endif()
endmacro ()
