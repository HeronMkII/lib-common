# lib-common is a common library
Every subsystem on the team will use this library. It will contain code for UART, CAN, SPI, and any other piece of code that is useful and universal.

# how to use this libary (without changing it)
This library is intended to be used as a submodule in other projects. To add this submodule to your specific subsystem project, do this:
'''
$ cd your/repo/path
$ git add submodule https://HeronMkII/lib-common
'''
This should add it into your repository. Now, **don't touch it**. Refer to next section if you want to make a change to lib-common. If you try to make changes from within your repository it probably will not work, and it might break something.
To pull the latest changes from the lib-common repo (if it was updated since the last time you used it), do this:
''' 
% cd your/repo/path
$ git submodule update --remote
'''
Now, the changes are made in your local directory. You still need to commit and push them:
'''
$ cd your/repo/path
$ git add lib-common
$ git commit
$ git push
'''
Now you have an updated lib-common in your repo.

# how to edit this library
You can clone this repo onto your computer and start making changes. Every time you push your changes, shit will change in all of the subsystem repos that use this as a submodule, so be smart. **If you're making anything more than a change in syntax, you have to create a pull request. Otherwise I will revert your commits**. After you clone lib-common to your computer, do this do make a pull request:
'''
git checkout -b your-branch-name-here
> make some changes
git push origin your-branch-name-here
'''
Then, go to github.com/HeronMkII/lib-common and find your branch, and create a pull request for it.

# references
because I don't actually know how to use git:
how to do a pull request: https://yangsu.github.io/pull-request-tutorial/
how to work with submodules: https://git-scm.com/book/en/v2/Git-Tools-Submodules

Cheers!
