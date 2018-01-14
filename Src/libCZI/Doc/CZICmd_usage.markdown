CZIcmd Documentation                 {#naczircmd}
=======================

The console application "CZIcmd" is provided for two purposes:
- demonstrate the usage of the libCZI
- it might be useful to convert/extract data from a CZI-file

The parameters of the program are:

	usage: CZIcmd -c COMMAND -s SOURCEFILE -o OUTPUTFILE [-p PLANECOORDINATE]
					 [-r ROI] [-d DISPLAYSETTINGS] [-h] [-b] [-t] [-j DECODERNAME] 
					 [-v VERBOSITYLEVEL] [-y PYRAMIDINFO] [-z ZOOM] [-i INFOLEVEL]
					 [-e SELECTION] [-f FILTER]

	  using libCZI version 0.2

	  -?, --help          Show this help message and exit.
	  -c COMMAND, --command COMMAND
						  COMMAND can be any of 'PrintInformation',
						  'ExtractSubBlock', 'SingleChannelTileAccessor',
						  'ChannelComposite', 'SingleChannelPyramidTileAccessor',
						  'SingleChannelScalingTileAccessor',
						  'ScalingChannelComposite' and 'ExtractAttachment'.
						  'PrintInformation' will print information about the
						  CZI-file to the console. The argument 'info-level' can be
						  used to specify which information is to be printed.
						  'ExtractSubBlock' will write the bitmap contained in the
						  specified sub-block to the OUTPUTFILE.
						  'ChannelComposite' will create a channel-composite of the
						  specified region and plane and apply display-settings to
						  it. The resulting bitmap will be written to the specified
						  OUTPUTFILE.
						  'SingleChannelTileAccessor' will create a tile-composite
						  (only from sub-blocks on pyramid-layer 0) of the
						  specified region and plane. The resulting bitmap will be
						  written to the specified OUTPUTFILE.
						  'SingleChannelPyramidTileAccessor' adds to the previous
						  command the ability to explictely address a specific
						  pyramid-layer (which must exist in the CZI-document).
						  'SingleChannelScalingTileAccessor' gets the specified
						  region with an arbitrary zoom factor. It uses the
						  pyramid-layers in the CZI-document and scales the bitmap
						  if neccessary. The resulting bitmap will be written to
						  the specified OUTPUTFILE.
						  'ScalingChannelComposite' operates like the previous
						  command, but in addition gets all channels and creates a
						  multi-channel-composite from them using display-settings.
						  'ExtractAttachment' allows to extract (and save to a
						  file) the contents of attachments.
	  -s SOURCEFILE, --source SOURCEFILE
						  SOURCEFILE specifies the source CZI-file.
	  -o OUTPUTFILE, --output OUTPUTFILE
						  OUTPUTFILE specifies the output-filename. A suffix will
						  be appended to the name given here depending on the type
						  of the file.
	  -p PLANE-COORDINATES, --plane-coordinate PLANE-COORDINATES
						  Uniquely select a 2D-plane from the document. It is given
						  in the form [DimChar][number], where 'DimChar' specifies
						  a dimension and can be any of 'Z', 'C', 'T', 'R', 'I',
						  'H', 'V' or 'B'. 'number' is an integer.
						  Examples: C1T3, C0T-2, C1T44Z15H1.
	  -r ROI, --rect ROI
						  Select a paraxial rectangular region as the
						  region-of-interest. The coordinates may be given either
						  absolute or relative. If using relative coordinates, they
						  are relative to what is determined as the upper-left
						  point in the document.
						  Relative coordinates are specified with the syntax
						  'rel([x],[y],[width],[height])', absolute coordinates are
						  specified 'abs([x],[y],[width],[height])'.
						  Examples: rel(0,0,1024,1024), rel(-100,-100,500,500),
						  abs(-230,100,800,800).
	  -d DISPLAYSETTINGS, --display-settings DISPLAYSETTINGS
						  Specifies the display-settings used for creating a
						  channel-composite. The data is given in JSON-notation.
	  -h, --calc-hash     Calculate a hash for the output-picture. The
						  MD5Sum-algorithm is used for this.
	  -t, --drawtileboundaries
	  -j DECODERNAME, --jpgxrcodec DECODERNAME
						  Choose which decoder implementation is used. Specifying
						  "WIC" will request the Windows-provided decoder - which
						  is only available on Windows. By default the internal
						  JPG-XR-decoder is used.
	  -v VERBOSITYLEVEL, --verbosity VERBOSITYLEVEL
						  Set the verbosity of this program. The argument is a
						  comma- or semicolon-separated list of the following
						  strings: 'All', 'Errors', 'Warnings', 'Infos', 'Errors1',
						  'Warnings1', 'Infos1', 'Errors2', 'Warnings2', 'Infos2'.
	  -b, --background    Draw a one-pixel black line around each tile.
	  -y PYRAMIDINFO, --pyramidinfo PYRAMIDINFO
						  For the command 'SingleChannelPyramidTileAccessor' the
						  argument PYRAMIDINFO specifies the pyramid layer. It
						  consists of two integers (separated by a comma, semicolon
						  or pipe-symbol), where the first specifies the
						  minification-factor (between pyramid-layers) and the
						  second the pyramid-layer (starting with 0 for the layer
						  with the highest resolution).
	  -z ZOOM, --zoom ZOOM
						  The zoom-factor (which is used for the commands
						  'SingleChannelScalingTileAccessor' and
						  'ScalingChannelComposite'). It is a float between 0 and
						  1.
	  -i INFO-LEVEL, --info-level INFO-LEVEL
						  When using the command 'PrintInformation' the INFO-LEVEL
						  can be used to specify which information is printed.
						  Possible values are "Statistics", "RawXML",
						  "DisplaySettings", "DisplaySettingsJson", "AllSubBlocks",
						  "Attachments", "AllAttachments", ""PyramidStatistics" and 
						  "All". The values are given as a list separated by comma 
						  or semicolon.
	  -e SELECTION, --selection SELECTION
						  For the command 'ExtractAttachment' this allows to
						  specify a subset which is to be extracted (and saved to a
						  file). It is possible to specify the name and the index -
						  only attachments for which the name/index is equal to
						  those values specified are processed. The arguments are
						  given in JSON-notation, e.g. {"name":"Thumbnail"} or
						  {"index":3.0}.
	  -f FILTER, --tile-filter FILTER
						  Specify to filter subblocks according to the scene-index.
						  A comma seperated list of either an interval or a single
						  integer may be given here, e.g. "2,3" or "2-4,6" or
						  "0-3,5-8".


The above text is printed if the program is executed with the argument '-?' or '\--help':

    CZIcmd --help

The program expects the argument '-c' or '\--command' in order to select between different operations. The command choosen then determines
which other arguments have to be given for proper operation or are meaningful.


## command 'PrintInformation'

If the command 'PrintInformation' is specified, the program expects a source CZI-file to present (specified with '-s' or '\--source'). It will then
print out some information about the content of the CZI-file on stdout - as shown here:

	>CZIcmd.exe --command PrintInformation --source D:\PICTURES\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi

	SubBlock-Statistics
	-------------------

	SubBlock - Count: 122720

	Bounding-Box:
	 X=0 Y=0 W=139756 H=103036

	M-Index: min=0 max=140

	Bounds:
	 C -> Start=0 Size=4
	 T -> Start=0 Size=1
	 S -> Start=0 Size=22
	 B -> Start=0 Size=1

	Bounding-Box for scenes:
	 Scene0 : X=23894 Y=1840 W=22240 H=18592
	 Scene1 : X=2 Y=0 W=22240 H=24096
	 Scene2 : X=47755 Y=3676 W=18592 H=18592
	 Scene3 : X=71630 Y=3676 W=24096 H=22240
	 Scene4 : X=97334 Y=5512 W=22240 H=20416
	 Scene5 : X=22058 Y=25708 W=24096 H=20416
	 Scene6 : X=13 Y=27544 W=20416 H=20416
	 Scene7 : X=66122 Y=27544 W=25920 H=22240
	 Scene8 : X=47762 Y=33052 W=16736 H=14912
	 Scene9 : X=93662 Y=29380 W=25920 H=22240
	 Scene10 : X=119361 Y=31216 W=20416 H=20416
	 Scene11 : X=22058 Y=49576 W=22240 H=24096
	 Scene12 : X=0 Y=51412 W=20416 H=22241
	 Scene13 : X=45922 Y=51412 W=24096 H=24096
	 Scene14 : X=67958 Y=51412 W=24096 H=25920
	 Scene15 : X=91808 Y=53248 W=25945 H=22240
	 Scene16 : X=117524 Y=55084 W=20416 H=25920
	 Scene17 : X=26 Y=75280 W=20416 H=20420
	 Scene18 : X=22058 Y=75280 W=24096 H=24096
	 Scene19 : X=45926 Y=77116 W=22240 H=20416
	 Scene20 : X=67958 Y=77116 W=20416 H=22240
	 Scene21 : X=89976 Y=78952 W=24096 H=22240
	 Scene22 : X=115693 Y=82624 W=20416 H=20416

The argument '-i' or '\--info-level' is used to choose between different types of output (where only "Statistics" is default). So we get additional information
about the display-settings by running this command:

	>CZIcmd.exe --command PrintInformation --info-level Statistics,DisplaySettings --source D:\PICTURES\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi 
	SubBlock-Statistics
	-------------------

	SubBlock - Count: 122720

	Bounding-Box:
	 X=0 Y=0 W=139756 H=103036

	M-Index: min=0 max=140

	Bounds:
	 C -> Start=0 Size=5
	 T -> Start=0 Size=1
	 S -> Start=0 Size=23
	 B -> Start=0 Size=1

	Bounding-Box for scenes:
	 Scene0 : X=23894 Y=1840 W=22240 H=18592
	 Scene1 : X=2 Y=0 W=22240 H=24096
	 Scene2 : X=47755 Y=3676 W=18592 H=18592
	 Scene3 : X=71630 Y=3676 W=24096 H=22240
	 Scene4 : X=97334 Y=5512 W=22240 H=20416
	 Scene5 : X=22058 Y=25708 W=24096 H=20416
	 Scene6 : X=13 Y=27544 W=20416 H=20416
	 Scene7 : X=66122 Y=27544 W=25920 H=22240
	 Scene8 : X=47762 Y=33052 W=16736 H=14912
	 Scene9 : X=93662 Y=29380 W=25920 H=22240
	 Scene10 : X=119361 Y=31216 W=20416 H=20416
	 Scene11 : X=22058 Y=49576 W=22240 H=24096
	 Scene12 : X=0 Y=51412 W=20416 H=22241
	 Scene13 : X=45922 Y=51412 W=24096 H=24096
	 Scene14 : X=67958 Y=51412 W=24096 H=25920
	 Scene15 : X=91808 Y=53248 W=25945 H=22240
	 Scene16 : X=117524 Y=55084 W=20416 H=25920
	 Scene17 : X=26 Y=75280 W=20416 H=20420
	 Scene18 : X=22058 Y=75280 W=24096 H=24096
	 Scene19 : X=45926 Y=77116 W=22240 H=20416
	 Scene20 : X=67958 Y=77116 W=20416 H=22240
	 Scene21 : X=89976 Y=78952 W=24096 H=22240
	 Scene22 : X=115693 Y=82624 W=20416 H=20416

	Display-Settings
	----------------

	Channel #0
	==========
	 Enabled: yes
	 Tinting: yes (R=0, G=0, B=255)
	 Black-point: 7.82071e-05  White-point: 0.0180172
	 Gradation-curve-mode: linear

	Channel #1
	==========
	 Enabled: yes
	 Tinting: yes (R=255, G=0, B=0)
	 Black-point: 0  White-point: 0.812503
	 Gradation-curve-mode: linear

	Channel #2
	==========
	 Enabled: yes
	 Tinting: yes (R=0, G=255, B=0)
	 Black-point: 7.99274e-05  White-point: 0.570345
	 Gradation-curve-mode: linear

	Channel #3
	==========
	 Enabled: yes
	 Tinting: yes (R=255, G=255, B=0)
	 Black-point: 7.99212e-05  White-point: 0.700039
	 Gradation-curve-mode: linear

	Channel #4
	==========
	 Enabled: yes
	 Tinting: yes (R=0, G=255, B=255)
	 Black-point: 7.97113e-05  White-point: 0.220098
	 Gradation-curve-mode: linear


The info-level 'PyramidStatistics' is used to print out information about the number of subblocks in the various pyramid-layers:
	
	>CZIcmd.exe --info-level PyramidStatistics  --command PrintInformation --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi

	Pyramid-Subblock-Statistics
	---------------------------

	scene#0:
	 number of subblocks with scale 1/32: 5
	 number of subblocks with scale 1/16: 20
	 number of subblocks with scale 1/8: 45
	 number of subblocks with scale 1/4: 140
	 number of subblocks with scale 1/2: 505
	 number of subblocks with scale 1/1: 485

	scene#1:
	 number of subblocks with scale 1/32: 5
	 number of subblocks with scale 1/16: 20
	 number of subblocks with scale 1/8: 45
	 number of subblocks with scale 1/4: 165
	 number of subblocks with scale 1/2: 600
	 number of subblocks with scale 1/1: 630

	scene#2:
	 number of subblocks with scale 1/32: 5
	 number of subblocks with scale 1/16: 15
	 number of subblocks with scale 1/8: 40
	 number of subblocks with scale 1/4: 120
	 number of subblocks with scale 1/2: 415
	 number of subblocks with scale 1/1: 355

	scene#3:
	 number of subblocks with scale 1/32: 5
	 number of subblocks with scale 1/16: 20
	 number of subblocks with scale 1/8: 45
	 number of subblocks with scale 1/4: 160
	 number of subblocks with scale 1/2: 595
	 number of subblocks with scale 1/1: 640

	scene#4:
	 number of subblocks with scale 1/32: 5
	 number of subblocks with scale 1/16: 20
	 number of subblocks with scale 1/8: 45
	 number of subblocks with scale 1/4: 140
	 number of subblocks with scale 1/2: 525
	 number of subblocks with scale 1/1: 540

	scene#5:
	 number of subblocks with scale 1/32: 5
	 number of subblocks with scale 1/16: 20
	 number of subblocks with scale 1/8: 45
	...


If 'RawXML' is specified as argument for '-i' or '\--info-level', the complete metadata is written to stdout as XML.

'DisplaySettingsJson' will output the display-settings in JSON-notation as it is used in CZICmd.

	>CZIcmd.exe --info-level DisplaySettingsJson  --command PrintInformation --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi

	Display-Settings in CZIcmd-JSON-Format
	--------------------------------------


	Pretty-Print:
	{
		"channels": [
			{
				"ch": 0,
				"black-point": 0.00007820712198736146,
				"white-point": 0.01801724173128605,
				"tinting": "#0000ff"
			},
			{
				"ch": 1,
				"black-point": 0.0,
				"white-point": 0.8125027418136597,
				"tinting": "#ff0000"
			},
			{
				"ch": 2,
				"black-point": 0.00007992737664608285,
				"white-point": 0.570344865322113,
				"tinting": "#00ff00"
			},
			{
				"ch": 3,
				"black-point": 0.00007992124301381409,
				"white-point": 0.7000391483306885,
				"tinting": "#ffff00"
			},
			{
				"ch": 4,
				"black-point": 0.00007971125887706876,
				"white-point": 0.2200983464717865,
				"tinting": "#00ffff"
			}
		]
	}

	Compact:
	{"channels":[{"ch":0,"black-point":0.00007820712198736146,"white-point":0.018017
	24173128605,"tinting":"#0000ff"},{"ch":1,"black-point":0.0,"white-point":0.81250
	27418136597,"tinting":"#ff0000"},{"ch":2,"black-point":0.00007992737664608285,"w
	hite-point":0.570344865322113,"tinting":"#00ff00"},{"ch":3,"black-point":0.00007
	992124301381409,"white-point":0.7000391483306885,"tinting":"#ffff00"},{"ch":4,"b
	lack-point":0.00007971125887706876,"white-point":0.2200983464717865,"tinting":"#
	00ffff"}]}

'Attachments' and 'AllAttachments' are used to get information about the attachments contained in the CZI-file:

	>CZIcmd.exe --info-level Attachments  --command PrintInformation --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi

	Attachment Info
	---------------

	count | name
	------+----------------------------
		1 | EventList
		1 | Label
		1 | SlidePreview
		1 | Thumbnail
		1 | TimeStamps

In this case we get a list of the attachments present in the file, aggregated by their name (and how many times an attachment with a specific name is present).

	>CZIcmd.exe --info-level AllAttachments  --command PrintInformation --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi

	Complete list of Attachments
	----------------------------

	index | filetype | GUID                                   | name
	------+----------+----------------------------------------+-------------
		0 | CZTIMS   | {D2FD4125-CBF0-4B27-A8F2-643EDC5BAE7B} | TimeStamps
		1 | CZEVL    | {725AE927-5D00-4EBC-BB61-9362207F1B5D} | EventList
		2 | CZI      | {45165480-EEB9-417E-BC72-877E9A37EDAE} | Label
		3 | CZI      | {DD9A366F-4941-45B9-94FD-6043B1B96C16} | SlidePreview
		4 | JPG      | {7B25A072-7E33-4D2B-8921-DE69D09A3127} | Thumbnail

Here we get the complete list of all attachments. 



## command 'SingleChannelTileAccessor'

The command 'SingleChannelTileAccessor' will use an accessor of type `SingleChannelTileAccessor` (cf. [accessors](@ref accessors)). It will use the argument
'-p' or '\--plane-coordinate' in order to specify the plane, and the argument '-r' or '\--rect' in order to specify a rectangular (and axis-aligned) region (or ROI).
The pixel-type of the output is determined automatically (cf. [ISingleChannelTileAccessor](@ref libCZI::ISingleChannelTileAccessor)).

The following sample will extract the ROI (x=21300,y=21000,w=4096,h=4096) from channel #0

	CZIcmd.exe --command SingleChannelTileAccessor --plane-coordinate C0 --rect rel(21300,21000,4096,4096) --source D:\PICTURES\2014_02_05__16_39__0020-2.czi --output d:\PICTURES\Out\Output_2014_02_05__16_39__0020-2

and write out the result to a PNG-file with name `d:\PICTURES\Out\Output_2014_02_05__16_39__0020-2.PNG`.

The following arguments are meaningful for this command: 
- '-p' or '\--plane-coordinate'
- '-r' or '\--rect'
- '-j' or '\--jpgxrcodec'
- '-b' or '\--background'

## command 'ChannelComposite'

The command 'ChannelComposite' operates similar to 'SingleChannelTileAccessor', but in addition gathers the tile-composites from all channels, applies display-settings
and creates a multi-channel-composite. Therefore, the argument to '\--plane-coordinate' does not contain a C-coordinate (all channels which are marked 'active' in the
display-settings will be processed). The display-settings are either given on the commandline with the argument '-d' or '\--display-settings', or if this argument is
not given, then they are retrieved from the CZI-document's metadata.

The following sample will create a PNG-file (with name `d:\PICTURES\Out\Output_DCV_30MB.PNG`) just like in the ZEN-2D-viewer (using the display-settings from the CZI-file):

	CZIcmd.exe --command ChannelComposite --plane-coordinate Z10 --rect rel(0,0,512,512)  --source D:\PICTURES\DCV_30MB.czi --output d:\PICTURES\Out\Output_DCV_30MB


![3-channel_image_in_ZEN](ZEN_screenshot_2.PNG "3-channel document in ZEN")
@image latex ZEN_screenshot_2.PNG "3-channel document in ZEN"


The following arguments are meaningful for this command: 
- '-p' or '\--plane-coordinate'
- '-r' or '\--rect'
- '-j' or '\--jpgxrcodec'
- '-b' or '\--background'
- '-d' or '\--display-settings'
- '-f' or '\--tile-filter'

In order to specify the display-settings on the commandline, a JSON-notation is used - an example is shown here:

	{
	  "channels": [
		{
		  "ch": 0,
		  "tinting": "#41ff00",
		  "weight": 1,
		  "black-point": 0,
		  "white-point": 0.15259021896696423
		},
		{
		  "ch": 1,
		  "tinting": "#ffd200",
		  "weight": 1,
		  "black-point": 0,
		  "white-point": 0.15259021896696423
		},
		{
		  "ch": 2,
		  "tinting": "#4178ff",
		  "weight": 1,
		  "black-point": 0,
		  "white-point": 0.15259021896696423
		}
	  ]
	}

It is an array "channels", where the following fields are possible:

field name  |  type   | explanation
----------- | ------- | ---------------------------------
ch          | integer | the channel-index for which this channel-display-settings applies to
black-point | number  | the black-point (default: 0)
white-point | number  | the white-point (default: 1)
tinting     | string  | gives the RGB24-color as a 6-digit hexadecimal number, must start with a hash ('#')
gamma       | number  | the gradation curve is defined by a gamma with the value given here
splinelut   | array of numbers | the gradation curve is defined a spline, and the list of numbers are the x and y coordinates of the control-points of a spline

Note that 'gamma' and 'splinelut' are mutually exclusive, if both happen to be present, then 'gamma' takes precedence.

An example for a display-setting when specifying a spline is:

	{
	  "channels": [
		{
		  "ch": 0,
		  "splinelut": [
			0.362559241706161,	0.876190476190476,
			0.554502369668246,	0.561904761904762
		  ]
		}
	  ]
	}

Note that it is a flat list of numbers, where the first number is interpreted as X and the second as Y - and so on.

Passing this JSON on the commandline might be challenging, though - because many characters need to be escaped (depending on your command shell).

## command 'ExtractAttachment'

The command 'ExtractAttachment' allows to extract attachments and save them to a distinct file.

	>CZIcmd.exe --command ExtractAttachment --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi --output attachments

	>dir

	 Volume in drive D is DATA
	 Volume Serial Number is 74F9-A4A3

	 Directory of D:\TFSJBL\LibCZIDistrib\Src\Build\VS\Win32\Release\out

	05/11/2016  12:56 PM    <DIR>          .
	05/11/2016  12:56 PM    <DIR>          ..
	05/11/2016  12:56 PM               158 attachments_EventList_1.CZEVL
	05/11/2016  12:56 PM         2,887,520 attachments_Label_2.CZI
	05/11/2016  12:56 PM         6,744,768 attachments_SlidePreview_3.CZI
	05/11/2016  12:56 PM             3,904 attachments_Thumbnail_4.JPG
	05/11/2016  12:56 PM                16 attachments_TimeStamps_0.CZTIMS
				   5 File(s)      9,636,366 bytes
				   2 Dir(s)  519,149,056,000 bytes free

The filename of the attachments is composed from the \--output-argument, appending the name of the attachment and its id. The extension is given by the 'filetype'-field 
of the attachment.

In the above example, all attachments are processed. It is possible to filter the attachments - by giving the name or the index. This is specified with the \--selection-argument:

	>CZIcmd.exe --command ExtractAttachment --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi --output attachments --selection {\"name\":\"Thumbnail\"}

This will only save the attachments with 'name' = "Thumbnail".

	>CZIcmd.exe --command ExtractAttachment --source D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi --output attachments --selection {\"index\":1.0}

This will only save the attachments with id = 1.