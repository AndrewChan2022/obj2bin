# obj2bin
.obj file to bin file {vert_count, tri_count, vertices, normals, indices}

## build

unix: 
```bash
mkdir build && cd build
cmake ..
make -j12
sudo make install

vi ~/.bashrc
# add line
export PATH="$PATH:/usr/local/obj2bin/bin"

```

windows:
```bash
mkdir build && cd build
cmake ..

# build install with msvc adminstrator mode

# add C:/Program Files/obj2bin/bin to path

```

## usage

```bash
# default output to xxx.bin
obj2bin xxx.obj

# explicitly set output
obj2bin xxx.obj -o xxx.bin
```