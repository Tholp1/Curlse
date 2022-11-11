-- premake5.lua



workspace "Curlse"
   configurations { "Debug", "Release" }

os.execute("cmake -S jsoncpp -B build/jsoncpp")
os.execute("cmake -S libzip -B build/libzip")

externalproject "jsoncpp"
   location "build/jsoncpp"
   kind "StaticLib"
   language "C++"

externalproject "libzip""
   location "build/libzip"
   kind "StaticLib"
   language "C++"

project "Curlse"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "*.h", "*.cpp"}
   includedirs {".", "jsoncpp/include"}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
