//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017-2021  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#include "pch.h"
#include "inc_libCZI.h"
#include "MockMetadataSegment.h"
#include "utils.h"
#include <codecvt>
#include <locale>

using namespace libCZI;
using namespace std;

TEST(MetadataReading, Test1)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto docInfo = md->GetDocumentInfo();

	GeneralDocumentInfo generalDocInfo = docInfo->GetGeneralDocumentInfo();
	EXPECT_TRUE(generalDocInfo.userName_valid && generalDocInfo.userName == L"zeiss") << "userName not as expected.";
	EXPECT_TRUE(generalDocInfo.creationDateTime_valid && generalDocInfo.creationDateTime == L"2013-09-12T11:18:12.2152881-04:00") << "creationDateTime not as expected.";

	auto scalingInfo = docInfo->GetScalingInfo();

	EXPECT_DOUBLE_EQ(scalingInfo.scaleX, 1.6432520108980473e-07);
	EXPECT_DOUBLE_EQ(scalingInfo.scaleY, 1.6432520108980473e-07);

	vector<DimensionIndex> dimensions = docInfo->GetDimensions();
	EXPECT_TRUE(std::find(dimensions.cbegin(), dimensions.cend(), DimensionIndex::C) != dimensions.cend()) << "expected to find C-dimension";
	EXPECT_TRUE(std::find(dimensions.cbegin(), dimensions.cend(), DimensionIndex::T) != dimensions.cend()) << "expected to find T-dimension";
	EXPECT_TRUE(std::find(dimensions.cbegin(), dimensions.cend(), DimensionIndex::S) != dimensions.cend()) << "expected to find S-dimension";
	EXPECT_TRUE(std::find(dimensions.cbegin(), dimensions.cend(), DimensionIndex::B) != dimensions.cend()) << "expected to find B-dimension";
}

TEST(MetadataReading, ScalingInfoExTest)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	const auto docInfo = md->GetDocumentInfo();

    const auto scalingInfo = docInfo->GetScalingInfoEx();

	EXPECT_TRUE(scalingInfo.IsScaleXValid());
	EXPECT_DOUBLE_EQ(scalingInfo.scaleX, 1.6432520108980473e-07);
	EXPECT_TRUE(scalingInfo.IsScaleYValid());
	EXPECT_DOUBLE_EQ(scalingInfo.scaleY, 1.6432520108980473e-07);
	EXPECT_FALSE(scalingInfo.IsScaleZValid());

	EXPECT_TRUE(scalingInfo.defaultUnitFormatX == L"um");
	EXPECT_TRUE(scalingInfo.defaultUnitFormatY == L"um");
	EXPECT_TRUE(scalingInfo.defaultUnitFormatZ.empty());
}

TEST(MetadataReading, InvalidDataTest1)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::InvalidData);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_FALSE(md->IsXmlValid()) << "Expected invalid XML.";

	EXPECT_THROW(md->GetDocumentInfo(), LibCZIXmlParseException);

	EXPECT_THROW(md->GetXml(), LibCZIXmlParseException);
}

TEST(MetadataReading, InvalidPathTest1)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Document/UserNameX");
	EXPECT_TRUE(!node) << "Not expecting to find this node.";

	EXPECT_THROW(auto node = md->GetChildNodeReadonly("ImageDocument[dd]"), LibCZIMetadataException);
}

TEST(MetadataReading, Test2)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	wstring value;
	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Document/UserName");
	node->TryGetValue(&value);
	EXPECT_STREQ(value.c_str(), L"zeiss");

	node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/AcquisitionDuration");
	node->TryGetValue(&value);
	EXPECT_STREQ(value.c_str(), L"7761956.9586000005");
}

TEST(MetadataReading, Test3)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	wstring value;
	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/Dimensions/Channels/Channel[Id=Channel:1]");
	bool b = node->TryGetValue(&value);
	EXPECT_FALSE(b) << "not expecting a value for this node";

	node = node->GetChildNodeReadonly("DyeId");
	b = node->TryGetValue(&value);
	EXPECT_TRUE(b) << "expecting a value for this node";
	EXPECT_STREQ(value.c_str(), L"McNamara-Boswell-0434");
}

TEST(MetadataReading, Test4)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	auto cnt = md.use_count();
	EXPECT_EQ(cnt, 1);

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	wstring value;
	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/Dimensions/Channels/Channel[Id=Channel:1]");

	cnt = md.use_count();
	EXPECT_EQ(cnt, 2);

	bool b = node->TryGetValue(&value);
	EXPECT_FALSE(b) << "not expecting a value for this node";

	node.reset();
	EXPECT_EQ(md.use_count(), 1);

	md.reset();
	EXPECT_EQ(md.use_count(), 0);
}

TEST(MetadataReading, Test5)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/SizeX");
	EXPECT_TRUE(!!node) << "this node should exists";

	int32_t value;
	bool b = node->TryGetValueAsInt32(&value);
	EXPECT_TRUE(b && value == 139758) << "not the expected information";

	node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/PixelType");
	b = node->TryGetValueAsInt32(&value);
	EXPECT_FALSE(b) << "did not expected that we can get an int";

	node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/AcquisitionDuration");
	b = node->TryGetValueAsInt32(&value);
	EXPECT_FALSE(b) << "did not expected that we can get an int";
}

TEST(MetadataReading, Test6)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/Dimensions/Channels/Channel[Id=Channel:2]/ExposureTime");
	EXPECT_TRUE(!!node) << "this node should exists";

	uint64_t value;
	bool b = node->TryGetValueAsUInt64(&value);
	EXPECT_TRUE(b && value == 100000000) << "not the expected information";

	int64_t value2;
	b = node->TryGetValueAsInt64(&value2);
	EXPECT_TRUE(b && value2 == 100000000) << "not the expected information";
}

TEST(MetadataReading, Test7)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/AcquisitionDuration");
	EXPECT_TRUE(!!node) << "this node should exists";

	double value;
	bool b = node->TryGetValueAsDouble(&value);
	EXPECT_TRUE(b) << "not the expected information";
	EXPECT_DOUBLE_EQ(value, 7761956.9586000005) << "not the expected information";

	node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/AcquisitionDateAndTime");
	EXPECT_TRUE(!!node) << "this node should exists";

	b = node->TryGetValueAsDouble(&value);
	EXPECT_FALSE(b) << "this should not parse as float";
}

TEST(MetadataReading, Test8)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/HardwareSetting/MicroscopeManager/IsCondenserFocusingAvailable");
	EXPECT_TRUE(!!node) << "this node should exists";

	bool value;
	bool b = node->TryGetValueAsBool(&value);
	EXPECT_TRUE(b && value == false) << "not the expected information";

	node = md->GetChildNodeReadonly("ImageDocument/Metadata/HardwareSetting/MicroscopeManager/HasSoftKeyLabeling");
	EXPECT_TRUE(!!node) << "this node should exists";

	b = node->TryGetValueAsBool(&value);
	EXPECT_TRUE(b && value == true) << "not the expected information";
}

TEST(MetadataReading, Test9)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	ASSERT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/Dimensions/S/Scenes/Scene[Index=3]/RegionId");
	ASSERT_TRUE(!!node) << "this node should exists";

	uint64_t value;
	bool b = node->TryGetValueAsUInt64(&value);
	EXPECT_TRUE(b && value == 635145958921364056ULL) << "not the expected information";

	uint32_t value32;
	b = node->TryGetValueAsUInt32(&value32);
	EXPECT_FALSE(b) << "not the expected information";
}

TEST(MetadataReading, NonExistingTest)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto node = md->GetChildNodeReadonly("ImageDocument/MetadataX/Information/Image");

	EXPECT_TRUE(!node) << "the requested node should not be present";

	node = md->GetChildNodeReadonly("ImageDocument/Metadata/Information/Image/Dimensions/Channels/Channel[Id=Channel:X]");
	EXPECT_TRUE(!node) << "the requested node should not be present";
}

static void EnumAllRecursively(IXmlNodeRead* node, std::function<bool(std::shared_ptr<IXmlNodeRead>)> func)
{
	node->EnumChildren(
		[&](std::shared_ptr<IXmlNodeRead> n)->bool
	{
		if (!func(n))
		{
			return false;
		}

		EnumAllRecursively(n.get(), func);
		return true;
	});
}

TEST(MetadataReading, WalkChildrenTest1)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	vector<string> names;
	EnumAllRecursively(md.get(),
		[&](std::shared_ptr<IXmlNodeRead> n)->bool
	{
		names.push_back(utf8_conv.to_bytes(n->Name()));
		return true;
	});

	auto cnt = md.use_count();
	EXPECT_EQ(cnt, 1);

	uint8_t hash[16];
	CalcHash(hash, names.cbegin(), names.cend());

	static const uint8_t expectedHash[16] = { 0x46,0x3b,0x14,0xa7,0xfc,0xdb,0x39,0xcb,0x01,0x5e,0x31,0x7e,0x75,0xb2,0x50,0xff };
	EXPECT_TRUE(memcmp(hash, expectedHash, 16) == 0) << "hash-code of the result is not as expected";
}

TEST(MetadataReading, WalkChildrenTest2)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	vector<string> namesAndAttributes;
	EnumAllRecursively(md.get(),
		[&](std::shared_ptr<IXmlNodeRead> n)->bool
	{
		string s(utf8_conv.to_bytes(n->Name()));

		n->EnumAttributes(
			[&](const std::wstring& attribName, const std::wstring& attribValue)->bool
		{
			s += ":" + utf8_conv.to_bytes(attribName) + "=" + utf8_conv.to_bytes(attribValue);
			return true;
		});

		namesAndAttributes.push_back(s);
		return true;
	});

	auto cnt = md.use_count();
	EXPECT_EQ(cnt, 1);

	uint8_t hash[16];
	CalcHash(hash, namesAndAttributes.cbegin(), namesAndAttributes.cend());

	static const uint8_t expectedHash[16] = { 0x0a,	0x5a, 0x1a,	0xcd, 0xa0,	0x9b, 0x6d,	0x4d, 0xf1,	0xe7, 0x27,	0x7a, 0x8e,	0xcc, 0xe6,	0xb1 };
	EXPECT_TRUE(memcmp(hash, expectedHash, 16) == 0) << "hash-code of the result is not as expected";
}

TEST(MetadataReading, WalkChildrenTest3)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	vector<string> namesAndValue;
	EnumAllRecursively(md.get(),
		[&](std::shared_ptr<IXmlNodeRead> n)->bool
	{
		string s(utf8_conv.to_bytes(n->Name()));
		wstring value;
		if (n->TryGetValue(&value))
		{
			s += " -> " + utf8_conv.to_bytes(value);
		}

		namesAndValue.push_back(s);
		return true;
	});

	auto cnt = md.use_count();
	EXPECT_EQ(cnt, 1);

	uint8_t hash[16];
	CalcHash(hash, namesAndValue.cbegin(), namesAndValue.cend());

	static const uint8_t expectedHash[16] = { 0x35,0x84,0x1e,0x9a,0xdf,0xb6,0xce,0x1a,0x88,0x1f,0x7d,0x42,0x1e,0x36,0xd4,0x0f };
	EXPECT_TRUE(memcmp(hash, expectedHash, 16) == 0) << "hash-code of the result is not as expected";
}

TEST(MetadataReading, DimensionInfo1Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data3);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto docInfo = md->GetDocumentInfo();

	vector<DimensionIndex> dimIndices;
	docInfo->EnumDimensions([&](DimensionIndex dimInd)->bool {dimIndices.push_back(dimInd); return true; });

	auto dimInfo = docInfo->GetDimensionInfo(DimensionIndex::Z);

	int start, end;
	dimInfo->GetInterval(&start, &end);

	EXPECT_TRUE(start == 0 && end == 20) << "start or size was not as expected";

	auto dimZ = docInfo->GetDimensionZInfo();

	double offset, increment;
	bool b = dimZ->TryGetIntervalDefinition(&offset, &increment);
	ASSERT_TRUE(b) << "Was expecting an interval definition.";

	EXPECT_DOUBLE_EQ(offset, 0) << "Not the expected result.";
	EXPECT_DOUBLE_EQ(increment, 9.525) << "Not the expected result.";
}

TEST(MetadataReading, DimensionInfo2Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data4);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto docInfo = md->GetDocumentInfo();

	vector<DimensionIndex> dimIndices;
	docInfo->EnumDimensions([&](DimensionIndex dimInd)->bool {dimIndices.push_back(dimInd); return true; });

	auto dimInfo = docInfo->GetDimensionInfo(DimensionIndex::Z);

	int start, end;
	dimInfo->GetInterval(&start, &end);

	EXPECT_TRUE(start == 0 && end == 20) << "start or size was not as expected";

	auto dimZ = docInfo->GetDimensionZInfo();

	double offset, increment;
	bool b = dimZ->TryGetIntervalDefinition(&offset, &increment);
	ASSERT_FALSE(b) << "Was not expecting an interval definition.";

	vector<double> values;
	b = dimZ->TryGetPositionList(&values);
	ASSERT_TRUE(b) << "Was expecting a list definition.";

	vector<double> expected = { 0, 0.5, 1, 1.5 ,2, 2.5, 3, 3.8, 4.2 };
	ASSERT_TRUE(values.size() == expected.size()) << "Was expecting a list of 9 doubles.";

	for (size_t i = 0; i < values.size(); ++i)
	{
		EXPECT_DOUBLE_EQ(values[i], expected[i]) << "Not the expected result.";
	}

	double d;
	ASSERT_TRUE(dimZ->TryGetReferencePosition(&d)) << "Was expecting to have a start-position.";
	EXPECT_DOUBLE_EQ(d, -11.25) << "Not the expected result.";

	ASSERT_FALSE(dimZ->TryGetXyzHandedness(nullptr)) << "Was not expecting to get a true result here.";
	ASSERT_FALSE(dimZ->TryGetZAxisDirection(nullptr)) << "Was not expecting to get a true result here.";
	ASSERT_FALSE(dimZ->TryGetZDriveMode(nullptr)) << "Was not expecting to get a true result here.";
	ASSERT_FALSE(dimZ->TryZDriveSpeed(nullptr)) << "Was not expecting to get a true result here.";

	auto dimT = docInfo->GetDimensionTInfo();
	XmlDateTime startDateTime;
	b = dimT->TryGetStartTime(&startDateTime);
	ASSERT_TRUE(dimZ->TryGetReferencePosition(&d)) << "Was expecting to have a start-time.";
	EXPECT_EQ(startDateTime.year, 1);
	EXPECT_EQ(startDateTime.mon, 1);
	EXPECT_EQ(startDateTime.mday, 1);
	EXPECT_EQ(startDateTime.hour, 0);
	EXPECT_EQ(startDateTime.min, 0);
	EXPECT_EQ(startDateTime.sec, 0);
	EXPECT_EQ(startDateTime.isUTC, false);
	EXPECT_EQ(startDateTime.HasTimeZoneOffset(), false);
}

TEST(MetadataReading, DateTimeParse1Test)
{
	XmlDateTime dt;
	bool b = XmlDateTime::TryParse("2008-08-30T01:45:36.123Z", &dt);
	EXPECT_TRUE(b) << "Expecting a valid parsing.";
	EXPECT_EQ(dt.year, 2008);
	EXPECT_EQ(dt.mon, 8);
	EXPECT_EQ(dt.mday, 30);
	EXPECT_EQ(dt.hour, 1);
	EXPECT_EQ(dt.min, 45);
	EXPECT_EQ(dt.sec, 36);
	EXPECT_EQ(dt.isUTC, true);

	b = XmlDateTime::TryParse("2004-04-12T13:20:00+05:30", &dt);
	EXPECT_TRUE(b) << "Expecting a valid parsing.";
	EXPECT_EQ(dt.year, 2004);
	EXPECT_EQ(dt.mon, 4);
	EXPECT_EQ(dt.mday, 12);
	EXPECT_EQ(dt.hour, 13);
	EXPECT_EQ(dt.min, 20);
	EXPECT_EQ(dt.sec, 0);
	EXPECT_EQ(dt.isUTC, false);
	EXPECT_EQ(dt.HasTimeZoneOffset(), true);
	EXPECT_EQ(dt.offsetHours, 5);
	EXPECT_EQ(dt.offsetMinutes, 30);

	b = XmlDateTime::TryParse("20044-044-12", &dt);
	EXPECT_FALSE(b) << "Not expecting a valid parsing.";

	b = XmlDateTime::TryParse("2002-05-30T09:00:00", &dt);
	EXPECT_TRUE(b) << "Expecting a valid parsing.";
	EXPECT_EQ(dt.year, 2002);
	EXPECT_EQ(dt.mon, 5);
	EXPECT_EQ(dt.mday, 30);
	EXPECT_EQ(dt.hour, 9);
	EXPECT_EQ(dt.min, 0);
	EXPECT_EQ(dt.sec, 0);
	EXPECT_EQ(dt.isUTC, false);
	EXPECT_EQ(dt.HasTimeZoneOffset(), false);

	b = XmlDateTime::TryParse("20044-044-12T13:20:00+05:30", &dt);
	EXPECT_FALSE(b) << "Not expecting a valid parsing.";
}

TEST(MetadataReading, DimensionInfoChannels1Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data1);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto docInfo = md->GetDocumentInfo();

	auto channels = docInfo->GetDimensionChannelsInfo();
}

TEST(MetadataReading, DimensionInfoChannels2Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data5);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());

	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";

	auto docInfo = md->GetDocumentInfo();

	auto channels = docInfo->GetDimensionChannelsInfo();

	ASSERT_EQ(channels->GetChannelCount(), 3) << "Was expecting to find 3 channels.";

	auto ch1 = channels->GetChannel(0);
	SpectrumCharacteristics characteristics;
	bool b = ch1->TryGetIlluminationWavelength(&characteristics);
	ASSERT_TRUE(b && characteristics.type == SpectrumCharacteristics::InformationType::Ranges && characteristics.ranges.size() == 3);
	ASSERT_TRUE(characteristics.ranges[0].singleValue == false);
	EXPECT_DOUBLE_EQ(characteristics.ranges[0].startOrSingleValue, 400);
	EXPECT_DOUBLE_EQ(characteristics.ranges[0].end, 420);

	ASSERT_TRUE(characteristics.ranges[1].singleValue == false);
	EXPECT_DOUBLE_EQ(characteristics.ranges[1].startOrSingleValue, 460);
	EXPECT_DOUBLE_EQ(characteristics.ranges[1].end, 500);

	ASSERT_TRUE(characteristics.ranges[2].singleValue == true);
	EXPECT_DOUBLE_EQ(characteristics.ranges[2].startOrSingleValue, 600);

	auto ch2 = channels->GetChannel(1);
	b = ch2->TryGetIlluminationWavelength(&characteristics);
	EXPECT_FALSE(b);

	auto ch3 = channels->GetChannel(2);
	b = ch3->TryGetIlluminationWavelength(&characteristics);
	EXPECT_TRUE(b);
	ASSERT_TRUE(b && characteristics.type == SpectrumCharacteristics::InformationType::Ranges && characteristics.ranges.size() == 5);
	ASSERT_TRUE(characteristics.ranges[0].singleValue == false);
	EXPECT_TRUE(characteristics.ranges[0].startOrSingleValue <= (numeric_limits<double>::min)());
	EXPECT_DOUBLE_EQ(characteristics.ranges[0].end, 100);
	ASSERT_TRUE(characteristics.ranges[1].singleValue == false);
	EXPECT_DOUBLE_EQ(characteristics.ranges[1].startOrSingleValue, 400);
	EXPECT_DOUBLE_EQ(characteristics.ranges[1].end, 480);
	ASSERT_TRUE(characteristics.ranges[2].singleValue == false);
	EXPECT_DOUBLE_EQ(characteristics.ranges[2].startOrSingleValue, 500);
	EXPECT_DOUBLE_EQ(characteristics.ranges[2].end, 520);
	ASSERT_TRUE(characteristics.ranges[3].singleValue == true);
	EXPECT_DOUBLE_EQ(characteristics.ranges[3].startOrSingleValue, 600);
	ASSERT_TRUE(characteristics.ranges[4].singleValue == false);
	EXPECT_DOUBLE_EQ(characteristics.ranges[4].startOrSingleValue, 800);
	EXPECT_TRUE(characteristics.ranges[4].end <= (numeric_limits<double>::max)());
}

TEST(MetadataReading, DimensionInfoChannels3Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data5);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";
	auto docInfo = md->GetDocumentInfo();
	auto channels = docInfo->GetDimensionChannelsInfo();
	ASSERT_EQ(channels->GetChannelCount(), 3) << "Was expecting to find 3 channels.";

	auto ch1 = channels->GetChannel(0);
	RangeOrSingleValue<std::uint64_t> exposureTime;
	bool b = ch1->TryGetExposureTime(&exposureTime);
	ASSERT_TRUE(b);
	EXPECT_FALSE(exposureTime.singleValue);
	EXPECT_EQ(exposureTime.startOrSingleValue, 100);
	EXPECT_EQ(exposureTime.end, 2993);

	auto ch2 = channels->GetChannel(1);
	b = ch2->TryGetExposureTime(&exposureTime);
	ASSERT_TRUE(b);
	EXPECT_TRUE(exposureTime.singleValue);
	EXPECT_EQ(exposureTime.startOrSingleValue, 10099);

	auto ch3 = channels->GetChannel(2);
	b = ch3->TryGetExposureTime(&exposureTime);
	ASSERT_FALSE(b);
}

TEST(MetadataReading, DimensionInfoAiryScan1Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data6);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";
	auto docInfo = md->GetDocumentInfo();
	auto channels = docInfo->GetDimensionChannelsInfo();
	ASSERT_EQ(channels->GetChannelCount(), 1) << "Was expecting to find 1 channel.";

	auto ch1 = channels->GetChannel(0);
	auto airyScan = ch1->GetAiryscanSettings();
	ASSERT_TRUE(airyScan);
	auto fastAiryScan = airyScan->GetFastAiryScanSettings();
	ASSERT_TRUE(fastAiryScan);
	vector<int> fiberMapping;
	bool b = fastAiryScan->TryGetFiberMapping(&fiberMapping);
	ASSERT_TRUE(b);
	EXPECT_TRUE(
		fiberMapping.size() == 10 && fiberMapping[0] == 1 && fiberMapping[1] == 2 && fiberMapping[2] == 3 && fiberMapping[3] == 4 &&
		fiberMapping[4] == 5 && fiberMapping[5] == 6 && fiberMapping[6] == 7 && fiberMapping[7] == 8 && fiberMapping[8] == 10 &&
		fiberMapping[9] == 22);
	IFastAiryScanSettings::Vector v;
	b = fastAiryScan->TryGetIlluminationLinePixels(&v);
	ASSERT_TRUE(b);
	EXPECT_NEAR(v.x, 0, 0.0001);
	EXPECT_NEAR(v.y, 4, 0.0001);
	b = fastAiryScan->TryGetIlluminationLine(&v);
	ASSERT_TRUE(b);
	EXPECT_NEAR(v.x, 0, 0.0001);
	EXPECT_NEAR(v.y, 3.975, 0.0001);
	double val;
	b = fastAiryScan->TryGetEffectiveNAinX(&val);
	ASSERT_TRUE(b);
	EXPECT_NEAR(val, 0.43, 0.0001);
	b = fastAiryScan->TryGetEffectiveNAinY(&val);
	ASSERT_TRUE(b);
	EXPECT_NEAR(val, 0.38, 0.0001);
}

TEST(MetadataReading, DimensionInfoAtrribute1Test)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data6);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";
	auto docInfo = md->GetDocumentInfo();
	auto channels = docInfo->GetDimensionChannelsInfo();
	ASSERT_EQ(channels->GetChannelCount(), 1) << "Was expecting to find 1 channel.";

	auto ch1 = channels->GetChannel(0);
	ASSERT_TRUE(ch1);

	wstring id;
	bool b = ch1->TryGetAttributeId(&id);
	ASSERT_TRUE(b);
	EXPECT_TRUE(id.compare(L"Channel:1") == 0);
}