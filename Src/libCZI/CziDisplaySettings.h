//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
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

#pragma once
#include "libCZI.h"
#include "splines.h"
#include "pugixml.hpp"

class CChannelDisplaySettingsOnPod : public libCZI::IChannelDisplaySetting
{
private:
	libCZI::ChannelDisplaySettingsPOD cdsPod;
public:
	CChannelDisplaySettingsOnPod(const libCZI::ChannelDisplaySettingsPOD& pod)
		: cdsPod(pod)
	{}

public:	// interface IChannelDisplaySetting
	bool GetIsEnabled() const override;
	float GetWeight() const override;
	bool TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const override;
	void GetBlackWhitePoint(float* pBlack, float* pWhite) const override;
	libCZI::IDisplaySettings::GradationCurveMode GetGradationCurveMode() const override;
	bool TryGetGamma(float* gamma) const override;
	bool TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* ctrlPts) const override;
	bool TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const override;
};

class CDisplaySettingsOnPod : public libCZI::IDisplaySettings
{
private:
	std::map<int, std::shared_ptr<libCZI::IChannelDisplaySetting>> channelDs;
public:
	explicit CDisplaySettingsOnPod(std::function<bool(int no, int&, libCZI::ChannelDisplaySettingsPOD& dispSetting)> getChannelDisplaySettings);
	CDisplaySettingsOnPod(const libCZI::DisplaySettingsPOD& pod);

	static std::shared_ptr<libCZI::IDisplaySettings> CreateFromXml(pugi::xml_node node);
public:	// interface IDisplaySettings
	void EnumChannels(std::function<bool(int)> func) const override;
	std::shared_ptr<libCZI::IChannelDisplaySetting> GetChannelDisplaySettings(int chIndex) const override;
};

