param (
    [string]$versionTag
)

# Import PowerShellForGitHub module
Import-Module PowerShellForGitHub

# Base URL for downloading files
$owner = "wiresock"
$repository = "TcpInspector"
$baseURL = "https://github.com/$owner/$repository/releases/download/$versionTag/"
$files = @("TcpInspector-v0.1.0-ARM64.zip", "TcpInspector-v0.1.0-x64.zip", "TcpInspector-v0.1.0-x86.zip")

foreach ($file in $files) {
    $downloadURL = $baseURL + $file
    $downloadPath = "./" + $file
    Invoke-WebRequest -Uri $downloadURL -OutFile $downloadPath

    # Unzip the file
    $extractPath = $downloadPath -replace ".zip", ""
    Expand-Archive -LiteralPath $downloadPath -DestinationPath $extractPath -Force

    # Sign the executable
    $exePath = $extractPath + "/TcpInspector.exe"
    & signtool sign /fd sha1 /t http://timestamp.digicert.com /n "IP SMIRNOV VADIM VALERIEVICH" $exePath
    & signtool sign /as /td sha256 /fd sha256 /tr http://timestamp.digicert.com /n "IP SMIRNOV VADIM VALERIEVICH" $exePath

    # Change to the directory of the folder to be zipped
    Push-Location $extractPath

    # Get all items in the current directory (the extracted folder)
    $items = Get-ChildItem

    # Define the path for the new ZIP file
    $zipPath = "../" + $file -replace ".zip", "-signed.zip"
    Compress-Archive -Path $items -DestinationPath $zipPath -Force

    # Return to the original directory
    Pop-Location

    # Delete the original ZIP file and extracted folder
    Remove-Item -Path $downloadPath -Force
    Remove-Item -Path $extractPath -Recurse -Force
}

# Getting the GitHub release
$release = Get-GitHubRelease -OwnerName $owner -RepositoryName $repository -Tag $versionTag

# Removing existing ZIP files from the release
$assets = Get-GitHubReleaseAsset -OwnerName $owner -RepositoryName $repository -ReleaseId $release.id
foreach ($asset in $assets) {
    if ($asset.name -like "*.zip") {
        Remove-GitHubReleaseAsset -OwnerName $owner -RepositoryName $repository -AssetId $asset.id -Force
    }
}

# Uploading new ZIP files and deleting them after upload
foreach ($file in Get-ChildItem "./" -Filter "*-signed.zip") {
    $release | New-GitHubReleaseAsset -Path $file.FullName
    Remove-Item -Path $file.FullName -Force
}

