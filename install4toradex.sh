#!/usr/bin/env bash

scp Application/lense   root@$1:/home/root/.

scp gst_plugin/.libs/liblensecorrection.so   root@$1:/usr/lib/gstreamer-0.10/.
