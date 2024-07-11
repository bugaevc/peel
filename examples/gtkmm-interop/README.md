# peel and gtkmm interoperability

This example demonstrates how you can use both peel and [gtkmm](https://www.gtkmm.org)
in the same program. This may be useful to incrementally port a program from
gtkmm to peel.

peel and gtkmm should generally be able to co-exist in the same program without
stepping on each other's toes. Furthermore, you may want to "bridge" between
peel and gtkmm types, for instance to use a peel-created widget in a
gtkmm-created window. This example shows the appropriate code to do so.

It is not currently possible to inherit a peel class from a gtkmm class, or
vice versa.
