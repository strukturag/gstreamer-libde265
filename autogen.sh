#!/bin/sh

if test -d .git;
then
  # Make sure we have common
  if test ! -f common/gst-autogen.sh;
  then
    echo "+ Setting up common submodule"
    git submodule init
  fi
  git submodule update

  # install pre-commit hook for doing clean commits
  if test ! \( -x .git/hooks/pre-commit -a -L .git/hooks/pre-commit \);
  then
      rm -f .git/hooks/pre-commit
      ln -s ../../common/hooks/pre-commit.hook .git/hooks/pre-commit
  fi
fi

if [ -x "`which autoreconf 2>/dev/null`" ] ; then
   exec autoreconf -ivf
fi

LIBTOOLIZE=libtoolize
SYSNAME=`uname`
if [ "x$SYSNAME" = "xDarwin" ] ; then
  LIBTOOLIZE=glibtoolize
fi
aclocal -I m4 && \
	autoheader && \
	$LIBTOOLIZE && \
	autoconf && \
	automake --add-missing --force-missing --copy
