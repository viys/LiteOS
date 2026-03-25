param (
    [Parameter(Position = 0, Mandatory)]
    [ValidateSet("all", "cmake", "make", "clean", "delete", "menuconfig", "help")]
    [string]$Action
)

$rootPath = $PSScriptRoot
$buildDir = Join-Path $rootPath "build"
$srcDir   = $rootPath

# ====================== 工具函数 ======================

function Assert-ToolInPath {
    param([Parameter(Mandatory)][string]$ToolName)
    $cmd = Get-Command $ToolName -ErrorAction SilentlyContinue
    if (-not $cmd) {
        Write-Host "未找到工具：$ToolName" -ForegroundColor Red
        Write-Host "请确认已安装并添加到 PATH。" -ForegroundColor Yellow
        exit 1
    }
}

function Assert-Environment {
    Assert-ToolInPath "cmake"
    Assert-ToolInPath "ninja"
}

function Assert-MenuconfigEnvironment {
    Assert-ToolInPath "menuconfig.exe"
    Assert-ToolInPath "python"
}

function Invoke-CMakeConfig {
    Write-Host ">>> CMake 配置" -ForegroundColor Cyan
    cmake -B $buildDir -S $srcDir -G Ninja
    if ($LASTEXITCODE -ne 0) {
        Write-Host "CMake 配置失败（退出码 $LASTEXITCODE）" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

function Invoke-CMakeBuild {
    if (-not (Test-Path $buildDir)) {
        Write-Host "构建目录不存在，请先执行 cmake。" -ForegroundColor Red
        exit 1
    }
    Write-Host ">>> 编译" -ForegroundColor Cyan
    cmake --build $buildDir
    if ($LASTEXITCODE -ne 0) {
        Write-Host "编译失败（退出码 $LASTEXITCODE）" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    Write-Host "Build time: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -ForegroundColor Green
}

function Invoke-CMakeClean {
    if (-not (Test-Path $buildDir)) {
        Write-Host "构建目录不存在，跳过 clean。" -ForegroundColor Yellow
        return
    }
    Write-Host ">>> 清理编译产物（保留构建目录）" -ForegroundColor Cyan
    cmake --build $buildDir --target clean
    if ($LASTEXITCODE -ne 0) {
        Write-Host "清理失败（退出码 $LASTEXITCODE）" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

function Invoke-Delete {
    if (Test-Path $buildDir) {
        Write-Host ">>> 删除构建目录: $buildDir" -ForegroundColor Yellow
        Remove-Item -Recurse -Force $buildDir
    } else {
        Write-Host "构建目录不存在，无需删除。" -ForegroundColor Yellow
    }
}

function Invoke-Menuconfig {
    $releaseDir = Join-Path $rootPath "release"
    $kconfigDir = Join-Path $rootPath "scripts/kconfig"
    $configH    = Join-Path $releaseDir "my_config.h"
    $logTxt     = Join-Path $releaseDir "log.txt"
    $dotConfig  = Join-Path $releaseDir ".config"

    if (-not (Test-Path $releaseDir)) {
        New-Item -ItemType Directory -Force -Path $releaseDir | Out-Null
    }

    Write-Host ">>> 运行 menuconfig" -ForegroundColor Cyan
    Push-Location $releaseDir
    try {
        menuconfig.exe (Join-Path $kconfigDir "Kconfig")
        if ($LASTEXITCODE -ne 0) {
            Write-Host "menuconfig 失败（退出码 $LASTEXITCODE）" -ForegroundColor Red
            return
        }
        python (Join-Path $kconfigDir "kconfig.py") (Join-Path $kconfigDir "Kconfig") .config $configH $logTxt $dotConfig
        if ($LASTEXITCODE -ne 0) {
            Write-Host "kconfig.py 失败（退出码 $LASTEXITCODE）" -ForegroundColor Red
            exit $LASTEXITCODE
        }
    } finally {
        Pop-Location
    }
}

function Show-Help {
    Write-Host ""
    Write-Host "用法：" -ForegroundColor White
    Write-Host "  .\build.ps1 <Action>"
    Write-Host ""
    Write-Host "Action：" -ForegroundColor Yellow
    Write-Host "  all          配置并构建工程"
    Write-Host "  cmake        执行 CMake 配置"
    Write-Host "  make         执行编译（cmake --build）"
    Write-Host "  clean        清理编译产物（保留构建目录）"
    Write-Host "  delete       删除整个构建目录"
    Write-Host "  menuconfig   运行 menuconfig 配置工程"
    Write-Host "  help         显示此帮助信息"
    Write-Host ""
    Write-Host "示例：" -ForegroundColor Yellow
    Write-Host "  .\build.ps1 all"
    Write-Host "  .\build.ps1 cmake"
    Write-Host "  .\build.ps1 make"
    Write-Host "  .\build.ps1 clean"
    Write-Host "  .\build.ps1 delete"
    Write-Host "  .\build.ps1 menuconfig"
    Write-Host ""
}

# ====================== 入口 ======================

if ($Action -eq "help") { Show-Help; exit 0 }

if ($Action -eq "menuconfig") {
    Assert-MenuconfigEnvironment
} else {
    Assert-Environment
}
Set-Location $rootPath

trap {
    Set-Location $rootPath
    break
}

switch ($Action) {
    "all" {
        Invoke-CMakeConfig
        Invoke-CMakeBuild
        Write-Host "[完成] 工程构建完成。" -ForegroundColor Green
    }
    "cmake"      { Invoke-CMakeConfig }
    "make"       { Invoke-CMakeBuild }
    "clean"      { Invoke-CMakeClean }
    "delete"     { Invoke-Delete }
    "menuconfig" { Invoke-Menuconfig }
}
