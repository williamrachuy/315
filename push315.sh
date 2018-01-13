#!/usr/bin/bash
set username "williamrachuy"
set password "re7ained"

git add -A ~/315
git commit -m "commit"
git push
expect "Username for 'https://github.com': "
send "$username"
expect "Password for 'https://williamrachuy@github.com': "
send "$password"
