## Projects
- **overlay** it has been supposed that it will be supported 2 types of overlay: with software render and hardware render through OpenGL; but in fact it supports only software render only
- **goverlay** plugin intended to display SVG file over video through OpenGL; based on the example
- **test** application for debugging both plugins

Folder *resources* contains the SVG file used as overlay.

### overlay
Sink: RGBA  
Src: RGBA  
*Parameters:*
- **location** path to an image
- **type** overlay type, one of the followed values: software, gl, test
- **align** align position (0-8), started from the top-left coner

**Example**
>gst-launch-1.0 filesrc location=720.ogv ! decodebin ! videoconvert ! overlay type=software location=speedometer.svg ! autovideosink

### goverlay
Sink: RGBA  
Src: RGBA  
*Parameters:*
- **location** path to an image
- **align** align position (0-8), started from the top-left coner

**Example**
>gst-launch-1.0 filesrc location=720.ogv ! decodebin ! videoconvert ! glupload ! goverlay location=speedometer.svg align=6 ! glimagesink