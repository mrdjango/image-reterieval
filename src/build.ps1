Write-Host "Building Letter Recognition System for Windows..." -ForegroundColor Green

# Check if g++ is available
try {
    $null = Get-Command g++ -ErrorAction Stop
    Write-Host "Found g++ compiler" -ForegroundColor Green
} catch {
    Write-Host "Error: g++ not found. Please install MinGW or another C++ compiler." -ForegroundColor Red
    exit 1
}

# Create build directory
if (!(Test-Path "build_cpu")) {
    New-Item -ItemType Directory -Path "build_cpu" | Out-Null
}
Set-Location "build_cpu"

# Try to find OpenCV
$opencvFound = $false
$opencvInclude = ""
$opencvLibs = ""

# Try vcpkg path
if (Test-Path "C:/vcpkg/installed/x64-windows/include/opencv2") {
    Write-Host "Found OpenCV in vcpkg" -ForegroundColor Green
    $opencvInclude = "-I`"C:/vcpkg/installed/x64-windows/include`""
    $opencvLibs = "-L`"C:/vcpkg/installed/x64-windows/lib`" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui"
    $opencvFound = $true
}

# Try standard OpenCV path
if (!$opencvFound -and (Test-Path "C:/opencv/build/include/opencv2")) {
    Write-Host "Found OpenCV in standard path" -ForegroundColor Green
    $opencvInclude = "-I`"C:/opencv/build/include`""
    $opencvLibs = "-L`"C:/opencv/build/x64/mingw/lib`" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui"
    $opencvFound = $true
}

# Try MSYS2 path
if (!$opencvFound -and (Test-Path "C:/msys64/mingw64/include/opencv4")) {
    Write-Host "Found OpenCV in MSYS2" -ForegroundColor Green
    $opencvInclude = "-I`"C:/msys64/mingw64/include/opencv4`""
    $opencvLibs = "-L`"C:/msys64/mingw64/lib`" -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui"
    $opencvFound = $true
}

if (!$opencvFound) {
    Write-Host "Error: OpenCV not found. Please install OpenCV and update the paths in this script." -ForegroundColor Red
    Write-Host "Common installation methods:" -ForegroundColor Yellow
    Write-Host "  - vcpkg: vcpkg install opencv" -ForegroundColor Yellow
    Write-Host "  - MSYS2: pacman -S mingw-w64-x86_64-opencv" -ForegroundColor Yellow
    Write-Host "  - Manual download from opencv.org" -ForegroundColor Yellow
    exit 1
}

# Compile template_generator
Write-Host "Compiling template_generator..." -ForegroundColor Yellow
$result = & g++ -std=c++17 -O3 $opencvInclude -o template_generator.exe ../template_generator.cpp ../letter_recognition.cpp $opencvLibs 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling template_generator:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}

# Compile recognize
Write-Host "Compiling recognize..." -ForegroundColor Yellow
$result = & g++ -std=c++17 -O3 $opencvInclude -o recognize.exe ../recognize.cpp ../letter_recognition.cpp $opencvLibs 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling recognize:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}

# Compile main
Write-Host "Compiling main..." -ForegroundColor Yellow
$result = & g++ -std=c++17 -O3 $opencvInclude -o main.exe ../main.cpp ../letter_recognition.cpp $opencvLibs 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling main:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "Available executables:" -ForegroundColor Cyan
Write-Host "  template_generator.exe - Generate letter templates" -ForegroundColor White
Write-Host "  recognize.exe - Recognize letters in images" -ForegroundColor White
Write-Host "  main.exe - Main application" -ForegroundColor White
Write-Host ""
Write-Host "To run the recognition system:" -ForegroundColor Cyan
Write-Host "  ./recognize.exe" -ForegroundColor White
Write-Host ""
Write-Host "To generate templates:" -ForegroundColor Cyan
Write-Host "  ./template_generator.exe" -ForegroundColor White 