<#
.SYNOPSIS
清空当前仓库根目录的 tmp，保留目录本身。
.DESCRIPTION
必须先结束使用临时文件的生成、构建与验证进程，并将需要保留的证据移出 tmp。
拒绝重解析点和仓库外路径；删除失败立即报错。使用 -WhatIf 预览。
.EXAMPLE
pwsh -File tools/clean_tmp.ps1 -WhatIf
.EXAMPLE
pwsh -File tools/clean_tmp.ps1
#>
[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Low')]
param()

$ErrorActionPreference = 'Stop'
$repositoryRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if (-not (Test-Path -LiteralPath (Join-Path $repositoryRoot 'AGENTS.md') -PathType Leaf) -or
    -not (Test-Path -LiteralPath (Join-Path $repositoryRoot 'VERSION') -PathType Leaf)) {
    throw '无法确认 ChaLi 仓库根目录，拒绝清理。'
}
$temporaryRoot = [IO.Path]::GetFullPath((Join-Path $repositoryRoot 'tmp'))
if ([IO.Path]::GetDirectoryName($temporaryRoot) -ne $repositoryRoot -or
    [IO.Path]::GetFileName($temporaryRoot) -ne 'tmp') {
    throw '清理目标不在仓库根目录 tmp，拒绝清理。'
}
if (-not (Test-Path -LiteralPath $temporaryRoot)) {
    Write-Output "临时目录不存在，无需清理：$temporaryRoot"
    return
}
$directory = Get-Item -LiteralPath $temporaryRoot -Force
if (-not $directory.PSIsContainer -or
    ($directory.Attributes -band [IO.FileAttributes]::ReparsePoint)) {
    throw 'tmp 不是普通目录，拒绝清理。'
}
# PowerShell 默认不跟随重解析点；先检查全树，避免删除期间才发现特殊目录。
$links = @(Get-ChildItem -LiteralPath $temporaryRoot -Recurse -Force -Attributes ReparsePoint)
if ($links.Count -gt 0) {
    throw "tmp 包含重解析点，请先人工处理：$($links[0].FullName)"
}
$entries = @(Get-ChildItem -LiteralPath $temporaryRoot -Force)
foreach ($entry in $entries) {
    $resolvedTarget = [IO.Path]::GetFullPath($entry.FullName)
    if ([IO.Path]::GetDirectoryName($resolvedTarget) -ne $temporaryRoot) {
        throw "目标不在 tmp 内：$resolvedTarget"
    }
    if ($entry.Attributes -band [IO.FileAttributes]::ReparsePoint) {
        throw "tmp 包含重解析点，请先人工处理：$resolvedTarget"
    }
    if ($PSCmdlet.ShouldProcess($resolvedTarget, '删除临时文件或目录')) {
        Remove-Item -LiteralPath $resolvedTarget -Recurse -Force -ErrorAction Stop
    }
}
Write-Output "tmp 清理完成（预览模式：$WhatIfPreference）：$temporaryRoot"
