$examplesDir = "d:\Github\ArduLora\examples"
$fqbn = "rak_rui:stm32:WisDuoRAK3172EvaluationBoard"
$libPath = "d:\Github\ArduLora"
$results = @()

$examples = Get-ChildItem -Path $examplesDir -Filter *.ino -Recurse

foreach ($example in $examples) {
    Write-Host "Compiling $($example.Name)..."
    $output = arduino-cli compile --fqbn $fqbn --libraries $libPath $example.FullName 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  Success!" -ForegroundColor Green
        $results += [PSCustomObject]@{ Example = $example.Name; Status = "Success"; Error = "" }
    } else {
        Write-Host "  Failed!" -ForegroundColor Red
        $results += [PSCustomObject]@{ Example = $example.Name; Status = "Failed"; Error = $output }
    }
}

$results | Out-File "d:\Github\ArduLora\build_results.txt"
$results | Format-Table | Out-Host
