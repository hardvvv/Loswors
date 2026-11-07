# Converter to C Header

This Python script converts any file (image, binary, etc..) into a C/C++ header file (`.h`).
The file data is embedded as a byte array, allowing you to compile the file directly into your final program.


## How to run from the console
- Make sure Python 3 is installed
- Open a terminal in the script directory


### Run:
```bash
python b2h.py <input_file> <output_h_file> <array_name>
```

### Example:
```bash
python b2h.py image.png resource.h my_image
```


### Output example
The script:
- Reads a file in binary mode
- Converts its contents into a `constexpr unsigned char <array_name>` array
- Writes the result into a `.h .hpp` file
- Also generates a variable with the file size `constexpr unsigned int <array_name>_len`

```c
constexpr unsigned char my_image[] = { 0x89, 0x50, 0x4e, ... };
constexpr unsigned int my_image_len = 12345;
```

This is useful for embedding resources (images, fonts, etc..)
directly into a C or C++ app.
