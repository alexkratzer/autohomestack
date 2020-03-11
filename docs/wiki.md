# some useful hints

## documentation

### usage MkDocs

* `mkdocs serve` - Start the live-reloading docs server.
* `mkdocs build` - Build the documentation site.
* `aws s3 sync ./site s3://autohomestack.de` Deploy to s3 bucket
* `mkdocs gh-deploy`  Deploy your documentation to GitHub Pages
* `localhost:8000` to view it in browser
* [https://autohomestack.github.io/website/](https://autohomestack.github.io/website/)

### draw.io

1. Open the XML (or create new one) with [https://www.draw.io/](https://www.draw.io/)

1. Make your changes

1. Export the Image as PNG and get the XML source

### plantuml

``` plantuml
@startuml
(*) --> "First Activity"
"First Activity" --> (*)
@enduml
```

* [plantuml examples](http://ogom.github.io/draw_uml/plantuml/)
* [plantuml examples](http://plantuml.com/sitemap-language-specification)
* [plantuml examples](https://real-world-plantuml.com)
* [online editor](https://www.planttext.com)

open **preview at VSC**

* Ctrl + Shift + N -> to open a new window
* point to a different root folder
* Ctrl + Shift + V -> open markdown preview mode
* Alt + D -> to render plantuml

## git bash

cmd|task
-|-
git checkout -b <branch_name>|make new feature branch
git checkout Pre_Master|merge feature branch to Pre_Master
git pull|merge feature branch to Pre_Master
git merge <branch_name>|merge feature branch to Pre_Master
git push --delete origin <branch_name>|delete feature branch
git branch -d <branch_name>|delete feature branch
