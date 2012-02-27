#!/bin/bash
setsebool user_dmesg=on

checkmodule -M -m -o test2.mod test2.te
semodule_package -o test2.pp -m test2.mod -f test2.fc
semodule -i test2.pp
restorecon /var/log/messages
