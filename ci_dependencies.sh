#!/usr/bin/env bash

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  READ THE ZPROJECT/README.MD FOR INFORMATION ABOUT MAKING PERMANENT CHANGES. #
################################################################################

REQUESTED_BRANCH=$1

set -e

if [ -z "$DEPENDENCIES_DIR" ]; then
    export DEPENDENCIES_DIR="`pwd`/tmp-deps"
fi
mkdir -p "$DEPENDENCIES_DIR"
cd "$DEPENDENCIES_DIR"

# Clone and build dependencies, if not yet installed to Travis env as DEBs
# or MacOS packages; other OSes are not currently supported by Travis cloud
echo "`date`: Starting build of dependencies (if any) using ci_dependencies.sh $REQUESTED_BRANCH..."

# Start of recipe for dependency: augeas
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libaugeas-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions augeas >/dev/null 2>&1) || \
       ([ -e "augeas" ]) \
; then
 echo "INFO: Get 'augeas' using apt-get libaugeas-dev" >&2
 sudo apt-get install libaugeas-dev -y || exit $?
fi


# Start of recipe for dependency: fty-common-logging
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libfty_common_logging-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions fty-common-logging >/dev/null 2>&1) || \
       ([ -e "fty-common-logging" ]) \
; then

    FOLDER_NAME="fty-common-logging"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'fty-common-logging' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/fty-common-logging.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-logging.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/fty-common-logging.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-logging.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-logging.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/fty-common-logging.git"
            echo "git clone https://github.com/42ity/fty-common-logging.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-logging.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-logging' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-logging' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'fty-common-logging' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


# Start of recipe for dependency: cxxtools
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list cxxtools-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions cxxtools >/dev/null 2>&1) || \
       ([ -e "cxxtools" ]) \
; then

    FOLDER_NAME="cxxtools"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'cxxtools' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/cxxtools.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/cxxtools.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/cxxtools.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/cxxtools.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/cxxtools.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/cxxtools.git"
            echo "git clone https://github.com/42ity/cxxtools.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/cxxtools.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'cxxtools' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'cxxtools' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'cxxtools' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


# Start of recipe for dependency: fty-common
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libfty_common-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions fty-common >/dev/null 2>&1) || \
       ([ -e "fty-common" ]) \
; then

    FOLDER_NAME="fty-common"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'fty-common' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/fty-common.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/fty-common.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/fty-common.git"
            echo "git clone https://github.com/42ity/fty-common.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'fty-common' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


# Start of recipe for dependency: fty-common-mlm
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libfty_common_mlm-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions fty-common-mlm >/dev/null 2>&1) || \
       ([ -e "fty-common-mlm" ]) \
; then

    FOLDER_NAME="fty-common-mlm"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'fty-common-mlm' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/fty-common-mlm.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-mlm.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/fty-common-mlm.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-mlm.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-mlm.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/fty-common-mlm.git"
            echo "git clone https://github.com/42ity/fty-common-mlm.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-mlm.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-mlm' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-mlm' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'fty-common-mlm' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


# Start of recipe for dependency: fty-common-messagebus
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libfty_common_messagebus-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions fty-common-messagebus >/dev/null 2>&1) || \
       ([ -e "fty-common-messagebus" ]) \
; then

    FOLDER_NAME="fty-common-messagebus"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'fty-common-messagebus' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/fty-common-messagebus.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-messagebus.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/fty-common-messagebus.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-messagebus.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-messagebus.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/fty-common-messagebus.git"
            echo "git clone https://github.com/42ity/fty-common-messagebus.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-messagebus.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-messagebus' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-messagebus' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'fty-common-messagebus' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


# Start of recipe for dependency: fty-common-dto
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libfty_common_dto-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions fty-common-dto >/dev/null 2>&1) || \
       ([ -e "fty-common-dto" ]) \
; then

    FOLDER_NAME="fty-common-dto"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'fty-common-dto' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/fty-common-dto.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-dto.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/fty-common-dto.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-dto.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/fty-common-dto.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/fty-common-dto.git"
            echo "git clone https://github.com/42ity/fty-common-dto.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/fty-common-dto.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-dto' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'fty-common-dto' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'fty-common-dto' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


# Start of recipe for dependency: protobuf
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list protobuf-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions protobuf >/dev/null 2>&1) || \
       ([ -e "protobuf" ]) \
; then

    FOLDER_NAME="protobuf"

if [ -d "$FOLDER_NAME" ]; then
    echo "$FOLDER_NAME already exist. Skipped." >&2
else
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'protobuf' from Git repository..." >&2
    if [ "x$REQUESTED_BRANCH" = "x" ]; then
        echo "git clone https://github.com/42ity/protobuf.git $FOLDER_NAME"
        $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/protobuf.git $FOLDER_NAME
    else
        if git ls-remote --heads https://github.com/42ity/protobuf.git | grep -q "$REQUESTED_BRANCH"; then
            echo "git clone -b "$REQUESTED_BRANCH" https://github.com/42ity/protobuf.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 -b "$REQUESTED_BRANCH" https://github.com/42ity/protobuf.git $FOLDER_NAME
        else
            echo "$REQUESTED_BRANCH not found for https://github.com/42ity/protobuf.git"
            echo "git clone https://github.com/42ity/protobuf.git $FOLDER_NAME"
            $CI_TIME git clone --quiet --depth 1 https://github.com/42ity/protobuf.git $FOLDER_NAME
        fi
    fi
    echo "Entering in ${PWD}/${FOLDER_NAME}"
    cd "./${FOLDER_NAME}"
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
        git --no-pager log --oneline -n1
    if [ -e ci_dependencies.sh ]; then
        PROPAGATED_BRANCH="`git branch | grep * | cut -d ' ' -f2`"
        DEFAULT_BRANCH="`git symbolic-ref refs/remotes/origin/HEAD | sed 's@^refs/remotes/origin/@@'`"
        if [ "x$REQUESTED_BRANCH" = "x" ]; then
            echo "`date`: INFO: Building prerequisites of 'protobuf' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        elif [ "x$PROPAGATED_BRANCH" = "x$DEFAULT_BRANCH" ]; then
            echo "`date`: INFO: Building prerequisites of 'protobuf' using ci_dependencies.sh the default branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh)
        else
            echo "`date`: INFO: Building prerequisites of 'protobuf' using ci_dependencies.sh $PROPAGATED_BRANCH branch..." >&2
            ($CI_TIME source ./ci_dependencies.sh $PROPAGATED_BRANCH)
        fi
    fi
    if [ -e autogen.sh ]; then
        $CI_TIME ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        $CI_TIME ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        $CI_TIME libtoolize --copy --force && \
        $CI_TIME aclocal -I . && \
        $CI_TIME autoheader && \
        $CI_TIME automake --add-missing --copy && \
        $CI_TIME autoconf || \
        $CI_TIME autoreconf -fiv
    fi
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j4
    $CI_TIME make install
    echo "Leaving from ${PWD}"
    cd "${BASE_PWD}"
    echo "Now in ${PWD}"
fi
fi


