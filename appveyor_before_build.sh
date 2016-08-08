#STRIP="> /dev/null 2>&1"
APPVEYOR=0 # Force re-download of boost!

download() {
	if [ $# -lt 3 ]; then
		echo "Invalid parameters to download."
		return 1
	fi

	NAME=$1
	shift

	echo "$NAME..."

	while [ $# -gt 1 ]; do
		URL=$1
		FILE=$2
		shift
		shift

		if ! [ -f $FILE ]; then
			printf "  Downloading $FILE... "

			if [ -z $VERBOSE ]; then
				curl --silent --retry 10 -kLy 5 -o $FILE $URL
				RET=$?
			else
				curl --retry 10 -kLy 5 -o $FILE $URL
				RET=$?
			fi

			if [ $RET -ne 0 ]; then
				echo "Failed!"
			else
				echo "Done."
			fi
		else
			echo "  $FILE exists, skipping."
		fi
	done

	if [ $# -ne 0 ]; then
		echo "Missing parameter."
	fi
}

real_pwd() {
	pwd | sed "s,/\(.\),\1:,"
}

CMAKE_OPTS=""
add_cmake_opts() {
	CMAKE_OPTS="$CMAKE_OPTS $@"
}


PLATFORM=$1

if [ -z $PLATFORM ]; then
	PLATFORM=`uname -m`
fi

case $PLATFORM in
	x64|x86_64|x86-64|win64|Win64 )
		ARCHNAME=x86-64
		ARCHSUFFIX=64
		BITS=64
		;;

	x32|x86|i686|i386|win32|Win32 )
		ARCHNAME=x86
		ARCHSUFFIX=86
		BITS=32
		;;

	* )
		echo "Unknown platform $PLATFORM."
		exit 1
		;;
esac


echo "Downloading dependency packages."

mkdir -p deps
cd deps

DEPS="`pwd`"

# GLM
nuget install glm

# Boost
if [ -z $APPVEYOR ]; then
	download "Boost 1.61.0" \
		http://sourceforge.net/projects/boost/files/boost-binaries/1.61.0/boost_1_61_0-msvc-14.0-$BITS.exe \
		boost-1.61.0-win$BITS.exe
fi

# Bullet
download "Bullet 2.83.7" \
	https://dl.dropboxusercontent.com/u/13558082/openrw-deps/Bullet-2.83.7-win$BITS-vc140.7z \
	Bullet-2.83.7-win$BITS-vc140.7z

# OpenAL
download "OpenAL-Soft 1.16.0" \
	http://kcat.strangesoft.net/openal-binaries/openal-soft-1.16.0-bin.zip \
	OpenAL-Soft-1.16.0.zip

# SDL2
download "SDL 2.0.3" \
	https://www.libsdl.org/release/SDL2-devel-2.0.3-VC.zip \
	SDL2-2.0.3.zip

cd ..

mkdir -p Build_$BITS/deps
cd Build_$BITS/deps

DEPS_INSTALL=`pwd`
cd $DEPS

echo
echo "Extracting dependencies..."

# GLM
printf "glm... "
{
	add_cmake_opts -DGLM_ROOT_DIR="`real_pwd`/glm.0.9.7.1/build/native"

	echo done.
}
cd $DEPS

# Boost
printf "Boost 1.61.0... "
{
	if [ -z $APPVEYOR ]; then
		cd $DEPS_INSTALL

		BOOST_SDK="`real_pwd`/Boost"

		if [ -d Boost ] && grep "BOOST_VERSION 106100" Boost/boost/version.hpp > /dev/null; then
			printf "Exists. "
		elif [ -z $SKIP_EXTRACT ]; then
			rm -rf Boost
			$DEPS/boost-1.61.0-win$BITS.exe //dir="$(echo $BOOST_SDK | sed s,/,\\\\,g)" //verysilent
		fi

		add_cmake_opts -DBOOST_ROOT="$BOOST_SDK" \
			-DBOOST_LIBRARYDIR="$BOOST_SDK/lib$BITS-msvc-14.0"

		echo Done.
	else
		# Appveyor has all the boost we need already
		BOOST_SDK="C:/Libraries/boost_1_60_0"
		add_cmake_opts -DBOOST_ROOT="$BOOST_SDK" \
			-DBOOST_LIBRARYDIR="$BOOST_SDK/lib$BITS-msvc-14.0"

		echo AppVeyor.
	fi
}
cd $DEPS

# Bullet
printf "Bullet 2.83.7... "
{
	cd $DEPS_INSTALL

	if [ -d Bullet ]; then
		printf "Exists. (No version checking) "
	elif [ -z $SKIP_EXTRACT ]; then
		rm -rf Bullet
		eval 7z x -y $DEPS/Bullet-2.83.7-win$BITS-vc140.7z $STRIP
		mv Bullet-2.83.7-win$BITS-vc140 Bullet
	fi

	BULLET_SDK="`real_pwd`/Bullet"
	add_cmake_opts -DBULLET_INCLUDE_DIR="$BULLET_SDK/include/bullet" \
		-DBULLET_DYNAMICS_LIBRARY="$BULLET_SDK/lib/BulletDynamics.lib" \
		-DBULLET_SOFTBODY_LIBRARY="$BULLET_SDK/lib/BulletSoftBody.lib" \
		-DBULLET_COLLISION_LIBRARY="$BULLET_SDK/lib/BulletCollision.lib" \
		-DBULLET_COLLISION_LIBRARY_DEBUG="$BULLET_SDK/lib/BulletCollision_Debug.lib" \
		-DBULLET_MATH_LIBRARY="$BULLET_SDK/lib/LinearMath.lib" \
		-DBULLET_MATH_LIBRARY_DEBUG="$BULLET_SDK/lib/LinearMath_Debug.lib"

	echo Done.
}
cd $DEPS

# OpenAL
printf "OpenAL-Soft 1.16.0... "
{
	if [ -d openal-soft-1.16.0-bin ]; then
		printf "Exists. "
	elif [ -z $SKIP_EXTRACT ]; then
		rm -rf openal-soft-1.16.0-bin
		eval 7z x -y OpenAL-Soft-1.16.0.zip $STRIP
	fi

	OPENAL_SDK="`real_pwd`/openal-soft-1.16.0-bin"

	add_cmake_opts -DOPENAL_INCLUDE_DIR="$OPENAL_SDK/include" \
		-DOPENAL_LIBRARY="$OPENAL_SDK/libs/Win$BITS/OpenAL32.lib"

	echo Done.
}
cd $DEPS

# SDL2
printf "SDL 2.0.3... "
{
	if [ -d SDL2-2.0.3 ]; then
		printf "Exists. "
	elif [ -z $SKIP_EXTRACT ]; then
		rm -rf SDL2-2.0.3
		eval 7z x -y SDL2-2.0.3.zip $STRIP
	fi

	SDL_SDK="`real_pwd`/SDL2-2.0.3"
	add_cmake_opts -DSDL2_INCLUDE_DIR="$SDL_SDK/include" \
		-DSDL2MAIN_LIBRARY="$SDL_SDK/lib/x$ARCHSUFFIX/SDL2main.lib" \
		-DSDL2_LIBRARY_PATH="$SDL_SDK/lib/x$ARCHSUFFIX/SDL2.lib"

	#FIXME: add_runtime_dlls `pwd`/SDL2-2.0.3/lib/x$ARCHSUFFIX/SDL2.dll

	echo Done.
}


cd $DEPS_INSTALL/..

cmake -G "Visual Studio 14 2015 Win64" -DBUILD_TESTS=0 -DTESTS_NODATA=1 .. $CMAKE_OPTS
cd ..
