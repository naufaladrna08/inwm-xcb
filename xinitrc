# xinitrc file for testing our window manager.

# 1. Start some programs to play with.
xeyes &
sleep 2 && xterm &

# 2. Start our window manager.
exec ./inwm