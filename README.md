# EntityUnification

Code created for my bachelor thesis.
This application unifies two ontology sets really fast based on a scoring algorithm described in [1].

```
Usage Info:
"Either use a standard folder with a config file or  manually set all parameters."

Usage: ./GeoReaderMain [options]

"required options:"
--standard-folder [s]: <path to config.conf directory>
"or"
--filename-1 [1]: <filename>
--filename-2 [2]: <filename>
--scores [c]: <filename>

"optional options:"
--outputname [o]: <filename> --default: ./out.txt
--relation-map [r]: <filename>
--iterations [i]: <number>
--debug [d]
--generate-example-files [g]: <path>
"[g] generates examples for config.conf, relationmapping.conf and scores.conf at the target path."
```

[1]: http://ad-publications.informatik.uni-freiburg.de/theses/Bachelor_Anton_Stepan_2013.pdf
