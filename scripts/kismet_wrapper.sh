#!/bin/bash
# Launch Kismet in logging-only mode for the background
# Output goes to the BlackBox/logs directory
kismet -c bb_wifi --override kismet_httpd.enabled=false --log-types kismet,pcapng --log-prefix ../logs/wardrive_
