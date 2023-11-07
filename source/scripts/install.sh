adb push ../build/app/aibox /data/local/tmp
adb push run.sh /data/local/tmp
adb shell chmod +x /data/local/tmp/aibox
adb shell chmod +x /data/local/tmp/run.sh