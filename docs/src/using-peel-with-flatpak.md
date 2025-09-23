# Using peel with Flatpak

Many applications in the GNOME ecosystem are built and packaged using
[Flatpak]. Flatpak comes with a `flatpak-builder` tool, which builds an
application along with any of its bundled dependencies inside a Flatpak build
environment, according to a [manifest].

[Flatpak]: https://flatpak.org/
[manifest]: https://docs.flatpak.org/en/latest/manifests.html

To use peel in a your app, add peel to the list of _modules_ in your manifest:

```json
{
    "name": "peel",
    "buildsystem": "meson",
    "sources": [
        {
            "type": "git",
            "url": "https://gitlab.gnome.org/bugaevc/peel.git",
            "branch": "main"
        }
    ],
    "cleanup": ["*"]
}
```

This goes before your app itself. So your overall manifest might look somewhat
like this:

```json
{
    "id": "org.example.MyApp",
    "runtime": "org.gnome.Platform",

    ...

    "modules": [
        {
            "name": "peel",
            ...
        },
        {
           "name": "MyApp",
           ...
        }
    ]
}
```

Note that when using Meson to build your app, you should, preferably, still
include a wrap file into your repository, as described in
[Using peel with Meson]. This enables your app to be transparently built both
inside and outside of Flatpak.

[Using peel with Meson]: using-peel-with-meson.md
