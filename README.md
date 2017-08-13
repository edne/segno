# Segno

![segno](http://i.imgur.com/QnPEkWr.png)

## Dependencies

* `glfw3`
  * On Debian: `libglfw3-dev`
  * On ArchLinux: `glfw-x11`
* `guile`
  * On Debian: `guile-2.0-dev`
  * On ArchLinux: `guile`

And of course you need `gmake` and either `gcc` or `clang`.

## How to run

* `make info` will display building information
* `make segno` will compile the code
* `make run` will compile and run the code
* `make clean` will remove object files
* `make distclean` will remove object files and the executable binary

The default target is `all`, which is an alias to the `run` target.

## Usage

Work in progress, write code in the REPL, see [the example](example.scm) for
the syntax.

## License

This project is licensed under the GNU General Public License v3.0 - see the
[LICENSE](LICENSE) file for details.
