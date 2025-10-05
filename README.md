# C++-Image-Processing-

Overview:

This project is a C++ Image Processing Tool that allows users to perform multiple operations on grayscale images (.pgm format).
It provides options such as flipping, rotating, cropping, scaling, filtering, contrast/brightness adjustment, and more — all through a simple text-based menu interface.
The project uses basic C++ (no advanced libraries or pointers for simplicity) and demonstrates concepts of file handling, 2D arrays (via vectors), and modular programming.

⚙️ Features:

 Basic Operations:
 
	Load and Save PGM Images
	Display Image (using IrfanView or macOS Preview)
	Horizontal and Vertical Flip
	Change Brightness (increase or decrease)
	Change Contrast (increase or decrease)
	Change Sharpness (increase or decrease)
 Rotation & Scaling:
 
	Rotate Right by 90°
	Rotate Left by 90°
	Rotate by Custom Angle (left or right)
	Scale Up (Zoom in)
	Scale Down (Zoom out)
 Cropping & Combination:
 
	Crop image (select a part of the image)
	Combine two images:
	Side-by-side
	Top-to-bottom
 Filters & Effects:
 
	Convert to Binary (Black & White)
	Detect Edges
	Apply Mean Filter (Smoothing)
	Apply Median Filter (Noise Removal)
	Apply Custom Linear Filter (loaded from linearfilter.txt)
How It Works:

The program uses a struct Image that stores image data in a 2D vector.
All transformations (flip, rotate, scale, etc.) are applied directly on pixel values.
After any modification, you need to save the image manually.
The menu options are loaded from a file named MainMenu.txt.

