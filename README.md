# Curlse

Download CurseForge modpacks to MultiMC instances using CLI.
This is a terrible terrible tool as I mostly made it for myself, but it does get the job done. You need to download the manifest zip file from the website and use the curlse on it.

Future development from myself is unlikely, if you'd like to make a pull request I'd be happy to atleast look at it.

while i would rather use GPL3, im using MPL2 to be on the safe side, even though this shares no code with it, I use Feriums api key to save some headache

# Building
this project uses premake5 for its build system (https://premake.github.io/download/)

Linux:
`premake5 gmake2 && make .`

Windows and Mac users, you are on your own, but its probably similar.

# Use

`curlse (modpack manifest file) (dir of multimc instance folder)`

With the blanks filled in It'll look something like this:

`curlse ~/Downloads/Above+and+Beyond-1.3.zip ~/bin/MultiMC.d/instances`
