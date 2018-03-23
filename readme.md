# imgdiff

### Usage
```
Usage: imgdiff [-b base_image] [-c highlight_color] [-o output_path] image1 image2
Options:
        -b [1|2] The image to use as base when generating a difference image
        -c The hex value of the color to use for highlighting differences
        -o The location where the difference image should be saved
        -q Disable json output
```

### Output
```
{
    "differences":[
        {
            "height":62,
            "width":100,
            "x":366,
            "y":216
        }
    ],
    "image1":"/path/to/image1",
    "image2":"/path/to/image2",
    "output":"/path/to/output",
    "threshold":85.0
}
```

#### Return codes
* 10 First image is invalid/in-accessible
* 20 Second image is invalid/in-accessible
* 30 Output image could not be saved

### Dependencies
* opencv (tested with 3.4)
* jsoncpp (tested with 1.8)