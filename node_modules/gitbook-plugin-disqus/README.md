Disqus integration for GitBook
==============

Install the Disqus plugin via **NPM**, which should be installed globally using the `-g` option:

```
$ npm install gitbook-plugin-disqus -g
```

> As you are installing globally, you may need to put `sudo` in front of the command, `sudo npm install gitbook-plugin-disqus -g`


To use the Disqus plugin in your Gitbook project, add the disqus plugin to the `book.json` file, along with your shortname (you create a shortname for disqus by creating a new website on the [disqus.com](https://disqus.com/) website)

```
{
    "plugins": ["disqus"],
    "pluginsConfig": {
        "disqus": {
            "shortName": "XXXXXXX"
        }
    }  
}
```

> The --plugins option to the Gitbook `serve` and `build` commands has been depreciated.

Thank you.
