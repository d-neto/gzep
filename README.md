# gzep
<img src="https://i.imgur.com/k4pQPU3.png" width="100%">

<p>A file compression application (with an original name) that uses Huffman coding and that's it. I'm sorry, don't take this too seriously please.</p>

## Usage
1. Clone the repository.

2. Build the binary program.
``` bash
cd ./gzep
make
```
3. Run
``` bash
./bin/gzep
$ > [Info] Usage
encode: ./bin/gzep <filepath> <?output>
decode: ./bin/gzep -d <filepath> <?output>
```
The file compression only works on texts with ASCII characaters.
