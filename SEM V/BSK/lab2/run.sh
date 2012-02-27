#!/bin/bash
setsebool user_dmesg=on

checkmodule -M -m -o test.mod test.te
semodule_package -o test.pp -m test.mod
semodule -i test.pp
restorecon /var/log/messages
