using libCZI                 {#using_naczirlib}
============

## Opening a CZI-file ##

The first thing you want to do with a CZI-file is to open it. This is done with the CZIReader-object.
An instance of a CZIReader is created with the function

~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::shared_ptr<ICZIReader> CreateCZIReader()
~~~~~~~~~~~~~~~~~~~~~

The CZIReader-object has the following methods

~~~~~~~~~~~~~~~~~~~~~{.cpp}
		virtual void Open(std::shared_ptr<IStream> stream);
		virtual std::shared_ptr<IMetadataSegment> ReadMetadataSegment();
		virtual std::shared_ptr<IAccessor> CreateAccessor(AccessorType accessorType);
		virtual void Close();

		// ... some omitted
		virtual SubBlockStatistics GetStatistics();
~~~~~~~~~~~~~~~~~~~~~

With the Open-method, the caller has to pass in an object which implements the interface "IStream". This object is used by the 
CZIReader in order to access the data in a CZI-file. It is just the abstraction of a random-access stream. libCZI includes
a (simple) implementation for reading a file which is constructed by passing in a filename:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::shared_ptr<IStream> CreateStreamFromFile(const wchar_t* szFilename)
~~~~~~~~~~~~~~~~~~~~~

When the Open-method has executed, the sub-block directory has been read and parsed, and the remaing methods can be called. This 
code can be used to open a CZI-file:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
	auto stream = libCZI::CreateStreamFromFile(LR"(D:\CZI\sample.czi)");
	auto cziReader = libCZI::CreateCZIReader();
	cziReader->Open(stream); 
~~~~~~~~~~~~~~~~~~~~~

The `SubBlockStatistics` gives information about the sub-blocks in the CZI-file. The coordinates of each sub-block are examined, and
the mininum and maximum are determined and are available in the struct returned by the method `ISubBlockRepository::GetStatistics()`.
It is usually important to examine the dimBounds member in order to determine which dimensions are used in the CZI-file.

## Reading sub-blocks in a CZI-file ##

All the sub-blocks in the CZI-file can be enumerated by the method `ISubBlockRepository::EnumerateSubBlocks`:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
virtual void EnumerateSubBlocks(std::function<bool(int index, const SubBlockInfo& info)> funcEnum) 
~~~~~~~~~~~~~~~~~~~~~

This code will print some of the information for all sub-blocks contained in a CZI-file:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
	auto stream = libCZI::CreateStreamFromFile(LR"(W:\Data\TestData\DCV_30MB.czi)");
	auto cziReader = libCZI::CreateCZIReader();
	cziReader->Open(stream);
	cziReader->EnumerateSubBlocks(
		[](int idx, const libCZI::SubBlockInfo& info)
	{
		cout << "Index " << idx << ": " << libCZI::Utils::DimCoordinateToString(&info.coordinate) << " Rect=" << info.logigalRect << endl;
		return true;
	});
~~~~~~~~~~~~~~~~~~~~~

It will print out something like:

	...
	Index 4: Z4C0T0 Rect=(0,0,512,512)
	Index 5: Z5C0T0 Rect=(0,0,512,512)
	Index 6: Z6C0T0 Rect=(0,0,512,512)
	Index 7: Z7C0T0 Rect=(0,0,512,512)
	Index 8: Z8C0T0 Rect=(0,0,512,512)
	Index 9: Z9C0T0 Rect=(0,0,512,512)
	Index 10: Z10C0T0 Rect=(0,0,512,512)
	Index 11: Z11C0T0 Rect=(0,0,512,512)
	Index 12: Z12C0T0 Rect=(0,0,512,512)
	Index 13: Z13C0T0 Rect=(0,0,512,512)
	Index 14: Z14C0T0 Rect=(0,0,512,512)
	...

The index argument we got here is used to identify a sub-block and can be used with the method `ISubBlockRepository::ReadSubBlock`. This code
will enumerate all sub-blocks found in a CZI-file, read them and save them to a PNG-file:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
	auto stream = libCZI::CreateStreamFromFile(LR"(W:\Data\TestData\DCV_30MB.czi)");
	auto cziReader = libCZI::CreateCZIReader();
	cziReader->Open(stream);
	cziReader->EnumerateSubBlocks(
		[&](int idx, const libCZI::SubBlockInfo& info)
	{
		auto sbBlk = cziReader->ReadSubBlock(idx);
		auto bitmap = sbBlk->CreateBitmap();
		wstring filename(L"SubBlock#");
		filename += to_wstring(idx);
		filename += L".PNG";
		SaveAsPng(filename, bitmap);
		return true;
	});
~~~~~~~~~~~~~~~~~~~~~

Note that the function `SaveAsPng` is not part of libCZI. It is also worth noting that the `ISubBlock::CreateBitmap` method will transparently
decode the bitmap (in case we have a compressed bitmap in the sub-block).

## creating multi-tile composites ##

This piece of code will extract a small rectangular region from a huge multi-tile document:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
	auto stream = libCZI::CreateStreamFromFile(LR"(D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi)");
	auto cziReader = libCZI::CreateCZIReader();
	cziReader->Open(stream);
	auto statistics = cziReader->GetStatistics();
	auto accessor = cziReader->CreateSingleChannelTileAccessor();
	libCZI::CDimCoordinate planeCoord{ { NaCZIr::DimensionIndex::C,1 } };	// the document only contains C-dimension
	auto multiTileComposit = accessor->Get(
		libCZI::IntRect{ statistics.boundingBox.x + 26152, statistics.boundingBox.y + 32215 ,3000,2200 },
		&planeCoord,
		nullptr);	// use default options
	SaveAsPng(LR"(D:\TileComposite.png)", multiTileComposit);
~~~~~~~~~~~~~~~~~~~~~

Note that we are using the `SubBlockStatistics` in order to specify a ROI with coordinates relative to the upper left corner of the bounding box.
The result is depicted here:

![single channel tile accessor](SingleChannelTileAccessor_3.PNG "multi-tile_accessor_3")
@image latex SingleChannelTileAccessor_3.PNG "single channel tile accessor"


Here is an example which leverages the SingleChannelScalingTileAccessor:

~~~~~~~~~~~~~~~~~~~~~{.cpp}
	auto stream = libCZI::CreateStreamFromFile(LR"(D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi)");
	auto cziReader = libCZI::CreateCZIReader();
	cziReader->Open(stream);
	auto statistics = cziReader->GetStatistics();
	auto accessor = cziReader->CreateSingleChannelScalingTileAccessor();
	NaCZIr::CDimCoordinate planeCoord{ { libCZI::DimensionIndex::C,1 } }; // the document only contains C-dimension, we choose channel#1
	auto multiTileComposit = accessor->Get(
		libCZI::IntRect{
				statistics.boundingBox.x + statistics.boundingBox.w / 4,
				statistics.boundingBox.y + statistics.boundingBox.h / 4 ,
				(statistics.boundingBox.w / 8) * 5,
				(statistics.boundingBox.h / 8) * 5 },
		&planeCoord,
		0.1f,
		nullptr);
	SaveAsPng(LR"(D:\ScalingTileComposite.png)", multiTileComposit);
~~~~~~~~~~~~~~~~~~~~~

The ROI is now x=\f$\frac{width}{4}\f$, y=\f$\frac{height}{4}\f$, w=\f$\frac{5}{8}\cdot width\f$, h=\f$\frac{5}{8}\cdot height\f$ - where \f$width\f$ and \f$height\f$
refer to the width and height of the bounding box of the document.  \n
The zoom is given as 0.1 - so the resulting document will have width=\f$0.1\times\frac{5}{8}\cdot width\f$ and height=width=\f$0.1\times\frac{5}{8}\cdot height\f$.

This operaton is depicted here:

![single channel scaling tile accessor](SingleChannelTileAccessor_4.PNG "multi-tile_accessor_4")
@image latex SingleChannelTileAccessor_4.PNG "single channel scaling tile accessor"

## creating a multi-channel composite ##

In order to create a colorful picture from a bunch of channels (usually grayscale), we need to apply a color to it - that's refered to as "tinting". 
Furthermore, we want to apply a gradation curve. All the required parameters for this are refered to as "display settings".
In a CZI-file we can find display settings in the metadata.  
The following sample is reading the display settings from the metadata; then we get a (scaled) multi-tile composite for each of
the channels (more exactly: only for those channels which are marked 'active' in the display settings). Those bitmaps are
then fed into a function which will produce the multi-channel-composite (according to the display settings).

~~~~~~~~~~~~~~~~~~~~~{.cpp}
	auto stream = libCZI::CreateStreamFromFile(LR"(D:\PICTURES\NaCZIrTestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi)");
	auto cziReader = libCZI::CreateCZIReader();
	cziReader->Open(stream);
	auto statistics = cziReader->GetStatistics();

	// get the display-setting from the document's metadata
	auto mds = cziReader->ReadMetadataSegment();
	auto md = mds->CreateMetaFromMetadataSegment();
	auto docInfo = md->GetDocumentInfo();
	auto dsplSettings = docInfo->GetDisplaySettings();
	
	libCZI::IntRect roi{
		statistics.boundingBox.x + statistics.boundingBox.w / 4,
		statistics.boundingBox.y + statistics.boundingBox.h / 4 ,
		(statistics.boundingBox.w / 8) * 5,	(statistics.boundingBox.h / 8) * 5 };

	// get the tile-composite for all channels (which are marked 'active' in the display-settings)
	std::vector<shared_ptr<libCZI::IBitmapData>> actvChBms;
	int index = 0;	// index counting only the active channels
	std::map<int, int> activeChNoToChIdx;	// we need to keep track which 'active channels" corresponds to which channel index
	auto accessor = cziReader->CreateSingleChannelScalingTileAccessor();
	libCZI::CDisplaySettingsHelper::EnumEnabledChannels(dsplSettings.get(),
		[&](int chIdx)->bool
	{
		libCZI::CDimCoordinate planeCoord{ { libCZI::DimensionIndex::C, chIdx } };
		actvChBms.emplace_back(accessor->Get(roi, &planeCoord, 0.05f, nullptr));
		activeChNoToChIdx[chIdx] = index++;
		return true;
	});
		
	// initialize the helper with the display-settings and provide the pixeltypes 
	// (for each active channel)
	libCZI::CDisplaySettingsHelper dsplHlp;
	dsplHlp.Initialize(dsplSettings.get(), 
		[&](int chIdx)->libCZI::PixelType { return actvChBms[activeChNoToChIdx[chIdx]]->GetPixelType(); });

	// pass the tile-composites we just created (and the display-settings for the those active 
	//  channels) into the multi-channel-composor-function
	auto mcComposite = libCZI::Compositors::ComposeMultiChannel_Bgr24(
		dsplHlp.GetActiveChannelsCount(),
		std::begin(actvChBms),
		dsplHlp.GetChannelInfosArray());

	SaveAsPng(LR"(D:\ScalingTileComposite_MultiChannelComposite.png)", mcComposite);
~~~~~~~~~~~~~~~~~~~~~

In this sample we used the same document and ROI as before. The CZI-file in this case contained 5 channels (and all being 'active'),
so we had to get 5 tile-composites (using the SingleChannelScalingTileAccessor), which are then all put 
into the `ComposeMultiChannel_Bgr24` function (alongside with the corresponding display-settings). The function will produce a new 
bitmap (always of pixeltype Bgr24) which then contains the multi-channel-composite image. In this process we leveraged a utility 
`CDisplaySettingsHelper` which hides some of the book-keeping (among other things, it helps sorting out the 'active' channels 
and it converts the display-settings we got from the document's metadata into the form expected by the `ComposeMultiChannel_Bgr24` function).


The complete operation is depicted here:

![multi-channel-composite from a (scaled) tile-composite](ScalingSingleChannelTileAccessor1.png "scaled_multi-channel-composite1")
@image latex ScalingSingleChannelTileAccessor1.png "multi-channel-composite from a (scaled) tile-composite"