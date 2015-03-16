#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
cd robo
cp ProcessorExpert.pe.template ProcessorExpert.pe
cd ..
cd remotecontrol
cp ProcessorExpert.pe.template ProcessorExpert.pe
