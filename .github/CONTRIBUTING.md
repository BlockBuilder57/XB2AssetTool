## Contributing to XB2AssetTool

### Do's and Don'ts

- **Do** use clang-format to ensure your commit does not trample on code style.

- **Do** feel free to submit issues/PRs, we don't bite!
  - No, but seriously, if there's something you feel is missing, add it! We'd love to add your contribution to the project.

- **Do** keep any discussions on issues centered.

- **Don't** make a huge PR (25/50+ commits in one PR).
	- Instead, if it's getting that big, squash your changes to one commit or force push your branch forwards.

- **Don't** commit code that isn't yours, or that is NDA breaking. 
	Any NDA-breaking code will be immediately denied.
	- Note that *clean-room* RE is perfectly fine, if we can verify you did such.


### Commit Message Format
Commit messages are expected to be formatted like this.

```
[(Core | Build | UI in any combination)] Small summary here.

This is a bigger summary that describes all of the changes
in this commit. I can keep talking about this for all day.
It's pretty fun, isn't it.
```

An example of these formatting rules:

```
[Core/Build] Enhanced flux capacitor deviation

Tuned some values in the build system to allow Core to build faster.
It is unknown whether or not this affects other platforms.
```