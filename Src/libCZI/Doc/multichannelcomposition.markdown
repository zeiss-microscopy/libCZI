Multi-channel-composition                 {#multichannelcomposition}
=========================

Multi-channel-composition is the operation of combining a set of channels (usually grayscale images) into a one colored image.  

The operation (as implemented by the function Compositors::ComposeMultiChannel) is controlled by the following options:

- tinting by a color
- definition of a black- and white-point
- a gradiation curve
  
The steps in the operation are:
  
    Let result R-G-B pixel value = 0,0,0
    For each channel  
		If tinting is enabled for this channel:
			- get normalized pixel value
			- apply gradation
			- multiply with R-G-B tinting color
			- add R-G-B value to result pixel 

		If tinting is disabled for this channel:
			- get normalized R-G-B value
			- apply gradation to R, G and B
			- add R-G-B value to result pixel 


The operation "Apply gradiation" works in the following way: the normalized pixel value is mapped to an integer (in the range 0..255) by looking
up a value:

![linear gradation curve](gradationcurve_1.PNG "gradation_curve_linear")
@image latex gradationcurve_1.PNG "linear gradation curve"

Above a linear gradation curve is shown. There are three ways commonly used to define the gradation curve:
- linear (a straight line from the point [black point,0] to [white point,255] - as shown above)
- gamma (instead of a straight line we use an exponential with the exponent gamma as a parameter)
- defined by a spline

In the function Compositors::ComposeMultiChannel the gradation curve is given as an array of bytes. Those bytes give the RGB-value at 
uniformly distributed points between black point and white point. Values between those points are interpolated linearly.

For example, the parameters white_point=0.2, black_point=1.0 and lookupTable={0, 66, 100, 166, 255} will result in this gradation curve:

![gradation curve sample](gradationcurve_2.PNG "gradation_curve_sample")
@image latex gradationcurve_2.PNG "gradation curve sample"

For generating a look-up-table (which then can be used for Compositors::ComposeMultiChannel) two utility functions are provided:
Utils::Create8BitLookUpTableFromSplines and Utils::Create8BitLookUpTableFromGamma.
 


		



