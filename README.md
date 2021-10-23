# ejoy

`ejoy` is a stupid discord bot for wingcorp voice. It relies on C's weirdness to produce unpredictable comments when someone uploads a file, which is the primary feature. Also, it cycles expression matching `[ejoy]`.

The bot has been built on linux systems so far, but it should possible on windows, given that you have a C compiler and that you download and build the [`orca` library (which we use to interact with discord)](https://github.com/cee-studio/orca).

If you *are* on linux, simply run `install_dependencies.sh` to download, build and install `orca` to your `/usr/local/lib` folder.

Once dependencies are install, use `make` to build the bot.

You can then run the bot using `./ejoy <your bot token>`. 
I prefer to put my bot-token in a file called `bot.token`.

Happy hacking!

