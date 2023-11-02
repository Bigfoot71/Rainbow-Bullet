# Rainbow-Bullet
This is a small personal challenge that I impulsively undertook one evening. I developed it in about 5 hours, and then spent a few more hours preparing it for publication.

# Setup
You can clone the repository with raylib like this:
```console
git clone --recurse-submodules https://github.com/Bigfoot71/Rainbow-Bullet.git
```

Next, you can compile raylib directly into the project:
```console
make setup
```

Then, you can compile and launch the project as follows:
```console
make && ./RainbowBullet
```

You can also compile for `PLATFORM_WEB`:
```console
make clean setupClean                   # Only if you've previously built the project
make setup all PLATFORM=PLATFORM_WEB    # Compile raylib and the project for PLATFORM_WEB
```
