# Get the solution directory
$solutionDir = Split-Path $dte.Solution.FullName

# Look for Node two levels below the solution
$nodeRoot = Join-Path $solutionDir "deps\node" -resolve

# Find the Node build script
$buildScript = Join-Path $nodeRoot "vcbuild.bat" -resolve

# Build Node for Release and Debug
Push-Location $nodeRoot
& "$buildScript" nosign x64 
& "$buildScript" nosign debug x64 
Pop-Location
