<br/>
<br/>
<p align="center">
<img width="256" src="meta/assets/logo.png">
</p>
<br/>
<p align="center">
Striking modernist shapes and bold use of modern C are the hallmarks of <b>BRUTAL</b>.<br>
<b>BRUTAL</b> combine the desire of UNIX utopia from the 1970s with modern technology and engineering.
</p>
<br/>

## About

**BRUTAL** is a micro-kernel base operating system targeting the x86_64 architecture. ARM and Risc-V support is planned.

## Roadmap

Right now the main goal of brutal is to run doom.

See the [roadmap](doc/roadmap.md) file for more information on how we plan to get there and where you can contribute.

## Modern C

Brutal is written in a dialect of C called modern C characterize by the following principle:

- Avoid pointers and prefer value semantic.
- The standard library is old and unsafe, **don't use it**.
- Don't be afraid of writing type generic code.

Here is more resources on the subject:

- [The modern C book](https://modernc.gforge.inria.fr/)
- [Modern C and What We Can Learn From It - Luca Sas](https://www.youtube.com/watch?v=QpAhX-gsHMs)
- [Episode 414: Jens Gustedt on Modern C](https://www.youtube.com/watch?v=xioxhMOx9t4)

## Resources

- [Intel® 64 and IA-32 Architectures Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [OS development wiki](https://wiki.osdev.org/Main_Page)
- [Portable Operating System Interface (Draft)](http://www.open-std.org/jtc1/sc22/open/n4217.pdf)
- [ISO C2X (Draft))](https://wg14.link/c2x)

## Acknowledgements

This project take inspirations and/or reuse code from the following projects.

See [credits.md](doc/credits.md)

## License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="meta/assets/mit-license.png" />
</a>

The brutal operating system and its core components are licensed under the **MIT License**.

The full text of the license can be accessed via [this link](https://opensource.org/licenses/MIT) and is also included in the [license](License) file of this software package.
