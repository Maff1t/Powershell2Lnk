# Powershell2LNK
Powershell2LNK is a utility program that creates a Windows Shortcut (.LNK) file that executes a provided PowerShell script. The PowerShell script is base64 encoded and passed to PowerShell's -EncodedCommand parameter, which can execute base64 encoded PowerShell code.

## Usage
The application takes two arguments:

- Path to the input PowerShell script file (.ps1).
- Path to the output shortcut file (.lnk).

Example:

```bash
Powershell2LNK.exe "C:\path\to\your\script.ps1" "C:\path\to\your\shortcut.lnk"
```

This will create a shortcut file that, when executed, will run the PowerShell script provided
