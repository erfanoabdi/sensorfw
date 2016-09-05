sensorfw
========

Please use https://bugs.merproject.org to file bugs.

Debugging
---------

Minimal debugging output is generated by default. You can increase the output in different ways. Easiest is to increase debug level on the fly with the running sensorfwd. Output can be seen with
```
journalctl -af
```
Dynamically increase debug level from command line:
```
devel-su
kill -USR1 \`pgrep sensorfwd\`
twice for highest output level.
```
For status report:
```
devel-su
kill -USR2 \`pgrep sensorfwd\`
Static increase debugging level
```
If running from systemd, edit `/lib/systemd/system/sensorfwd.service` and change `--log-level=warning` to `--log-level=test` or do:
```
devel-su
sed -i -e 's/--log-level=warning/--log-level=test/' /lib/systemd/system/sensorfwd.service
```
then run
```
devel-su
systemctl daemon-reload
```
Then reboot or
```
systemctl restart sensorfwd
```
