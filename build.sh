#!/bin/bash -e
TMPDIR="$(mktemp -d)"
function finish
{
	rm -fR ${TMPDIR}
}
trap finish EXIT

./init.sh

export PATH=/opt/Freescale/KDS_2.0.0/toolchain/bin/:$PATH
export PATH=/opt/Freescale/KDS_2.0.0/eclipse/:$PATH

mkdir $TMPDIR/workspace

echo -------------- import into workspace
kinetis-design-studio -noSplash -data $TMPDIR/workspace -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import remotecontrol -import robo
echo -------------- generate processorexpert
kinetis-design-studio -noSplash -data $TMPDIR/workspace -application com.freescale.processorexpert.core.PExApplication -generateAll
echo -------------- build project
kinetis-design-studio -noSplash -data $TMPDIR/workspace -application org.eclipse.cdt.managedbuilder.core.headlessbuild -build all

