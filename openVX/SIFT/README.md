> Author : Jin Heo

> E-mail : 993jin@gmail.com

OpenVX SIFT
=============
This is an application of SIFT for Intel Vision Algorithm Designer.

Kernels
-------------
1. vxGaussianPyramid
* Descripton : generate Gaussian images with different Sigma
* parameters
  * input parameters
    * 0 : Source Image
    * 1 : k value multiplied to Gaussian sigma (set k for each octave)
  * output parameters
    * 2 : lowest layer image with lowest sigma
    * 3 : multiply sqrt(2) to previous sigma
    * 4 : multiply sqrt(2) to previous sigma
    * 5 : multiply sqrt(2) to previous sigma
    * 6 : multiply sqrt(2) to previous sigma
-----------------------------------------------------
2. vxDog
* Descripton : get the DoG from blurred images
* parameters
  * input parameters
    * 0 : gaussian blurred image layer 0
    * 1 : gaussian blurred image layer 1
    * 2 : gaussian blurred image layer 2
    * 3 : gaussian blurred image layer 3
    * 4 : gaussian blurred image layer 4
  * output parameters
    * 5 : DoG image (layer 0 - layer 1)
    * 6 : DoG image (layer 1 - layer 2)
    * 7 : DoG image (layer 2 - layer 3)
    * 8 : DoG image (layer 3 - layer 4)
-----------------------------------------------------
3. vxKeypoint
* Descripton : Dogs for getting local extremas, and get the keypoints at the extremas.
* parameters
  * input parameters
    * 0 : DoG image (layer 0 - layer 1)
    * 1 : DoG image (layer 1 - layer 2)
    * 2 : DoG image (layer 2 - layer 3)
    * 3 : DoG image (layer 3 - layer 4)

    * 4 : gaussian blurred image layer 0
    * 5 : gaussian blurred image layer 1
    * 6 : gaussian blurred image layer 2
    * 7 : gaussian blurred image layer 3
    * 8 : gaussian blurred image layer 4
  * output parameters
    * 9 : key1 calculated by layer 0, 1, 2
    * 10 : key2 calculated by layer 2, 3, 4
-----------------------------------------------------
4. vxDescriptor
* Descripton : for each key, make descriptors
* parameters
  * input parameters
    * 0 : source image
    * 1 : sigma of the gaussian matrix for magnitudes
    * 2 : temp keys

  * output parameters
    * 3 : final keys
    * 4 : direction descriptor
    * 5 : magnitude descriptor
----------------------------------------------------
5. A example graph of the kernels
![Graph Example](https://github.com/sajjadt/vision_fpga_acceleration/blob/master/openVX/SIFT/graph.jpg "Sample Graph of SIFT with VAD")
----------------------------------------------------
6. The result
![The Result](https://github.com/sajjadt/vision_fpga_acceleration/blob/master/openVX/SIFT/example.jpg "The result of SIFT")

