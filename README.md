# `xban`: hides the X mouse cursor on keypress

`xban` is a simple window manager agnostic tool that hides the X cursor when the
keyboard is used and reveals it again when the mouse moves.

If additional features are desired such as:
  - hiding the mouse cursor automatically after a period of inactivity
  - ignoring keypresses when a modifier is used or
  - moving the cursor to a specific location when the cursor is hidden

then refer to the more elaborate xbanish project which covers this:
https://github.com/jcs/xbanish

## Installation

```
$ git clone https://github.com/bakkeby/xban.git
$ cd xban
$ sudo make install
$ xban
```

This is also available in the [AUR](https://aur.archlinux.org/packages/xban).

## Dependencies

### Arch

- base-devel
- libxi
- libxfixes

```sh
sudo pacman -S base-devel libxi libxfixes
```

### Debian

- build-essential
- libxi-dev
- libxfixes-dev

```sh
sudo apt install build-essential libxi-dev libxfixes-dev
```

### Void

- base-devel
- libXi-devel
- libXfixes-devel

```sh
xbps-install -Su base-devel libXi-devel libXfixes-devel
```

## Credits

The work is inspired by [xbanish](https://github.com/jcs/xbanish).

## License

`xban` is available under an MIT license. See the `LICENSE` file.
