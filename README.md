# Undiskcopy: Turn Disk Copy 4.2 images into something you can use

A great deal of early Macintosh software was archived in [Apple Disk Copy
4.2 format](https://wiki.68kmla.org/DiskCopy_4.2_format_specification)
([archive.org link for when 68kmla is down](https://web.archive.org/web/20201028142058/https://wiki.68kmla.org/DiskCopy_4.2_format_specification)).
Undiskcopy converts those images into raw disk images that can be written
directly to media, e.g. to a floppy disk using `dd`.

## Building

Just run `make`.

## Usage

```
$ undiskcopy input-file.img output-file.img
$ dd if=output-file.img of=/path/to/floppy/device bs=512 conv=noerror,sync
```

**Warning**: the name of the device file that represents a USB floppy drive on
OS X (and likely other systems) can change. Make sure that the path you use
in the `dd` command's `of=` argument actually refers to the floppy drive and
not to something you wanted to keep!


## Compatibility

Undiskcopy should build and run on any somewhat POSIX-ish system that has a C99
compiler. It probably won't compile out of the box on Windows systems, but
porting should be trivial.

If the `file` command describes an image file as "Apple DiskCopy 4.2 image",
this tool should be able to convert it.

Tags are ignored if present. That's almost certainly not a problem for
Macintosh disks, but some sources indicate that tags could be essential on
the Lisa. I don't have a Lisa so I don't know whether Lisa images converted by
undiskcopy will work. See discussion in the format specification linked above
and at <https://macgui.com/news/article.php?t=482>.

## Support, or lack thereof

This software is a
[home-cooked meal](https://www.robinsloan.com/notes/home-cooked-app/).
I made it for me. If you find it useful, that's a happy side effect. I'm making
it available with **absolutely no warranty whatsoever** and no promise of
support, bug fixes, or future development of any kind. Use it at your own
risk.

You are welcome to report bugs, but I don't promise to fix them. I'm more
likely to act on bug reports that contain all the information I need to
reproduce the bug. In most cases that includes a copy of the disk image that
you were using when you encountered the bug.

## License

This software is licensed under the MIT License with Commons Clause restriction.
See LICENSE.md.
