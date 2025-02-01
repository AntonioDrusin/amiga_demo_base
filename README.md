# Credits

Most of the code is copied and modified from the original code included in the fantastic Visual Studio Code Extension Amiga Debug by Bartman/Abyss.
https://github.com/BartmanAbyss/vscode-amiga-debug


I am including the source code for P6112 player, for modification to suit your demo/game. This is not my work. https://aminet.net/package/mus/misc/P6112



# About
This is an Amiga startup project for demos or games. I needed a few changes from the default project that BartmanAbyss provides (which is more oriented towards demos)


1. Safe copper wait for waiting past line 255.
2. Ability to run in NTSC.
3. Separation of concerns for screen setup.
4. Separation of concerns for copper support.
5. Full inclusion of a newer P61 player source code, for modifications and integration.
6. Makefile modified to run convert.exe when the graphics sources have changed.