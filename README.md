# crypticizer
A simple diary program with security in mind.

One of the crucial ideas here is that,
**no temporary file shall be on the hard drive**.
Now, this is mostly true so long as your temporary directory (for example, /tmp)
is on your RAM.
You can check this by issuing:
```
df -h | grep /tmp
```
If it says tmpfs, you are good!
Otherwise you don't get the RAM-loading security benefit
(you can still use it if you don't mind it.)

Another idea is that you would have complete control over
who has access to which part of the diary.
