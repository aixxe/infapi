reg add "HKLM\Software\KONAMI\beatmania IIDX INFINITAS" /v "InstallDir" /t REG_SZ /d "%cd%"
reg add "HKLM\Software\KONAMI\beatmania IIDX INFINITAS" /v "ResourceDir" /t REG_SZ /d "%cd%\Resource"

infapi.exe