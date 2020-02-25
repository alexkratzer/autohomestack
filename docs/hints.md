# some useful hints

## usage MkDocs

* `mkdocs serve` - Start the live-reloading docs server.
* `mkdocs build` - Build the documentation site.
* `aws s3 sync ./site s3://autohomestack.de` Deploy to s3 bucket
* `mkdocs gh-deploy`  Deploy your documentation to GitHub Pages
* `localhost:8000` to view it in browser
* [https://autohomestack.github.io/website/](https://autohomestack.github.io/website/)

## web apps

* to draw some images: [https://www.draw.io/](https://www.draw.io/)

## node red

Initialising Variable

```javascript
var count=context.get('count') || 0;
var count2=context.get('count2') || 0;

// You can also use an object e.g
var local=context.get('data') || {};
if (local.count===undefined) //test exists
{
  local.count=0;
}
```
