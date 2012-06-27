#!/bin/bash

# AUTHOR: Destan Sarpkaya
# June 2012

#check if in right directory
clear
packegingPath=`pwd`
if [[ "$packegingPath" != *packaging/linux* ]]
then
	DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
	echo "You are on wrong path! Jumping to $DIR"
    cd "$DIR"
fi

cevirgecVersion=0.0.1 #hardcoded for now!!!
#get version
#cevirgecVersion=`cat ../../resources/app/changelog.txt | grep "(.*)" -w -m 1 -o`    # parse version from changelog
#cevirgecVersion=`echo ${cevirgecVersion/"("/}`                                                          # get rid of the paranthesis
#cevirgecVersion=`echo ${cevirgecVersion/")"/}`
echo "Using version " $cevirgecVersion

#get architecture
echo "Enter architecture: (32 or 64)"
read cevirgecArchitecture

if [[ "$cevirgecArchitecture" != "32" && "$cevirgecArchitecture" != "64" ]]
then
    echo "Wrong architecture! Terminating."
    exit
fi

if [[ "$cevirgecArchitecture" == "32" ]]
then
    cevirgecArchitecture="i386"
fi

if [[ "$cevirgecArchitecture" == "64" ]]
then
    cevirgecArchitecture="amd64"
fi

echo "Using architecture " $cevirgecArchitecture

#prepare files
rm -rf area																#clean 'area'
mkdir -p area/searcheverything-$cevirgecVersion/data
cp -R common_files_debian/data/* area/searcheverything-$cevirgecVersion/data	#copy data folder
cp ../../* area/searcheverything-$cevirgecVersion								#copy source
sed -i "s/{DATE_PLACEHOLDER}/`date -R`/" ../../resources/app/changelog.txt    # write date of changelog, this should be before 'copy resources' otherwise source mismatch occurs and packaging fails
cp -R ../../resources area/searcheverything-$cevirgecVersion					#copy resources folder of source
cd area/searcheverything-$cevirgecVersion
rm -v *.pro.user *.o *.obj *.cdx *.dc

#execute dh_make
export DEBFULLNAME="Destan Sarpkaya"
export DEBEMAIL="destan@dorukdestan.com"
dh_make -e destan@dorukdestan.com -c gpl -s --createorig				#this creates the 'debian' folder
cd debian
rm -v *.ex *.EX
cd ..
echo "moving control, copyright files to debian folder"
cp ../../common_files_debian/* debian/

#manipulate version, architecture
sed -i "s/VERSION_PLACEHOLDER/$cevirgecVersion/" debian/control
sed -i "s/ARCH_PLACEHOLDER/$cevirgecArchitecture/" debian/control

#manipulate changelog 
echo "writing changelog"
cp resources/app/changelog.txt debian/changelog                         # Copy changelog from resources

#start build
debuild -kdestan@dorukdestan.com

echo ==================
echo "packaging finished"
echo ==================
