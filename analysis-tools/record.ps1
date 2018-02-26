$filePath = "C:\Users\test\Desktop\MOUSE-LOG.txt"
$firstLine = (Get-Content $filePath | Measure-Object -Line).Lines
echo "Recording calls to EncodeSecrecy_V11, press a key when done..."
pause
$lastLine = (Get-Content $filePath | Measure-Object -Line).Lines
Get-Content -Tail ($lastLine - $firstLine) $filePath | Set-Content $args[0]
