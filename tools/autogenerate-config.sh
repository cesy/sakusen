#!/bin/sh

# Script to check for existence of a few executables, and disable components as
# necessary.  Note that this is *not* trying to aid portability.  Reinventing
# the configure script is a task for another day...

# The shell-scripting is probably not as portable as it might be, either.

TARGET=config.mk

function check_for_prog
{
  prog="$1"
  if which "${prog}" > /dev/null 2>&1
  then
    eval ${prog}_exists=yes
  else
    eval ${prog}_exists=no
    printf "Couldn't find program '%s'\n" "${prog}"
  fi
}

function check_for_lib
{
  if test $# -lt 2
  then
    printf "Internal script error: check_for_lib got $# arguments\n"
    exit 2
  fi
  # First argument is library name
  lib="$1"
  shift
  # Second argument is header name
  header="$1"
  shift
  # Subsequent arguments are things to pass to gcc (probably includes)
  if printf '#include <%s>\nint main() { return 0; }\n' "${header}" | \
    gcc -x c++ -o /dev/null "$@" -l${lib} -
  then
    eval ${name}_exists=yes
  else
    eval ${lib}_exists=no
    printf "Missing either library '%s' or its header '%s'\n" "${lib}" "${header}"
  fi
}

function enable_component
{
  failure=0
  # First argument is component name
  component=$1
  # Subsequent arguments are programs or libraries that must exist
  while true
  do
    shift
    if test $# = 0
    then
      break
    fi

    eval prog_found=\$$1_exists

    if test "${prog_found}" = no
    then
      printf "ENABLE_%s=no\n" ${component} >> "${TARGET}"
      printf "Disabling component %s because '%s' wasn't found\n" \
        "${component}" "$1"
      failure=1
      break
    elif test "${prog_found}" != yes
    then
      printf "Internal script error: program or library '$1' has not been tested for\n" >&2
      exit 1
    fi
  done

  if test $failure = 0
  then
    printf "ENABLE_%s=yes\n" ${component} >> "${TARGET}"
    printf "Enabling component %s\n" "${component}"
  fi

  printf "ifeq (\$(ENABLE_%s),no)\n" ${component} >> "${TARGET}"
  printf '  BUILD_CFLAGS := $(BUILD_CFLAGS) -DDISABLE_%s\n' ${component} >> "${TARGET}"
  printf "endif\n\n" ${component} >> "${TARGET}"
  
  return $failure
}

printf "# This file autogenerated by %s\n" "$0" > "${TARGET}"
printf "BUILD_CFLAGS :=\n" > "${TARGET}"

check_for_prog swig
check_for_prog perl
check_for_prog mcs
#check_for_prog something_that_doesnt_exist
check_for_lib readline readline/readline.h
check_for_lib SDL SDL/SDL.h
# Would like to check for pangocairo library relly, but don't know what
# version number to append, so settle for reporting cairo, and relying on
# pkg-config to link against pangocairo.  This might make the errors slightly
# harder to understand
check_for_lib cairo pango/pangocairo.h `pkg-config pangocairo --cflags --libs`
check_for_lib unicode unicode.h
check_for_lib pcrecpp pcrecpp.h
check_for_lib mhash mhash.h
#check_for_lib something_that_doesnt_exist some_header.h

if ! enable_component EVERYTHING pcrecpp mhash
then
  printf 'Sorry, that component is critical.  Aborting.\n'
  rm "${TARGET}"
  exit 1
fi

enable_component BINDINGS swig
enable_component PERL_BINDINGS swig perl
enable_component DOTNET_BINDINGS swig mcs
enable_component UNIX_SOCKETS
enable_component READLINE readline
enable_component SDL SDL
enable_component CAIRO cairo
enable_component CONVERSION unicode
enable_component AVAHI avahiclient
#enable_component IMPOSSIBLE_COMPONENT something_that_doesnt_exist

printf 'BUILD_CXXFLAGS := $(BUILD_CFLAGS)\n' >> "${TARGET}"
printf "BUILD_ARCH := %s\n" $(uname -m) >> "${TARGET}"
