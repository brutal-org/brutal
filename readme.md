# **BRUTAL**

[![Build, and Test](https://github.com/brutal-org/brutal/actions/workflows/main.yml/badge.svg)](https://github.com/brutal-org/brutal/actions/workflows/main.yml)
[![GitHub issues](https://img.shields.io/github/issues/brutal-org/brutal)](https://github.com/brutal-org/brutal/issues)
[![GitHub forks](https://img.shields.io/github/forks/brutal-org/brutal)](https://github.com/brutal-org/brutal/network)
[![GitHub stars](https://img.shields.io/github/stars/brutal-org/brutal)](https://github.com/brutal-org/brutal/stargazers)
[![GitHub license](https://img.shields.io/github/license/brutal-org/brutal)](https://github.com/brutal-org/brutal/blob/main/license)

<br/>
<br/>
<p align="center">
<img width="256" src="meta/assets/logo.png">
</p>
<br/>
<br/>
<p align="center">
Striking modernist shapes and bold use of modern C are the hallmarks of <b>BRUTAL</b>.<br>
<b>BRUTAL</b> combines the desire of UNIX utopia from the 1970s with modern technology and engineering.
</p>
<br/>
<br/>

<p align="center">
<img src="meta/screenshots/2021-06-20.png">
<br>
<b>BRUTAL</b> serial boot logs.
</p>


## About

**BRUTAL** is a micro-kernel base operating system targeting the x86_64 architecture. ARM and Risc-V support is planned.

## Roadmap

Right now the main goal of brutal is to run doom.

See the [roadmap](book/01-02-roadmap.md) file for more information on how we plan to get there and where you can contribute.

## Components

### Kernel

**BRUTAL** is based on a micro-kernel

### Library

**BRUTAL** feature a rich C library with custom allocators, data structures and algorithms.

Type-safe printf:

```c
log("the answer to {} the {} and {} is... {}", "life", "universe", "everything", 42);

> the answer to life the universe and everything is 42
```

Type-generic containers:

```c
typedef struct {int x; int y;} Point;
typedef Vec(Point) PointVec;

PointVec v;
vec_init(&v);
vec_push(&v, (Point){16, 32});

Point p;
vec_pop(&v, &p);

vec_deinit(&v);
```

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

See [credits.md](book/credits.md)

## License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="meta/assets/mit-license.png" />
</a>

The brutal operating system and its core components are licensed under the **MIT License**.

The full text of the license can be accessed via [this link](https://opensource.org/licenses/MIT) and is also included in the [license](License) file of this software package.
