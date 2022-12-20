-- premake5.lua

print("you may need to run this twice the first time")


function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end


workspace "Curlse"
   configurations { "Debug", "Release" }


os.execute("cmake -S jsoncpp -B build/jsoncpp -Wno-dev -DBUILD_SHARED_LIBS=OFF")
os.execute("cmake -S zlib -B zlib -Wno-dev -DBUILD_SHARED_LIBS=OFF")
os.execute("cmake -S libzip -B build/libzip -Wno-dev -DBUILD_SHARED_LIBS=OFF -DZLIB_LIBRARY=./zlib -DZLIB_INCLUDE_DIR=./zlib -DBUILD_TOOLS=OFF -DBUILD_EXAMPLES=OFF -DENABLE_COMMONCRYPTO=OFF -DENABLE_GNUTLS=OFF -DENABLE_MBEDTLS=OFF -DENABLE_OPENSSL=OFF -DENABLE_WINDOWS_CRYPTO=OFF -DENABLE_LZMA=OFF -DENABLE_BZIP2=OFF -DENABLE_ZSTD=OFF")
os.execute("cp build/libzip/zipconf.h libzip/lib/zipconf.h")
os.execute("cmake -S libzippp -B build/libzippp -Wno-dev -DBUILD_SHARED_LIBS=OFF -DZLIB_LIBRARY=./zlib -DZLIB_INCLUDE_DIR=./zlib -DLIBZIP_LIBRARY=./build/libzip/lib -DLIBZIP_INCLUDE_DIR=./libzip/lib -DLIBZIPPP_BUILD_TESTS=OFF")



externalproject "jsoncpp"
   location "build/jsoncpp"
   kind "StaticLib"
   language "C++"
   includedirs {".", "jsoncpp/include", "build/libzip", "build/libzippp",  "zlib"}

externalproject "zlib"
   location "zlib"
   kind "StaticLib"
   language "C"
   includedirs {".", "jsoncpp/include", "build/libzip", "build/libzippp", "zlib"}

externalproject "libzip"
   location "build/libzip"
   kind "StaticLib"
   language "C"
   includedirs {".", "jsoncpp/include", "libzip", "build/libzip", "build/libzippp", "zlib"}

externalproject "libzippp"
   location "build/libzippp"
   kind "StaticLib"
   language "C++"
   includedirs {".", "jsoncpp/include", "build/libzip", "build/libzippp", "zlib"}
if (file_exists("build/jsoncpp/lib/libjsoncpp.a"))
then
   local zlib = io.open("zlib/libz.a")
   local zlibnew = io.open("zlib/liblibz.a", "w")
   zlibnew:write(zlib:read("*a"))
   zlib:close()
   zlibnew:close()
end
if(file_exists("build/libzip/lib/libzip.a"))
then
   local ziplib = io.open("build/libzip/lib/libzip.a")
   local ziplibnew = io.open("build/libzip/lib/liblibzip.a", "w")
   ziplibnew:write(ziplib:read("*a"))
   ziplib:close()
   ziplibnew:close()
end
   links{"build/libzip/lib/libzip", "zlib/libz"}


project "Curlse"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "*.h", "*.cpp"}
   includedirs {".", "jsoncpp/include", "build/libzip", "build/libzippp", "zlib"}
   --libdirs {"build/jsoncpp/lib/", "build/libzippp/"} 

if (file_exists("build/libzippp/libzippp_static.a"))
then
   local zippplib = io.open("build/libzippp/libzippp_static.a")
   local zippplibnew = io.open("build/libzippp/liblibzippp_static.a", "w")
   zippplibnew:write(zippplib:read("*a"))
   zippplib:close()
   zippplibnew:close()
end
if (file_exists("build/jsoncpp/lib/libjsoncpp.a"))
then
   local jsonlib = io.open("build/jsoncpp/lib/libjsoncpp.a")
   local jsonlibnew = io.open("build/jsoncpp/lib/liblibjsoncpp.a", "w")
   jsonlibnew:write(jsonlib:read("*a"))
   jsonlib:close()
   jsonlibnew:close()
end
   links {"build/jsoncpp/lib/libjsoncpp", "build/libzippp/libzippp_static","build/libzip/lib/libzip", "zlib/libz"} 
   --links {"libjsoncpp", "libzippp_static"} 
   buildoptions{"-Wzero-as-null-pointer-constant"}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

    filter "system:windows"
      defines {"Windows"}
