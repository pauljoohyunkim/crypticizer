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

## Dependencies and Compilation
You should have **ncurses** library and **openssl** (version later than 3.0) library installed.
### Meson (Preferred)
For release build:
```
meson setup --buildtype release build
cd build
ninja
ninja install
```
For development:
```
meson setup --buildtype debug build     # Or just meson setup build
cd build
ninja
```
### CMake (Legacy)
```
mkdir build
cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release     # For development, use Debug instead of release
make
```
(Installation support for cmake will be added later?)

## Usage (TL;DR)
Start project by
```
mkdir my_diary
cd my_diary
crypticizer
```
Arrow keys (or h,j,k,l) for navigation.

'+' key for new entry.

Enter key for editing an existing entry.

F5 key for refreshing.

'q' key for quitting.

## Usage (Detailed)
To start a project, simply create an empty directory and navigate to it,
then run the program.
```
mkdir my_diary
cd my_diary
crypticizer
```
You will then be asked to set a (master) password of the project.
**(If you forget it, I cannot really help you...)**

On the first time running, it will create a directory called *.crypticizer*, where the password hash is stored.

After setting the password,
you will be asked to set a default text editor for the project.
Leaving this empty will set it to using vim (because you are a chad).

Then, you will see a blank screen with an empty box, and a smaller box with hints.

You can create a first entry by pressing '+' key,
which will create a temporary file in /tmp directory (most likely) with vim (at the moment) only if you have it installed. (The support for choosing text editor of your choice will be added later.)

When you exit vim ([How to exit vim](https://stackoverflow.com/questions/11828270/how-do-i-exit-vim)),
the file will automatically encrypted with your password with AEAD, and saved as *(epoch time).crpt*

The newly created file should now be on the list.
Press enter to edit it, which will decrypted to a temporary file in the ramdisk again.

You can navigate through the file via the arrow keys or h,j,k,l (vim binding).

You can refresh the list (if an entry that you expected does not show up) by hitting F5 key.

You can quit the program by pressing 'q'.

## Specifications
Master password is hashed by SHA512 with a salt.

Each time an entry is encrypted, it goes through the following process.
```
salt = generateSalt()
IV = generateIV()
expandedKey = SCRYPT(key, salt)
encryptedText = salt + IV + AEAD(IV, AES_256_GCM, plainText, expandedKey)
```
where AEAD is concatenation of ciphertext and tag generated.
