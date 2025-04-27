# obj2bin

.obj file to bin file 


version 1.0

```cpp
    uint32_t vert_count;
    uint32_t index_count;
    
    float vertices[];
    float normals[];    // same as vertices.size(),  cannot be zero size
    float indices[];
```



version 2.0

not compatible with previous version any more  

add version  
add normal_count so can be 0  



```cpp
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t vert_count;
    uint32_t normal_count;
    uint32_t tri_count;
    
    float vertices[];
    float normals[];        // can be zero size
    float indices[];
```

version 2.1.0

not compatible with previous version any more  

add magic number  
version add patch_version  
add face group  

```cpp
    char     magic[6] = "objbin";  // 6 bytes, not null-terminated
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t patch_version;

    uint32_t vert_count;
    uint32_t normal_count;
    uint32_t index_count;
    uint32_t group_name_count;
    uint32_t group_names_size;
    uint32_t face_count;
    
    float vertices[];
    float normals[];
    float indices[];
    struct {
        uint32_t    len;
        char        name[];       // not null-terminated
    } group_names[];
    uint32_t group_ids[];

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