<#
.SYNOPSIS
为标准库隔离回归临时限制项目写入，按保存的 DACL 恢复。
#>
param(
    [ValidateSet('lock', 'restore')][string]$Mode,
    [string]$Project,
    [string]$Source,
    [string]$Backup
)
$ErrorActionPreference = 'Stop'
if ($Mode -eq 'lock') {
    $records = @(
        @{ Path = $Project; Directory = $true; Sddl = (Get-Acl -LiteralPath $Project).Sddl },
        @{ Path = $Source; Directory = $false; Sddl = (Get-Acl -LiteralPath $Source).Sddl }
    )
    $records | ConvertTo-Json | Set-Content -LiteralPath $Backup -Encoding UTF8
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent().User
    foreach ($record in $records) {
        $acl = Get-Acl -LiteralPath $record.Path
        $rights = [Security.AccessControl.FileSystemRights]'WriteData,AppendData'
        $rule = [Security.AccessControl.FileSystemAccessRule]::new($identity, $rights, [Security.AccessControl.AccessControlType]::Deny)
        $acl.AddAccessRule($rule)
        Set-Acl -LiteralPath $record.Path -AclObject $acl
    }
} else {
    if (-not (Test-Path -LiteralPath $Backup)) { return }
    foreach ($record in (Get-Content -LiteralPath $Backup -Raw | ConvertFrom-Json)) {
        $acl = if ($record.Directory) { [Security.AccessControl.DirectorySecurity]::new() } else { [Security.AccessControl.FileSecurity]::new() }
        $acl.SetSecurityDescriptorSddlForm($record.Sddl, [Security.AccessControl.AccessControlSections]::Access)
        Set-Acl -LiteralPath $record.Path -AclObject $acl
    }
}
