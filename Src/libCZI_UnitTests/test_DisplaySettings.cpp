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

TEST(DisplaySettings, Test1)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";
	auto docInfo = md->GetDocumentInfo();
	auto displaySettings = docInfo->GetDisplaySettings();

	DisplaySettingsPOD pod;
	IDisplaySettings::Clone(displaySettings.get(), pod);

	EXPECT_TRUE(pod.channelDisplaySettings.size() == 5) << "Expected to have a size of 5.";

	for (size_t i = 0; i < pod.channelDisplaySettings.size(); ++i)
	{
		EXPECT_TRUE(pod.channelDisplaySettings[i].isEnabled == true) << "Expected the channel to be enabled";
		EXPECT_TRUE(pod.channelDisplaySettings[i].tintingMode == IDisplaySettings::TintingMode::Color) << "Expected the tinting mode to be 'Color'";
		EXPECT_TRUE(pod.channelDisplaySettings[i].gradationCurveMode == IDisplaySettings::GradationCurveMode::Linear) << "Expected the gradation-curve-mode to be 'Linear'";
	}
}

TEST(DisplaySettings, Test2)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>();
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	EXPECT_TRUE(md->IsXmlValid()) << "Expected valid XML.";
	auto docInfo = md->GetDocumentInfo();
	auto displaySettings = docInfo->GetDisplaySettings();

	DisplaySettingsPOD pod;
	IDisplaySettings::Clone(displaySettings.get(), pod);

	auto displaySettings2 = DisplaySettingsPOD::CreateIDisplaySettingSp(pod);

	auto chDs1 = displaySettings->GetChannelDisplaySettings(0);
	auto chDs2 = displaySettings2->GetChannelDisplaySettings(0);

	EXPECT_TRUE(chDs1->GetIsEnabled() == chDs2->GetIsEnabled()) << "Expected to have the same value.";
	EXPECT_TRUE(chDs1->GetWeight() == chDs2->GetWeight()) << "Expected to have the same value.";
	float bp1, bp2, wp1, wp2;
	chDs1->GetBlackWhitePoint(&bp1, &wp1);
	chDs2->GetBlackWhitePoint(&bp2, &wp2);
	EXPECT_TRUE(bp1 == bp2 && wp1 == wp2) << "Expected to have the same value.";
}

TEST(DisplaySettings, Test3)
{
	auto mockMdSegment = make_shared<MockMetadataSegment>(MockMetadataSegment::Type::Data2);
	auto md = CreateMetaFromMetadataSegment(mockMdSegment.get());
	ASSERT_TRUE(md->IsXmlValid()) << "Expected valid XML.";
	auto docInfo = md->GetDocumentInfo();
	auto displaySettings = docInfo->GetDisplaySettings();

	DisplaySettingsPOD pod;
	IDisplaySettings::Clone(displaySettings.get(), pod);

	auto displaySettings2 = DisplaySettingsPOD::CreateIDisplaySettingSp(pod);

	auto chDs1 = displaySettings->GetChannelDisplaySettings(1);
	auto chDs2 = displaySettings2->GetChannelDisplaySettings(1);

	EXPECT_TRUE(chDs1->GetIsEnabled() == chDs2->GetIsEnabled()) << "Expected to have the same value.";
	EXPECT_TRUE(chDs1->GetWeight() == chDs2->GetWeight()) << "Expected to have the same value.";
	float bp1, bp2, wp1, wp2;
	chDs1->GetBlackWhitePoint(&bp1, &wp1);
	chDs2->GetBlackWhitePoint(&bp2, &wp2);
	EXPECT_TRUE(bp1 == bp2 && wp1 == wp2) << "Expected to have the same value.";

	EXPECT_TRUE(chDs1->GetGradationCurveMode() == IDisplaySettings::GradationCurveMode::Spline &&
				chDs2->GetGradationCurveMode() == IDisplaySettings::GradationCurveMode::Spline) << "Expected to have the same value (=Spline).";

	vector< IDisplaySettings::SplineControlPoint> splineCtrlPts1, splineCtrlPts2;
	ASSERT_TRUE(chDs1->TryGetSplineControlPoints(&splineCtrlPts1) && chDs1->TryGetSplineControlPoints(&splineCtrlPts2)) << "Expected to find spline-control-points";
	EXPECT_THAT(splineCtrlPts1, ::testing::ContainerEq(splineCtrlPts2)) << "The data should have been equal";
}