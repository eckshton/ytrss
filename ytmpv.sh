#!/bin/bash

mkfifo pipe
yt-dlp -o - -f 'bestvideo' "$1" > pipe.video &
pids+=($!)
yt-dlp -o - -f 'bestaudio' "$1" > pipe.audio &
pids+=($!)

while true; do
    if [[ $(du pipe.audio | awk '{print $1}') != 0 ]]; then
        mpv --audio-file=pipe.audio pipe.video
        break
    fi
    sleep 1
done

for pid in "${pids[@]}"; do
    kill "$pid"
done
rm pipe*
