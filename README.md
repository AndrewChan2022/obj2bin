# obj2bin

version 2.0

.obj file to bin file 

```cpp
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t vert_count;
    uint32_t normal_count;
    uint32_t tri_count;
    
    float vertices[];
    float normals[];
    float indices[];
```

## build

unix: 
```bash
mkdir build && cd build
cmake ..
make -j12
sudo make install

# will install to /usr/local/bin"

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