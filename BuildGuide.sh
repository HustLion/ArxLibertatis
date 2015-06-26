#! /bin/sh

# Original: http://wiki.arx-libertatis.org/Downloading_and_Compiling_under_Linux

cd ArxLibertatis
mkdir build
cd build

# dependencies
sudo apt-get install git build-essential cmake zlib1g-dev libfreetype6-dev libopenal1 libopenal-dev mesa-common-dev libgl1-mesa-dev libboost-dev libsdl1.2-dev libglew-dev qt-sdk gdb libglm-dev bsdtar

cmake ..

sudo cp -r /usr/local/include/freetype2/freetype/ /usr/local/include/freetype

make -j`getconf _NPROCESSORS_ONLN`


# get data
# http://wiki.arx-libertatis.org/Installing_the_game_data_under_Linux#GOG.com_setup
# demo data: http://games.softpedia.com/get/Games-Demo/Arx-Fatalis.shtml

# just press enter and use default settings
cp ../scripts/arx-install-data arx-install-data

# run
./arx










# the following is just try to install freetype which failed.. use above is ok.
sudo apt-get install cmake libtool
wget http://download.savannah.gnu.org/releases/freetype/freetype-2.4.0.tar.gz
tar -xzvf freetype-2.4.0.tar.gz
cd freetype-2.4.0/
./configure --prefix=/usr/local/freetype
make
sudo mkdir /usr/local/freetype/include/freetype2/freetype/internal
sudo make install
cd builds/unix
# the following line should produce: 2.4.0, then this is a success.
freetype-config --ftversion

export FREETYPE_INCLUDE_DIRS=/usr/local/bin
export FREETYPE_DIR=/usr/local/bin



