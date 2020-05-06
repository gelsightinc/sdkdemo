echo off

REM Because we use IF statements, we need this setlocal statement and we will
REM to expand environment variables using exclamation syntax !<var>! instead of
REM percent syntax %<var>%.


setlocal enabledelayedexpansion

REM Set SRCDIR_ROOT directory to the current folder.  This assumes that this 
REM batch file lives at the root of the SDK tree.

set SRCDIR_ROOT=%~dp0

if DEFINED GS_SDK goto gssdk_defined

    echo "The required environment variable GS_SDK is not defined"
    echo "    You must define GS_SDK to point to the GelSightSdk folder"
    echo "    copysdk cannot continue"
    pause
    exit /b

:gssdk_defined


REM Currently we only build for x64 so this is simple


REM Copy 3rd party DLLs to build output folders so we 
REM can run the app with everything we need present
REM (i.e. no need for adding any other folder to the PATH)


REM TODO:  Make all these things parameters (e.g. Release/Debug, Eigen/gtest/libpng, v110/v120) 
REM so that I don't have to repeat the same line so many times


REM For each configuration C -- aks VS macro $(Configuration)

for %%C in  (
        Debug
        Release
    ) do (

        REM For each toolset T  -- aka VS macro $(PlatformToolset)

        for %%T in (
            v141
            v142
        ) do (

            REM For each platform (short name) P -- aka VS macro $(PlatformShortName)

            for %%P in (
                x64
            ) do (

                REM "OUTDIR" is the directory to which we copy stuff for this
                REM loop iteration

                set OUTDIR=!SRCDIR_ROOT!%%P\%%C\%%T\

                REM Copy dlls

                set SDIR=!GS_SDK!\%%P\%%C\%%T
                xcopy /D/I/Y "!SDIR!\*.dll" "!OUTDIR!"
				xcopy /D/I/Y "!SDIR!\*.pdb" "!OUTDIR!"
       
            )


        )
    )





pause
