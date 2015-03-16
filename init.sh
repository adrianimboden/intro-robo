#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
cd robo
rm ProcessorExpert.pe
cp ProcessorExpert.pe.template ProcessorExpert.pe
cd ..
cd remotecontrol
rm ProcessorExpert.pe
cp ProcessorExpert.pe.template ProcessorExpert.pe
