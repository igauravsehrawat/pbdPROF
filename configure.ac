dnl This file was original for Rmpi written by Dr. Yu Hao, and may be
dnl modified by Prof. Brian Rieply and suggested by R Core Team.
dnl Later, it was added by Wei-Chen Chen for pbdMPI and pbdPROF for more
dnl functionalities combining with Makevars.in.


dnl Process this file with autoconf to produce a configure script
AC_INIT(DESCRIPTION)

dnl Determine R_HOME.
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
R_SCMD="${R_HOME}/bin/Rscript -e"


dnl All AC_ARG_WITH
AC_ARG_WITH([mpi-include],
            AC_HELP_STRING([--with-mpi-include=INCLUDE_PATH],
                           [location of MPI header files]),
            [mpi_include_path=$withval])
AC_ARG_WITH([mpi-libpath],
            AC_HELP_STRING([--with-mpi-libpath=LIB_PATH],
                           [location of MPI library files]),
            [mpi_lib_path=$withval])
AC_ARG_WITH([mpi-type],
            AC_HELP_STRING([--with-mpi-type=MPI_TYPE],
                           [type of MPI: OPENMPI, MPICH2, MPICH3, or INTELMPI]),
            [mpi_type=$withval])

dnl All AC_ARG_ENABLE
AC_ARG_ENABLE(opa,
  AC_HELP_STRING([--enable-opa], [Enable opa for MPICH*, default yes.]),
  [ENABLE_OPA="no"], [ENABLE_OPA="yes"])


dnl One way is to specify both include and lib paths
if test -n "${mpi_include_path}" ; then
  MPI_INCLUDE_PATH="${mpi_include_path}"
  if test ! -f "${MPI_INCLUDE_PATH}/mpi.h" ; then
    AC_MSG_ERROR([Value of --with-mpi-include does not contain mpi.h])
  fi
else
  if test -n "${MPI_INCLUDE}" ; then
    MPI_INCLUDE_PATH="${MPI_INCLUDE}"
    if test ! -f "${MPI_INCLUDE_PATH}/mpi.h" ; then
      AC_MSG_ERROR([Value of MPI_INCLUDE does not contain mpi.h])
    fi
    echo "setting mpi include path from MPI_INCLUDE"
  fi
fi

if test -n "${mpi_lib_path}" ; then
  MPI_LIBPATH="${mpi_lib_path}"
else
  if test -n "${MPI_LIB_PATH}" ; then
    MPI_LIBPATH="${MPI_LIB_PATH}"
    echo "setting mpi lib path from MPI_LIB_PATH"
  fi
fi

if test -n "${MPI_INCLUDE_PATH}" -a -z "${MPI_LIBPATH}" ; then
  AC_MSG_ERROR([Must specify both the include and lib paths])
fi
if test -z "${MPI_INCLUDE_PATH}" -a -n "${MPI_LIBPATH}" ; then
  AC_MSG_ERROR([Must specify both the include and lib paths])
fi

MPITYPE="UNKNOWN"

if test -n "${mpi_type}" ; then
  MPITYPE="${mpi_type}"
else
  if test -n "${MPI_TYPE}" ; then
    MPITYPE="${MPI_TYPE}"
  fi
fi


dnl Check compiler.
CC=`"${R_HOME}/bin/R" CMD config CC`
AC_PROG_CC
if test "X$CC" = "X" ; then
  # Our best guess...
  CC="gcc -std=gnu99"
fi


dnl Check MPI in path.
AC_CHECK_PROG(SED, sed, sed, "F")
AC_CHECK_PROG(MPICC, mpicc, mpicc, "F")
AC_CHECK_PROG(OMPI_INFO, ompi_info, ompi_info, "F")
dnl AC_CHECK_PROG(MPICH2VERSION, mpich2version, mpich2version, "F")


dnl For OpenMPI
if test -z "${MPI_INCLUDE_PATH}" ; then
  if test "$MPITYPE" = "UNKNOWN" -o "$MPITYPE" = "OPENMPI" ; then
    if test "$SED" != "F" -a "$MPICC" != "F" -a "$OMPI_INFO" != "F" ; then
      echo "found sed, mpicc, and ompi_info ..."
      TMP_INC="F"
      TMP_LIB="F"

      TMP_INC_DIRS=`mpicc --showme:incdirs`
      echo ">> TMP_INC_DIRS = ${TMP_INC_DIRS}"

      TMP_IFS="${IFS}"; IFS=" "
      for dir in ${TMP_INC_DIRS}; do
        echo "checking ${dir} ..."
        if test -f "${dir}/mpi.h" ; then
          echo "found ${dir}/mpi.h ..."
          TMP_INC=${dir}
          break
        fi
      done
      IFS="${TMP_IFS}"

      TMP_LIB_DIRS=`mpicc --showme:libdirs`
      echo ">> TMP_LIB_DIRS = ${TMP_LIB_DIRS}"

      TMP_IFS="${IFS}"; IFS=" "
      for dir in ${TMP_LIB_DIRS}; do
        echo "checking ${dir} ..." 
        if test -f "${dir}/libmpi.so" ; then
          echo "found ${dir}/libmpi.so ..." 
          TMP_LIB=${dir}
          break
        fi
        dnl For Mac OS X
        if test -f "${dir}/libmpi.dylib" ; then
          echo "found ${dir}/libmpi.dylib ..."
          TMP_LIB=${dir}
          break
        fi
      done
      IFS="${TMP_IFS}"

      if test "${TMP_INC}" != "F" -a "${TMP_LIB}" != "F" ; then
        echo "found mpi.h and libmpi.so ..."
        echo ">> TMP_INC = ${TMP_INC}"
        echo ">> TMP_LIB = ${TMP_LIB}"
        MPITYPE="OPENMPI"
        MPI_INCLUDE_PATH="${TMP_INC}"
        MPI_LIBPATH="${TMP_LIB}"
      fi
    fi
  fi
fi


dnl For MPICH3/MPICH2/MPICH
if test -z "${MPI_INCLUDE_PATH}" ; then
### MPICH2 users have to specify --with-mpi-type=MPICH2 to get this.
  if test "$MPITYPE" = "MPICH2" -o "$MPITYPE" = "MPICH" -o "$MPITYPE" = "MPICH3" ; then
    if test "$SED" != "F" -a "$MPICC" != "F" ; then
      echo "found sed and mpicc ..."
      TMP_INC="F"
      TMP_LIB="F"

      TMP_CMD=`mpicc -show`
      echo ">> TMP_CMD = ${TMP_CMD}"

      dnl check "mpi.h".
      TMP_IFS="${IFS}"; IFS=" "
      for dir in ${TMP_CMD}; do
        dir_inc=`echo ${dir} | sed -e "s/^-I\(.*\)$/\1/"`

        dnl use a different way to test.
        if test "X${dir_inc}" = "X" ; then
          dir_inc=`echo ${dir} | sed -e "s/-I\(.*\)/\1/"`
        fi

        echo "checking ${dir_inc} ..."
        if test -f "${dir_inc}/mpi.h" ; then
          echo "found ${dir_inc}/mpi.h ..."
          TMP_INC=${dir_inc}
          break
        fi
      done
      IFS="${TMP_IFS}"

      dnl check library.
      TMP_IFS="${IFS}"; IFS=" "
      for dir in ${TMP_CMD}; do
        dir_lib=`echo ${dir} | sed -e "s/^-L\(.*\)$/\1/"`

        dnl use a different way to test.
        if test "X${dir_lib}" = "X" ; then
          dir_lib=`echo ${dir} | sed -e "s/-L\(.*\)/\1/"`
        fi

        echo "checking ${dir_lib} ..." 
        if test -f "${dir_lib}/libmpich.so" ; then
          echo "found ${dir_lib}/libmpich.so ..." 
          TMP_LIB=${dir_lib}
          break
        fi
        dnl For Mac OS X
        if test -f "${dir_lib}/libmpich.dylib" ; then
          echo "found ${dir_lib}/libmpich.dylib ..."
          TMP_LIB=${dir_lib}
          break
        fi
      done
      IFS="${TMP_IFS}"

      if test "${TMP_INC}" != "F" -a "${TMP_LIB}" != "F" ; then
        echo "found mpi.h and libmpich.so ..."
        echo ">> TMP_INC = ${TMP_INC}"
        echo ">> TMP_LIB = ${TMP_LIB}"
        MPITYPE="${MPITYPE}"
        MPI_INCLUDE_PATH="${TMP_INC}"
        MPI_LIBPATH="${TMP_LIB}"
      fi
    fi
  fi
fi


dnl For nothing found from above
dnl 'Paths not specified'
if test -z "${MPI_INCLUDE_PATH}" ; then
  dnl Another way is to specify MPI_ROOT, perhaps via --with-mpi
  AC_ARG_WITH(mpi,
              AC_HELP_STRING([--with-mpi=MPI_ROOT],
                             [location of top-level MPI directory]),
              [ if test "${withval}" != "no"; then
                      MPI_ROOT=${withval}
                      MPI_INCLUDE_PATH="${MPI_ROOT}/include"
                      LDFLAGS="$LDFLAGS -L${withval}/lib"
                      CPPFLAGS="$CPPFLAGS -I${withval}/include"
                fi ]
  )

  dnl Detect from conventional paths
  if test -z "$MPI_ROOT" ; then
    for d in /opt/SUNWhpc /opt/SUNWhpc/HPC \
             /opt/SUNWhpc/HPC/sun /opt/SUNWhpc/HPC8.2.1c/sun \
             /opt/SUNWhpc/HPC/gnu /opt/SUNWhpc/HPC8.2.1c/gnu \
             /opt/lib /usr/lib /usr /usr/local/lib /usr/local \
             /opt/lib64 /usr/lib64 /usr/local/lib64 \
             /usr/lib/mpi/gcc/openmpi \
             /usr/lib64/mpi/gcc/openmpi \
             /usr/lib/mpi/gcc/ompi \
             /usr/lib64/mpi/gcc/ompi \
             /usr/local/mpi/openmpi ; do
      if test -f $d/include/openmpi-i386/mpi.h && test -d $d/lib/openmpi/lib; then
      dnl This is what Fedora 17 had
        echo "I am here $d and it is openmpi"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/openmpi-x86_64/mpi.h && test -d $d/lib/openmpi/lib; then
      dnl This is what Fedora 17 had
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/openmpi-x86_64/mpi.h && test -d $d/lib64/openmpi/lib; then
      dnl This is what Fedora 17 had
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/mpi.h && test -d $d/lib/openmpi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/mpi.h && test -d $d/lib64/openmpi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/openmpi/mpi.h && test -d $d/lib/openmpi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/openmpi/mpi.h && test -d $d/lib64/openmpi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/mpi.h && test -d $d/lib/ompi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/mpi.h && test -d $d/lib64/ompi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/ompi/mpi.h && test -d $d/lib/ompi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/include/ompi/mpi.h && test -d $d/lib64/ompi; then
        echo "I am here $d and it is OpenMPI"
        MPI_ROOT=$d
        MPITYPE="OPENMPI"
        break
      elif test -f $d/openmpi/include/mpi.h && test -d $d/openmpi/lib; then
        echo "I am here $d/openmpi and it is OpenMPI"
        MPI_ROOT=$d/openmpi
        MPITYPE="OPENMPI"
        break
      elif test -f $d/ompi/include/mpi.h && test -d $d/ompi/lib; then
        echo "I am here $d/ompi and it is OpenMPI"
        MPI_ROOT=$d/ompi
        MPITYPE="OPENMPI"
        break
      elif test -f $d/lam/include/mpi.h && test -d $d/lam/lib; then
        echo "I am here $d/lam and it is LAM"
        MPI_ROOT=$d/lam
        MPITYPE="LAM"
        break
      elif test -f $d/include/lam/mpi.h && test -d $d/lib/lam; then
        echo "I am here $d and it is LAM"
        MPI_ROOT=$d
        MPITYPE="LAM"
        break
      elif test -f $d/include/lam/mpi.h && test -d $d/lib64/lam; then
        echo "I am here $d and it is LAM"
        MPI_ROOT=$d
        MPITYPE="LAM"
        break
      elif test -f $d/mpich3/include/mpi.h; then
        echo "I am here $d/mpich3 and it is MPICH3"
        MPI_ROOT=$d/mpich3
        MPITYPE="MPICH3"
        break
      elif test -f $d/mpich2/include/mpi.h; then
        echo "I am here $d/mpich2 and it is MPICH2"
        MPI_ROOT=$d/mpich2
        MPITYPE="MPICH2"
        break
      elif test -f $d/mpich/include/mpi.h; then
        echo "I am here $d/mpich and it is MPICH"
        MPI_ROOT=$d/mpich
        MPITYPE="MPICH"
        break
      elif test -f $d/mpi/include/mpi.h; then
        echo "I am here $d/mpi"
        MPI_ROOT=$d/mpi
        break
      elif test -f $d/include/mpi.h; then
        echo "I am here $d"
        MPI_ROOT=$d
        break
      fi
    done
  fi


  if test -n "$MPI_ROOT"; then
    dnl Guess MPITYPE if it is unknown and user supplies MPI_ROOT
    if test $MPITYPE = "UNKNOWN" ; then
      if test -d ${MPI_ROOT}/openmpi; then
        MPITYPE="OPENMPI"
      elif test -d ${MPI_ROOT}/ompi; then
        MPITYPE="OPENMPI"
      elif test -d ${MPI_ROOT}/lib/openmpi; then
        MPITYPE="OPENMPI"
      elif test -d ${MPI_ROOT}/lib64/openmpi; then
        MPITYPE="OPENMPI"
      elif test -d ${MPI_ROOT}/lib/ompi; then
        MPITYPE="OPENMPI"
      elif test -d ${MPI_ROOT}/lib64/ompi; then
        MPITYPE="OPENMPI"
      elif test -f ${MPI_ROOT}/lib/liblam.so; then
        MPITYPE="LAM"
      elif test -f ${MPI_ROOT}/lib64/liblam.so; then
        MPITYPE="LAM"
      elif test -d ${MPI_ROOT}/lib/lam; then
        MPITYPE="LAM"
      elif test -d ${MPI_ROOT}/lib64/lam; then
        MPITYPE="LAM"
      elif test -f ${MPI_ROOT}/lib/libmpich.a; then
        MPITYPE="MPICH2"
      elif test -f ${MPI_ROOT}/lib64/libmpich.a; then
        MPITYPE="MPICH2"
      elif test -f ${MPI_ROOT}/lib/libmpich.so; then
        MPITYPE="MPICH2"
      elif test -f ${MPI_ROOT}/lib64/libmpich.so; then
        MPITYPE="MPICH2"
      elif test -f ${MPI_ROOT}/lib/libmpi.so; then
        dnl Nautilus needs this
        MPITYPE="MPT"
      fi
    fi


    echo "Trying to find mpi.h ..."
    if test -f ${MPI_ROOT}/include/openmpi-i386/mpi.h; then
      dnl This is what Fedora 17 had
      echo "Found in ${MPI_ROOT}/include/openmpi-i386"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/openmpi-i386"
    elif test -f ${MPI_ROOT}/include/openmpi-x86_64/mpi.h; then
      dnl This is what Fedora 17 had
      echo "Found in ${MPI_ROOT}/include/openmpi-x86_64"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/openmpi-x86_64"
    elif test -f ${MPI_ROOT}/include/openmpi/mpi.h; then
      echo "Found in ${MPI_ROOT}/include/openmpi"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/openmpi"
    elif test -f ${MPI_ROOT}/include/ompi/mpi.h; then
      echo "Found in ${MPI_ROOT}/include/ompi"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/ompi"
    elif test -f ${MPI_ROOT}/include/lam/mpi.h; then
      echo "Found in ${MPI_ROOT}/include/lam"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/lam"
    elif test -f ${MPI_ROOT}/include/mpich2/mpi.h; then
      echo "Found in ${MPI_ROOT}/include/mpich2"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/mpich2"
    elif test -f ${MPI_ROOT}/include/mpich3/mpi.h; then
      echo "Found in ${MPI_ROOT}/include/mpich3"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/mpich3"
    elif test -f ${MPI_ROOT}/include/mpich/mpi.h; then
      echo "Found in ${MPI_ROOT}/include/mpich"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include/mpich"
    elif test -f ${MPI_ROOT}/include/mpi.h; then
      echo "Found in ${MPI_ROOT}/include"
      MPI_INCLUDE_PATH="${MPI_ROOT}/include"		  
    else 
      echo "Cannot find mpi.h header file"
      echo "Please use --with-mpi=/path/to/mpi or specify the environment variable MPI_ROOT"
      exit 1
    fi
  else
    AC_CHECK_HEADER(mpi.h,, AC_MSG_ERROR(["Cannot find mpi.h header file"]))
  fi 


  echo "Trying to find libmpi.so or libmpich.a ..."
  if test -f ${MPI_ROOT}/libmpi.so; then
    dnl This is what Fedora 10 had
    echo "Found libmpi in ${MPI_ROOT}"
    MPI_LIBPATH="${MPI_ROOT}"
  elif test -f ${MPI_ROOT}/lib/openmpi/lib/libmpi.so; then
    dnl This is what Fedora 17 had
    echo "Found libmpi in ${MPI_ROOT}/lib/openmpi/lib"
    MPI_LIBPATH="${MPI_ROOT}/lib/openmpi/lib"
  elif test -f ${MPI_ROOT}/lib64/openmpi/lib/libmpi.so; then
    dnl This is what Fedora 17 had
    echo "Found libmpi in ${MPI_ROOT}/lib64/openmpi/lib"
    MPI_LIBPATH="${MPI_ROOT}/lib64/openmpi/lib"
  elif test -f ${MPI_ROOT}/lib/openmpi/libmpi.so; then
    echo "Found libmpi in ${MPI_ROOT}/lib/openmpi"
    MPI_LIBPATH="${MPI_ROOT}/lib/openmpi"
  elif test -f ${MPI_ROOT}/lib/openmpi/libmpi.dylib; then
    echo "Found libmpi in ${MPI_ROOT}/lib/openmpi"
    MPI_LIBPATH="${MPI_ROOT}/lib/openmpi"
  elif test -f ${MPI_ROOT}/lib64/openmpi/libmpi.so; then
    echo "Found libmpi in ${MPI_ROOT}/lib64/openmpi"
    MPI_LIBPATH="${MPI_ROOT}/lib64/openmpi"
  elif test -f ${MPI_ROOT}/lib64/openmpi/libmpi.dylib; then
    echo "Found libmpi in ${MPI_ROOT}/lib64/openmpi"
    MPI_LIBPATH="${MPI_ROOT}/lib64/openmpi"
  elif test -f ${MPI_ROOT}/lib/libmpi.so; then
    echo "Found libmpi in ${MPI_ROOT}/lib"
    MPI_LIBPATH="${MPI_ROOT}/lib"
  elif test -f ${MPI_ROOT}/lib/libmpi.dylib; then
    echo "Found libmpi in ${MPI_ROOT}/lib"
    MPI_LIBPATH="${MPI_ROOT}/lib"
  elif test -f ${MPI_ROOT}/lib64/libmpi.so; then
    echo "Found libmpi in ${MPI_ROOT}/lib64"
    MPI_LIBPATH="${MPI_ROOT}/lib64"		
  elif test -f ${MPI_ROOT}/lib64/libmpi.dylib; then
    echo "Found libmpi in ${MPI_ROOT}/lib64"
    MPI_LIBPATH="${MPI_ROOT}/lib64"		
  elif test -f ${MPI_ROOT}/lib/lam/libmpi.so; then
    echo "Found libmpi in ${MPI_ROOT}/lib/lam"
    MPI_LIBPATH="${MPI_ROOT}/lib/lam"
    MPI_INCL2="-I${MPI_INCLUDE_PATH}/32"
  elif test -f ${MPI_ROOT}/lib64/lam/libmpi.so; then
    echo "Found libmpi in ${MPI_ROOT}/lib64/lam"
    MPI_LIBPATH="${MPI_ROOT}/lib64/lam"
    MPI_INCL2="-I${MPI_INCLUDE_PATH}/64"
  elif test -f ${MPI_ROOT}/lib/libmpich.so; then
    echo "Found libmpich in ${MPI_ROOT}/lib"
    MPI_LIBPATH="${MPI_ROOT}/lib"
    AC_CHECK_DECLS([MPICH2, MPICH2],[  MPI_DEFS="-DMPI2" ],, 
                   [#include <mpi.h>])
  elif test -f ${MPI_ROOT}/lib64/libmpich.so; then
    echo "Found libmpich in ${MPI_ROOT}/lib64"
    MPI_LIBPATH="${MPI_ROOT}/lib64"
    AC_CHECK_DECLS([MPICH2, MPICH2],[  MPI_DEFS="-DMPI2" ],, 
                   [#include <mpi.h>])
  elif test -f ${MPI_ROOT}/lib/libmpich.a; then
    echo "Found libmpich in ${MPI_ROOT}/lib"
    MPI_LIBPATH="${MPI_ROOT}/lib"
    AC_CHECK_DECLS([MPICH2, MPICH2],[  MPI_DEFS="-DMPI2" ],, 
                   [#include <mpi.h>])
  elif test -f ${MPI_ROOT}/lib64/libmpich.a; then
    echo "Found libmpich in ${MPI_ROOT}/lib64"
    MPI_LIBPATH="${MPI_ROOT}/lib64"
    AC_CHECK_DECLS([MPICH2, MPICH2],[  MPI_DEFS="-DMPI2" ],, 
                   [#include <mpi.h>])
  else
    AC_CHECK_LIB(mpi, main, MPI_LIBPATH="${MPI_ROOT}/lib",
                 [ echo "libmpi not found. exiting..."
                   exit 1 ])
  fi


  if test "$MPITYPE" = "LAM" ; then
    echo "Try to find liblam.so ..."
    if test -f ${MPI_ROOT}/lib/liblam.so; then
      echo "Found liblam in ${MPI_ROOT}/lib"
      MPI_LIBPATH="${MPI_ROOT}/lib"
    elif test -f ${MPI_ROOT}/lib64/liblam.so; then
      echo "Found liblam in ${MPI_ROOT}/lib64"
      MPI_LIBPATH="${MPI_ROOT}/lib64"
    elif test -f ${MPI_ROOT}/lib/lam/liblam.so; then
      echo "Found liblam in ${MPI_ROOT}/lib/lam"
      MPI_LIBPATH="${MPI_ROOT}/lib/lam"
    elif test -f ${MPI_ROOT}/lib64/lam/liblam.so; then
      echo "Found liblam in ${MPI_ROOT}/lib64/lam"
      MPI_LIBPATH="${MPI_ROOT}/lib64/lam"
    else
      AC_CHECK_LIB(lam, main, [ echo "liblam not found. Probably not LAM-MPI" ])
    fi    
  fi

fi dnl Dnd of 'Paths not specified'




dnl Now we have found the include and lib paths and may know the type

AC_CHECK_LIB(util, openpty, [ MPI_LIBS="$MPI_LIBS -lutil" ])
AC_CHECK_LIB(pthread, main, [ MPI_LIBS="$MPI_LIBS -lpthread" ])

if test "$MPITYPE" = "OPENMPI" -o "$MPITYPE" = "LAM" -o "$MPITYPE" = "MPICH2" -o "$MPITYPE" = "MPICH3" ; then
  MPI_DEFS="-DMPI2"
fi

PKG_CPPFLAGS="-I${MPI_INCLUDE_PATH} ${MPI_INCL2} ${MPI_DEFS} -D${MPITYPE}"
case "$MPITYPE" in
  OPENMPI)
    PKG_LIBS="-L${MPI_LIBPATH} -lmpi ${MPI_LIBS}"
    ;;
  LAM)
    PKG_LIBS="-L${MPI_LIBPATH} -lmpi -llam ${MPI_LIBS}"
    ;;
  MPICH3)
    if test "X$ENABLE_OPA" = "Xyes" ; then
      PKG_LIBS="-L${MPI_LIBPATH} -lmpich -lmpl -lopa ${MPI_LIBS}"
    else
      PKG_LIBS="-L${MPI_LIBPATH} -lmpich -lmpl ${MPI_LIBS}"
    fi
    ;;
  MPICH2)
    if test "X$ENABLE_OPA" = "Xyes" ; then
      PKG_LIBS="-L${MPI_LIBPATH} -lmpich -lmpl -lopa ${MPI_LIBS}"
    else
      PKG_LIBS="-L${MPI_LIBPATH} -lmpich -lmpl ${MPI_LIBS}"
    fi
    ;;
  MPICH)
    if test "X$ENABLE_OPA" = "Xyes" ; then
      PKG_LIBS="-L${MPI_LIBPATH} -lmpich -lmpl -lopa ${MPI_LIBS}"
    else
      PKG_LIBS="-L${MPI_LIBPATH} -lmpich -lmpl ${MPI_LIBS}"
    fi
    ;;
  INTELMPI)
    PKG_LIBS="-L${MPI_LIBPATH} -lmpigf -lmpi -lmpigi ${MPI_LIBS}"
    ;;
  *)
    dnl suppose MPI_LDFLAGS is provided
    if test "X${MPI_LDFLAGS}" != "X" ; then
      PKG_LIBS="${MPI_LDFLAGS} ${MPI_LIBS}"
    else
      AC_MSG_ERROR([Unknown type of mpi: use --with-mpi-type or MPI_TYPE to specify it])
    fi
    ;;
esac


dnl Echo all flags to see if they are set properly
if test "$MPIRUN" = "F" -a "$MPIEXEC" = "F" -a "$ORTERUN" = "F" ; then
  echo ">>>> MPI executable is not in PATH ..."
  echo ">>>> Please export or setenv PATH ..."
fi


dnl Check profiler.
AC_ARG_WITH([fpmpi],
            AC_HELP_STRING([--with-fpmpi="-L/..../fpmpi/lib -lfpmpi"],
                           [location and name of fpmpi library]),
            [fpmpi_ext_link=$withval])
AC_ARG_WITH([mpiP],
            AC_HELP_STRING([--with-mpiP="-L/..../mpiP/lib -lmpiP"],
                           [location and name of mpiP library]),
            [mpiP_ext_link=$withval])
AC_ARG_WITH([TAU],
            AC_HELP_STRING([--with-TAU="-L/..../TAU/lib -lTAU"],
                           [location and name of TAU library]),
            [TAU_ext_link=$withval])
N_PROF=0
PROF_TYPE="fpmpi"
if test -n "${fpmpi_ext_link}"; then
  N_PROF=`expr $N_PROF + 1`
  PROF_TYPE="fpmpi"
fi
if test -n "${mpiP_ext_link}"; then
  N_PROF=`expr $N_PROF + 1`
  PROF_TYPE="mpiP"
fi
if test -n "${TAU_ext_link}"; then
  N_PROF=`expr $N_PROF + 1`
  PROF_TYPE="TAU"
fi
if test ${N_PROF} -gt 1; then
  AC_MSG_ERROR([Only one of fpmpi, mpiP, or TAU should be used.])
fi


dnl Initial
FPMPI_EXT_LINK=""
FPMPI_INTERNAL=""
FPMPI_OBJS=""
INT32=""
R_FPMPI=""
MPIP_EXT_LINK=""
TAU_EXT_LINK=""


dnl fpmpi
if test "X${PROF_TYPE}" = "Xfpmpi"; then
  dnl Incorporate external fpmpi.
  FPMPI_EXT_LINK=""
  FPMPI_INTERNAL="yes"
  FPMPI_OBJS=""
  INT32=""
  R_FPMPI=""
  if test -n "${fpmpi_ext_link}"; then
    FPMPI_EXT_LINK="${fpmpi_ext_link}"
    FPMPI_INTERNAL="no"
  else
    FPMPI_INTERNAL="yes"
  fi


  dnl Incorporate internal fpmpi, partly adopted from fpmpi's "configure.in".
  if test "X${FPMPI_INTERNAL}" = "Xyes"; then
    FPMPI_OBJS="\$(FPMPI_OBJS)"
    INT32=""
    R_FPMPI="libfpmpi.a"
    AC_CONFIG_HEADER(src/fpmpi/fpmpiconf.h)

    AC_ARG_WITH(max_procs,
                AC_HELP_STRING([--with-max_procs],
                               [define the maximum number of processes for fpmpi]),
                max_procs_value=$withval,max_procs_value=512)
    # As a convenience, we could change a "k" in the value to *1024.
    AC_DEFINE_UNQUOTED(MAX_PROCS,$max_procs_value,[maximum number of processes])
    max_procsby32=`expr \( $max_procs_value + 31 \) / 32`
    AC_DEFINE_UNQUOTED(MAX_PROCBY32,$max_procsby32,[maximum number of processes,
                       divided by 32 (for use with int32 bit vectors)])

    FPMPI2_VERSION='"'2.2'"'
    AC_DEFINE_UNQUOTED(FPMPI2_VERSION,$FPMPI2_VERSION,
                       [Provide the version for FPMPI 2])

    # See if unsigned ints are 32-bit
    AC_LANG_C
    if test -z "$CROSS_SIZEOF_UINT" ; then
      CROSS_SIZEOF_UINT=0
    fi
    AC_CACHE_CHECK([for int32_t],pac_cv_have_int32_t,[
    AC_TRY_COMPILE([
    #include <stdlib.h>
    #include <sys/types.h>],
    [int32_t a=0;],pac_cv_have_int32_t=yes,pac_cv_have_int32_t=no)])

    if test "$pac_cv_have_int32_t" = yes ; then
      AC_DEFINE(HAVE_INT32_T,1,[Define if int32_t is available])
    else
      AC_CHECK_SIZEOF(unsigned int,$CROSS_SIZEOF_UINT)
      if test "X$ac_cv_sizeof_unsigned_int" = "X4" ; then
          INT32="unsigned int"
      else
          INT32=""
          AC_MSG_WARN($ac_t You must define int32 in profiler.h manually!)
      fi
      AC_SUBST(INT32)
    fi
    AC_CHECK_SIZEOF(long long)
    AC_CHECK_SIZEOF(int)
    AC_CHECK_SIZEOF(MPI_Status,,[#include "mpi.h"])
    SIZEOF_MPI_STATUS=$ac_cv_sizeof_MPI_Status
    AC_DEFINE_UNQUOTED(SIZEOF_MPI_STATUS,$SIZEOF_MPI_STATUS,
                       [Define the size of MPI_Status])
    if test "$ac_cv_sizeof_int" != "0" ; then
      SIZEOF_MPI_STATUS_IN_INTS=`expr $SIZEOF_MPI_STATUS / $ac_cv_sizeof_int`
    else
      # Pick a likely default (the actual code will check this)
      SIZEOF_MPI_STATUS_IN_INTS=5
    fi
    AC_DEFINE_UNQUOTED(SIZEOF_MPI_STATUS_IN_INTS,$SIZEOF_MPI_STATUS_IN_INTS,
                       [Define the size of MPI_Status in ints])

    AC_CHECK_HEADERS([stdlib.h],,, [
      #ifdef HAVE_STDLIB_H
      #include <stdlib.h>
      #endif
      ])
    AC_CHECK_HEADERS([unistd.h],,, [
      #ifdef HAVE_UNISTD_H
      #include <unistd.h>
      #endif
      ])
    AC_CHECK_HEADERS([sys/time.h],,, [
      #ifdef HAVE_SYS_TIME_H
      #include <sys/time.h>
      #endif
      ])
    AC_CHECK_HEADERS([sys/resource.h],,, [
      #ifdef HAVE_SYS_RESOURCE_H
      #include <sys/resource.h>
      #endif
      ])
    AC_TRY_LINK([#ifdef HAVE_UNISTD_H
      #include <unistd.h>
      #endif
      #ifdef HAVE_SYS_RESOURCE_H
      #include <sys/resource.h>
      #endif],
      [getrusage(RUSAGE_SELF,NULL);],
      pac_cv_has_getrusage=yes,pac_cv_has_getrusage=no)
    if test "$pac_cv_has_getrusage" = "yes" ; then
      AC_DEFINE(HAVE_GETRUSAGE,1,[Define for getrusage() availability])
      # Now, test that the fields are available (they may not be if strrict POSIX
      # compatibility is set in the compiler) 
      AC_CACHE_CHECK([whether minflt field of struct rusage is available],
                      pac_cv_have_rusage_minflt,[
        AC_TRY_COMPILE([
          #include <unistd.h>
          #include <resource.h>],[struct rusage r; r.ru_minflt=0; r.ru_majflt=0;],
          pac_cv_have_rusage_minflt=yes,pac_cv_have_rusage_minflt=no)])
      if test "$pac_cv_have_rusage_minflt" = "yes" ; then
        AC_DEFINE(GETRUSAGE_HAS_MINFLT,1,
                  [Define if minflt field for getrusage is available])
      fi
    fi

    AC_OUTPUT(src/fpmpi/profiler.h)
  fi
fi
dnl End of fpmpi.


dnl mpiP
if test "X${PROF_TYPE}" = "XmpiP"; then
  dnl Incorporate external mpiP.
  if test -n "${mpiP_ext_link}"; then
    MPIP_EXT_LINK="${mpiP_ext_link}"
  else
    AC_MSG_ERROR([--with-mpiP="-L/..../mpiP/lib -lmpiP"])
  fi
fi


dnl TAU
if test "X${PROF_TYPE}" = "XTAU"; then
  if test -n "${TAU_ext_link}"; then
    TAU_EXT_LINK="${TAU_ext_link}"
  else
    AC_MSG_ERROR([--with-TAU="-L/..../TAU/lib -lTAU"])
  fi
fi


dnl Report
echo " "
echo "******************* Results of pbdPROF package configure *****************"
echo " "
echo ">> TMP_INC = ${TMP_INC}"
echo ">> TMP_LIB = ${TMP_LIB}"
echo ">> MPI_ROOT = ${MPI_ROOT}"
echo ">> MPITYPE = ${MPITYPE}"
echo ">> MPI_INCLUDE_PATH = ${MPI_INCLUDE_PATH}"
echo ">> MPI_LIBPATH = ${MPI_LIBPATH}"
echo ">> MPI_LIBS = ${MPI_LIBS}"
echo ">> MPI_DEFS = ${MPI_DEFS}"
echo ">> MPI_INCL2 = ${MPI_INCL2}"
echo ">> MPI_LDFLAGS = ${MPI_LDFLAGS}"
echo ">> PKG_CPPFLAGS = ${PKG_CPPFLAGS}"
echo ">> PKG_LIBS = ${PKG_LIBS}"
echo " "
echo "******************* Results of profiler configure ***********************"
echo " "
echo ">> FPMPI_EXT_LINK = ${FPMPI_EXT_LINK}"
echo ">> FPMPI_INTERNAL = ${FPMPI_INTERNAL}"
echo ">> FPMPI_OBJS = ${FPMPI_OBJS}"
echo ">> INT32 = ${INT32}"
echo ">> R_FPMPI = ${R_FPMPI}"
echo ">> MPIP_EXT_LINK = ${MPIP_EXT_LINK}"
echo ">> TAU_EXT_LINK = ${TAU_EXT_LINK}"
echo ">> PROF_TYPE = ${PROF_TYPE}"
echo " "
echo "*************************************************************************"
echo " "

dnl Start to substitute templates
AC_SUBST(MPIRUN)
AC_SUBST(MPIEXEC)
AC_SUBST(ORTERUN)
AC_SUBST(TMP_INC)
AC_SUBST(TMP_LIB)
AC_SUBST(MPI_ROOT)
AC_SUBST(MPITYPE)
AC_SUBST(MPI_INCLUDE_PATH)
AC_SUBST(MPI_LIBPATH)
AC_SUBST(MPI_LIBS)
AC_SUBST(MPI_DEFS)
AC_SUBST(MPI_INCL2)
AC_SUBST(MPI_LDFLAGS)
AC_SUBST(PKG_CPPFLAGS)
AC_SUBST(PKG_LIBS)

dnl For profiler.
AC_SUBST(FPMPI_EXT_LINK)
AC_SUBST(FPMPI_INTERNAL)
AC_SUBST(FPMPI_OBJS)
AC_SUBST(INT32)
AC_SUBST(R_FPMPI)
AC_SUBST(MPIP_EXT_LINK)
AC_SUBST(TAU_EXT_LINK)
AC_SUBST(PROF_TYPE)

AC_OUTPUT(src/Makevars)

