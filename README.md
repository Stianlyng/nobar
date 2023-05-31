That's a great addition! Here's the updated README.md:

```markdown
# nobar

nobar is a lightweight, minimal X11 workspace manager designed specifically for users who don't feel the need for all the features of a regular topbar. It provides a simple visual indicator for the active workspace on your system. This makes it particularly useful for setups with small monitors, where screen real estate is at a premium.

## Features

- Provides a simple and efficient workspace indicator.
- Configurable color and height of the workspace indicator.
- Supports any number of workspaces.
- Designed with simplicity in mind, making it suitable for systems with small monitors or minimal desktop environments.

## Dependencies

- GCC (to build the program)
- X11 library (libX11)

## Building and Installing

1. Clone this repository:
    ```bash
    git clone https://github.com/Stianlyng/nobar.git
    ```
2. Navigate into the cloned directory:
    ```bash
    cd nobar
    ```
3. Compile the program with `make`:
    ```bash
    make
    ```
4. Install the program with `make install`:
    ```bash
    make install
    ```

## Usage

You can run `nobar` from any location by typing `nobar` in the terminal.

Command line options:
- `-h height` to set the height of the workspace indicator.
- `-c color` to set the color of the workspace indicator.

Example:
```bash
nobar -h 5 -c red
```

This will create a workspace indicator with a height of 5 pixels and a red color.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request or open an Issue.

## License

This project is licensed under the terms of the GPL license. See the [LICENSE](LICENSE) file for details.
```
