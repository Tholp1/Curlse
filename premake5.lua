-- premake5.lua

require "premake-vcpkg"
os.execute("chmod +x ./.vcpkg/vcpkg.exe")
os.execute("./.vcpkg/vcpkg.exe install jsoncpp")
os.execute("./.vcpkg/vcpkg.exe install libzippp")


workspace "Curlse"
   configurations { "Debug", "Release" }

project "Curlse"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
