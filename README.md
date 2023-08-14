# another home automation system

[autohomestack.de](http://autohomestack.de)

this repo: [https://github.com/alexkratzer/autohomestack](https://github.com/alexkratzer/autohomestack)

## usage MkDocs

* install poetry, than `poetry install`
* `poetry run mkdocs serve` - Start the live-reloading docs server.
* `poetry run mkdocs build` - Build the documentation site.
* [http://127.0.0.1:8000/autohomestack/](http://127.0.0.1:8000/autohomestack/) to view it in browser

## Deploy to Github pages

* `poetry run mkdocs gh-deploy`  Deploy your documentation to GitHub Pages
* [https://alexkratzer.github.io/autohomestack/](https://alexkratzer.github.io/autohomestack/)
~~[https://autohomestack.github.io/website/](https://autohomestack.github.io/website/)~~

## Deploy to s3 bucket

* `aws s3 sync ./site s3://autohomestack.de`
* [autohomestack.de](http://autohomestack.de)
