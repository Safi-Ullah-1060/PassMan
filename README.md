# PassMan: Your friendly Password Manager :)

## oop-semproj-cs25b-Safi-Ullah-1060

This software enables you to manage all your `passwords` and `Tokens`.

## Installation
### Linux
- Dowload the `PassMan` binary from the assets in `Releases` page
### Windows
- Dowload the `PassMan.exe` binary from the assets in `Releases` page

## Manual Build
## Clone the Repo
- Clone the repo and `cd path/to/Repo-Root`
- Make build directory
```
mkdir build && cd build
```
- Generate Make files and make the binary

### Linux
```
cmake .. && make
./PassMan
```
### Windows
```
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-windows.cmake -DCMAKE_BUILD_TYPE=Release && make
.\PassMan.exe
```

## Features
- Adding users
- Organization has multiple users and one admin who manages all the users and their service data
    - although users within an organization can manage their data
- Data is encrypted and then saved on the system and never leaves it
- When a user logs in only his data is decrypted
    - so in case of any malware attack your login information and tokens are safe
- ofc there is our favourite TUI :)

## Future Plans
- I plan to also add support for SSH keys
- Adding organizations
- Windows releases are also planned

## PS: contributions/PRs are highly appreciated :)
