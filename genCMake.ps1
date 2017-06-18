if (-Not (Test-Path "build64"))
{
    New-Item "build64" -ItemType "directory"
}

if (Test-Path "build64")
{
    Set-Location "build64"

    cmake .. -G "Visual Studio 15 2017 Win64"

    Set-Location ".."
}