for i in {0..100} ; do ffmpeg -accurate_seek -ss `echo $i*60.0 | bc` -i 4k_bbb.mp4 -frames:v 1 4k_bbb_100_frames_$i.png ; done

find . -iname \*.png | parallel convert -verbose -resize 1920x1080 "{}" "../1080p_1sec/{}" \;