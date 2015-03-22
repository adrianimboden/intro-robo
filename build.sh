#!/bin/bash -e
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TMPDIR="$(mktemp -d)"
function finish
{
	rm -fR ${TMPDIR}
}
trap finish EXIT

echo -------------- building unittests
mkdir ${TMPDIR}/tests
cd ${TMPDIR}/tests
cmake ${DIR}/unittests
make -j4
./unittests --gtest_output=xml:${DIR}/testresults.xml
echo -------------- executing unittests

echo -------------- building firmware
cd ${DIR}
./init.sh
mkdir ${TMPDIR}/workspace
echo -------------- import into workspace
kinetis-design-studio -noSplash -data $TMPDIR/workspace -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import remotecontrol -import robo
echo -------------- generate processorexpert
kinetis-design-studio -noSplash -data $TMPDIR/workspace -application com.freescale.processorexpert.core.PExApplication -generateAll
echo -------------- build project
kinetis-design-studio -noSplash -data $TMPDIR/workspace -application org.eclipse.cdt.managedbuilder.core.headlessbuild -build all


echo -------------- generate documentation
cd ${DIR}/remotecontrol/doxy
doxygen remotecontrol.doxyfile
cd ${DIR}/robo/doxy
doxygen robo.doxyfile


