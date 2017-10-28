$filePath = "C:\Users\test\Desktop\MOUSE-LOG.txt"
$firstLine = (Get-Content $filePath | Measure-Object -Line).Lines
echo "Recording HID feature report commands, press a key when done..."
pause
$lastLine = (Get-Content $filePath | Measure-Object -Line).Lines
Get-Content -Tail ($lastLine - $firstLine) $filePath | Set-Content $args[0]